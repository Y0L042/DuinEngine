#!/usr/bin/env python3
"""
Simple build packaging script with hardcoded relative paths.
Usage: python build.py [ENGINE] [EDITOR] [DEMO]
"""

import os
import shutil
import subprocess
import sys
from pathlib import Path
from datetime import datetime

# Hardcoded relative build paths - UPDATE THESE TO MATCH YOUR ACTUAL PATHS
BUILD_PATHS = {
    "ENGINE": "Duin/bin/Debug-windows-x86_64/Duin",
    "EDITOR": "DuinEditor/bin/Debug-windows-x86_64/DuinEditor", 
    "DEMO": "ExampleProjects/DuinFPS/bin/Debug-windows-x86_64/DuinFPS"
}

# Base output directory (relative to script location)
OUTPUT_BASE_DIR = "Builds"

def get_git_info():
    """Get git branch and commit information"""
    try:
        # Get current branch
        branch = subprocess.check_output(
            ['git', 'rev-parse', '--abbrev-ref', 'HEAD'],
            stderr=subprocess.DEVNULL
        ).decode('utf-8').strip()
        
        # Get latest commit hash
        commit_hash = subprocess.check_output(
            ['git', 'rev-parse', 'HEAD'],
            stderr=subprocess.DEVNULL
        ).decode('utf-8').strip()
        
        # Get commit message
        commit_message = subprocess.check_output(
            ['git', 'log', '-1', '--pretty=%B'],
            stderr=subprocess.DEVNULL
        ).decode('utf-8').strip()
        
        # Get commit date
        commit_date = subprocess.check_output(
            ['git', 'log', '-1', '--format=%cd', '--date=iso'],
            stderr=subprocess.DEVNULL
        ).decode('utf-8').strip()
        
        return {
            'branch': branch,
            'commit_hash': commit_hash,
            'commit_message': commit_message,
            'commit_date': commit_date
        }
    except (subprocess.CalledProcessError, FileNotFoundError):
        return None

def create_git_info_file(output_dir, git_info):
    """Create a file with git information in the output directory"""
    git_info_file = output_dir / "git_info.txt"
    
    with open(git_info_file, 'w') as f:
        f.write("Build Information\n")
        f.write("=================\n\n")
        f.write(f"Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n\n")
        
        if git_info:
            f.write("Git Information:\n")
            f.write(f"  Branch: {git_info['branch']}\n")
            f.write(f"  Commit: {git_info['commit_hash']}\n")
            f.write(f"  Date: {git_info['commit_date']}\n")
            f.write(f"  Message: {git_info['commit_message']}\n")
        else:
            f.write("Git Information: Not available (not a git repo or git not installed)\n")
    
    return git_info_file

def copy_build_files(project_name, build_path, output_dir):
    """Copy all files from build directory to output directory"""
    # Convert relative path to absolute path
    source_dir = Path(build_path).resolve()
    
    if not source_dir.exists():
        print(f"Warning: Build path does not exist: {source_dir}")
        return False
    
    print(f"Copying {project_name} from {source_dir}...")
    
    # Copy all files and directories
    if source_dir.is_file():
        # If it's a file, copy it directly
        shutil.copy2(source_dir, output_dir)
    else:
        # If it's a directory, copy everything
        shutil.copytree(source_dir, output_dir, dirs_exist_ok=True)
    
    print(f"Copied {project_name} to {output_dir}")
    return True

def main():
    """Main function"""
    # Get the directory where this script is located
    script_dir = Path(__file__).parent
    os.chdir(script_dir)  # Change to script directory
    
    # Parse command line arguments
    projects_to_copy = [arg.upper() for arg in sys.argv[1:]]
    
    # If no projects specified, copy all
    if not projects_to_copy:
        projects_to_copy = list(BUILD_PATHS.keys())
        print("No projects specified, copying all projects")
    else:
        # Validate project names
        valid_projects = []
        for project in projects_to_copy:
            if project in BUILD_PATHS:
                valid_projects.append(project)
            else:
                print(f"Warning: Unknown project '{project}', skipping")
        
        projects_to_copy = valid_projects
        
        if not projects_to_copy:
            print("No valid projects specified. Available projects: ENGINE, EDITOR, DEMO")
            return
    
    # Create base output directory if it doesn't exist
    base_output_dir = Path(OUTPUT_BASE_DIR)
    base_output_dir.mkdir(exist_ok=True)
    
    # Get git information
    git_info = get_git_info()
    
    # Process each project
    for project_name in projects_to_copy:
        # Create project-specific output directory
        project_output_dir = base_output_dir / project_name
        project_output_dir.mkdir(exist_ok=True)
        
        # Create timestamped output directory for this project
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        timestamped_output_dir = project_output_dir / f"build_{timestamp}"
        timestamped_output_dir.mkdir(exist_ok=True)
        
        print(f"Output directory for {project_name}: {timestamped_output_dir.absolute()}")
        
        # Copy build files
        build_path = BUILD_PATHS[project_name]
        success = copy_build_files(project_name, build_path, timestamped_output_dir)
        
        if success:
            # Create git info file in the timestamped directory
            create_git_info_file(timestamped_output_dir, git_info)
            print(f"Created git info for {project_name}")
    
    print(f"\nBuild packaging completed!")
    print(f"Copied projects: {', '.join(projects_to_copy)}")

if __name__ == "__main__":
    main()
