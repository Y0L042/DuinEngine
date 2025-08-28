import os
import sys
import time
from pathlib import Path
from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler

from PySide6.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout, 
                               QHBoxLayout, QListWidget, QTextEdit, QLabel, 
                               QComboBox, QPushButton, QFileDialog, QSplitter)
from PySide6.QtCore import Qt, QThread, Signal
from PySide6.QtGui import QTextCharFormat, QFont, QSyntaxHighlighter, QColor


class LogWatcher(QThread):
    """Thread that watches for changes in log directories"""
    log_files_updated = Signal(list)
    
    def __init__(self, log_dirs):
        super().__init__()
        self.log_dirs = log_dirs
        self.observer = Observer()
        self.running = False
        
    def run(self):
        """Start watching directories for changes"""
        self.running = True
        
        class LogHandler(FileSystemEventHandler):
            def __init__(self, callback):
                self.callback = callback
                
            def on_created(self, event):
                if not event.is_directory:
                    self.callback()
                    
            def on_deleted(self, event):
                if not event.is_directory:
                    self.callback()
                    
            def on_modified(self, event):
                if not event.is_directory:
                    self.callback()
        
        # Create event handler that triggers update
        handler = LogHandler(self.update_log_files)
        
        # Schedule watching for each directory
        for directory in self.log_dirs:
            if os.path.exists(directory):
                self.observer.schedule(handler, directory, recursive=True)
        
        # Start the observer
        self.observer.start()
        
        # Update log files initially
        self.update_log_files()
        
        # Keep thread running
        while self.running:
            time.sleep(0.1)
            
    def update_log_files(self):
        """Collect all log files from watched directories"""
        log_files = []
        for directory in self.log_dirs:
            if os.path.exists(directory):
                for root, _, files in os.walk(directory):
                    for file in files:
                        if file.endswith('.log') or file.endswith('.txt'):
                            log_files.append(os.path.join(root, file))
        self.log_files_updated.emit(log_files)
        
    def stop(self):
        """Stop watching directories"""
        self.running = False
        self.observer.stop()
        self.observer.join()


class LogHighlighter(QSyntaxHighlighter):
    """Syntax highlighter for log files"""
    
    def __init__(self, parent=None):
        super().__init__(parent)
        
        # Define highlighting rules
        self.highlighting_rules = []
        
        # Error patterns (red)
        error_format = QTextCharFormat()
        error_format.setForeground(QColor(255, 0, 0))
        error_format.setFontWeight(QFont.Bold)
        self.highlighting_rules.append((r'.*ERROR.*|.*Error.*|.*error.*', error_format))
        
        # Warning patterns (yellow/orange)
        warning_format = QTextCharFormat()
        warning_format.setForeground(QColor(255, 165, 0))
        warning_format.setFontWeight(QFont.Bold)
        self.highlighting_rules.append((r'.*WARN.*|.*WARNING.*|.*Warning.*|.*warning.*', warning_format))
        
        # Info patterns (blue)
        info_format = QTextCharFormat()
        info_format.setForeground(QColor(65, 105, 225))
        self.highlighting_rules.append((r'.*INFO.*|.*Info.*|.*info.*', info_format))
        
        # Debug patterns (green)
        debug_format = QTextCharFormat()
        debug_format.setForeground(QColor(0, 128, 0))
        self.highlighting_rules.append((r'.*DEBUG.*|.*Debug.*|.*debug.*', debug_format))
        
        # Timestamp patterns (gray)
        timestamp_format = QTextCharFormat()
        timestamp_format.setForeground(QColor(128, 128, 128))
        self.highlighting_rules.append((r'\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}', timestamp_format))
        
    def highlightBlock(self, text):
        """Apply highlighting to the given text block"""
        for pattern, format in self.highlighting_rules:
            import re
            expression = re.compile(pattern)
            for match in expression.finditer(text):
                start, end = match.span()
                self.setFormat(start, end - start, format)


class LogViewerApp(QMainWindow):
    """Main application window"""
    
    def __init__(self):
        super().__init__()
        
        # Hardcoded log directories for projects
        self.log_dirs = {
            # "DuinEditor": str(Path.home() / "DuinEditor/logs"),
            # "DuinFPS": str(Path.home() / "ExampleProjects/DuinFPS/logs"),
            "DuinEditor": "./DuinEditor/logs",
            "DuinFPS": "./ExampleProjects/DuinFPS/logs",
        }
        
        # Current log directory
        self.current_dir = None
        
        # Initialize log watcher to None
        self.log_watcher = None
        
        # Initialize UI
        self.init_ui()
        
    def init_ui(self):
        """Initialize the user interface"""
        self.setWindowTitle('Log Viewer')
        self.setGeometry(100, 100, 1200, 800)
        
        # Central widget
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        
        # Main layout
        main_layout = QVBoxLayout(central_widget)
        
        # Project selection
        project_layout = QHBoxLayout()
        project_layout.addWidget(QLabel("Select Project:"))
        
        self.project_combo = QComboBox()
        self.project_combo.addItems(self.log_dirs.keys())
        self.project_combo.currentTextChanged.connect(self.select_project)
        project_layout.addWidget(self.project_combo)
        
        self.select_btn = QPushButton("Select")
        self.select_btn.clicked.connect(self.select_project)
        project_layout.addWidget(self.select_btn)
        
        project_layout.addStretch()
        main_layout.addLayout(project_layout)
        
        # Splitter for log list and content
        splitter = QSplitter(Qt.Orientation.Horizontal)
        
        # Log files list
        self.log_list = QListWidget()
        self.log_list.currentTextChanged.connect(self.load_log_file)
        splitter.addWidget(self.log_list)
        
        # Log content display
        self.log_content = QTextEdit()
        self.log_content.setReadOnly(True)
        self.highlighter = LogHighlighter(self.log_content.document())
        splitter.addWidget(self.log_content)
        
        # Set splitter sizes
        splitter.setSizes([300, 900])
        main_layout.addWidget(splitter)
        
        # Status bar
        self.statusBar().showMessage('Ready')
        
        # Initially select the first project
        if self.log_dirs:
            self.project_combo.setCurrentIndex(0)
            self.select_project()
        
    def select_project(self):
        """Select a project and start watching its log directory"""
        project_name = self.project_combo.currentText()
        self.current_dir = self.log_dirs[project_name]
        
        # Stop previous watcher if exists
        if self.log_watcher is not None:
            self.log_watcher.stop()
            self.log_watcher.wait()
        
        # Create directory if it doesn't exist
        os.makedirs(self.current_dir, exist_ok=True)
        
        # Start watching the directory
        self.log_watcher = LogWatcher([self.current_dir])
        self.log_watcher.log_files_updated.connect(self.update_log_list)
        self.log_watcher.start()
        
        self.statusBar().showMessage(f"Watching: {self.current_dir}")
        
    def update_log_list(self, log_files):
        """Update the list of log files"""
        self.log_list.clear()
        for file_path in sorted(log_files, reverse=True):  # Newest first
            file_name = os.path.basename(file_path)
            item = self.log_list.addItem(file_name)
            # Store the full path as item data
            self.log_list.item(self.log_list.count() - 1).setData(Qt.UserRole, file_path)
            
    def load_log_file(self, file_name):
        """Load and display the selected log file"""
        if not file_name:
            return
            
        # Find the item with this file name
        items = self.log_list.findItems(file_name, Qt.MatchFlag.MatchExactly)
        if not items:
            return
            
        item = items[0]
        file_path = item.data(Qt.UserRole)  # Get the full path from item data
        
        try:
            with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
                
            self.log_content.setPlainText(content)
            self.statusBar().showMessage(f"Loaded: {file_path}")
            
        except Exception as e:
            self.log_content.setPlainText(f"Error reading file: {str(e)}")
            self.statusBar().showMessage(f"Error: {str(e)}")
            
    def closeEvent(self, event):
        """Clean up when closing the application"""
        if self.log_watcher is not None:
            self.log_watcher.stop()
            self.log_watcher.wait()
        event.accept()


if __name__ == '__main__':
    app = QApplication(sys.argv)
    viewer = LogViewerApp()
    viewer.show()
    sys.exit(app.exec())
