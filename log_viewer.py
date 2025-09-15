import os
import sys
import time
import json
import re
from pathlib import Path
from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler

from PySide6.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout, 
                               QHBoxLayout, QListWidget, QTextEdit, QLabel, 
                               QComboBox, QPushButton, QFileDialog, QSplitter)
from PySide6.QtCore import Qt, QThread, Signal
from PySide6.QtGui import QTextCharFormat, QFont, QSyntaxHighlighter, QColor, QTextCursor


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
    """Syntax highlighter for log files with advanced formatting"""
    
    def __init__(self, parent=None):
        super().__init__(parent)
        
        # Define highlighting rules
        self.highlighting_rules = []
        
        # Timestamp patterns (blue)
        timestamp_format = QTextCharFormat()
        timestamp_format.setForeground(QColor(100, 150, 250))
        self.highlighting_rules.append((r'\[\s*\d{2}:\d{2}:\d{2}\s*\|', timestamp_format))
        
        # Frame number patterns (purple)
        frame_format = QTextCharFormat()
        frame_format.setForeground(QColor(180, 100, 240))
        self.highlighting_rules.append((r'\|\s*\d+\s*\]', frame_format))
        
        # CORE patterns (orange)
        core_format = QTextCharFormat()
        core_format.setForeground(QColor(255, 165, 0))
        core_format.setFontWeight(QFont.Bold)
        self.highlighting_rules.append((r'\tCORE\t', core_format))
        
        # APP patterns (green)
        app_format = QTextCharFormat()
        app_format.setForeground(QColor(50, 200, 100))
        app_format.setFontWeight(QFont.Bold)
        self.highlighting_rules.append((r'\tAPP\t', app_format))
        
        # File location patterns (gray)
        location_format = QTextCharFormat()
        location_format.setForeground(QColor(150, 150, 150))
        self.highlighting_rules.append((r'\([^:]+:\d+\):', location_format))
        
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
        
        # JSON patterns (special handling in highlightBlock)
        self.json_start = None
        self.json_stack = []
        
    def highlightBlock(self, text):
        """Apply highlighting to the given text block"""
        # First apply all regex rules
        for pattern, format in self.highlighting_rules:
            import re
            expression = re.compile(pattern)
            for match in expression.finditer(text):
                start, end = match.span()
                self.setFormat(start, end - start, format)
        
        # Special handling for JSON content
        self.highlight_json(text)
    
    def highlight_json(self, text):
        """Highlight JSON content with special formatting"""
        json_format = QTextCharFormat()
        json_format.setForeground(QColor(180, 180, 180))  # Light gray for JSON
        
        # Check if we're inside a JSON block
        if self.json_start is not None:
            # We're inside a JSON block, continue until we find the end
            end_pos = text.find('}')
            if end_pos != -1:
                # Found the end of JSON
                self.setFormat(0, len(text), json_format)
                self.json_start = None
            else:
                # Still inside JSON
                self.setFormat(0, len(text), json_format)
            return
        
        # Check if this line starts a JSON block
        if text.strip().startswith('{'):
            self.json_start = self.currentBlock().position()
            self.setFormat(0, len(text), json_format)
            
            # Also check if the JSON ends on the same line
            if text.strip().endswith('}'):
                self.json_start = None


class LogViewerApp(QMainWindow):
    """Main application window with enhanced log viewing and inline JSON formatting"""
    
    def __init__(self):
        super().__init__()
        
        # Hardcoded log directories for projects
        self.log_dirs = {
            "DuinEditor": "./DuinEditor/logs",
            "DuinFPS":  "./ExampleProjects/DuinFPS/logs",
        }
        
        # Current log directory
        self.current_dir = None
        
        # Initialize log watcher to None
        self.log_watcher = None
        
        # Initialize UI
        self.init_ui()
        
    def init_ui(self):
        """Initialize the user interface"""
        self.setWindowTitle('Advanced Log Viewer')
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
        
        # Format JSON button
        self.format_json_btn = QPushButton("Format JSON")
        self.format_json_btn.clicked.connect(self.format_json_in_log)
        project_layout.addWidget(self.format_json_btn)
        
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
        self.log_content.setLineWrapMode(QTextEdit.LineWrapMode.NoWrap)
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
                
            self.raw_content = content  # Store the raw content
            self.log_content.setPlainText(content)
            self.statusBar().showMessage(f"Loaded: {file_path}")
            
        except Exception as e:
            self.log_content.setPlainText(f"Error reading file: {str(e)}")
            self.statusBar().showMessage(f"Error: {str(e)}")
            
    def format_json_in_log(self):
        """Format JSON content directly within the log display"""
        if not hasattr(self, 'raw_content'):
            return
            
        content = self.raw_content
        formatted_content = content
        
        # Find all JSON blocks in the content
        json_blocks = []
        start_pos = 0
        
        while True:
            # Find the start of a JSON block
            start_idx = content.find('{', start_pos)
            if start_idx == -1:
                break
                
            # Find the matching end of the JSON block
            brace_count = 0
            in_string = False
            escape = False
            end_idx = -1
            
            for i in range(start_idx, len(content)):
                char = content[i]
                
                if escape:
                    escape = False
                    continue
                    
                if char == '\\':
                    escape = True
                elif char == '"':
                    in_string = not in_string
                elif not in_string:
                    if char == '{':
                        brace_count += 1
                    elif char == '}':
                        brace_count -= 1
                        if brace_count == 0:
                            end_idx = i
                            break
                            
            if end_idx != -1:
                json_str = content[start_idx:end_idx+1]
                try:
                    # Parse and format the JSON
                    parsed_json = json.loads(json_str)
                    formatted_json = json.dumps(parsed_json, indent=2)
                    
                    # Replace the original JSON with formatted JSON
                    formatted_content = formatted_content.replace(json_str, formatted_json)
                    
                    # Move to the next position
                    start_pos = end_idx + 1
                except json.JSONDecodeError:
                    # If JSON is invalid, skip this block
                    start_pos = start_idx + 1
            else:
                break
                
        # Update the log content with formatted JSON
        self.log_content.setPlainText(formatted_content)
        
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
