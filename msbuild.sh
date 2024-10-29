#!/bin/bash

# List of project names and their paths
declare -A projects=(
    ["DuinEditor"]="DuinEditor/DuinEditor.vcxproj"
    ["Sandbox"]="ExampleProjects/Sandbox/Sandbox.vcxproj"
)

# Function to display usage information
function usage() {
    echo "Usage: $0 [build|build-all|clean|clean-all] <project_name>"
    echo "  build <project_name>   - Build the specified project"
    echo "  build-all              - Build all projects"
    echo "  clean <project_name>   - Clean the specified project"
    echo "  clean-all              - Clean all projects"
    echo "  Available projects:"
    for project in "${!projects[@]}"; do
        echo "    - $project"
    done
    exit 1
}

# Function to build a specific project with configuration and platform
function build_project() {
    local project=$1
    msbuild.exe "${projects[$project]}" -property:Configuration=Debug -property:Platform=x64
    if [ $? -eq 0 ]; then
        echo "Build succeeded for $project."
    else
        echo "Build failed for $project."
        exit 1
    fi
}

# Function to clean a specific project with configuration and platform
function clean_project() {
    local project=$1
    msbuild.exe "${projects[$project]}" -target:Clean -property:Configuration=Debug -property:Platform=x64
    if [ $? -eq 0 ]; then
        echo "Clean succeeded for $project."
    else
        echo "Clean failed for $project."
        exit 1
    fi
}

# Function to build all projects
function build_all_projects() {
    for project in "${!projects[@]}"; do
        echo "Building $project..."
        build_project "$project"
    done
}

# Function to clean all projects
function clean_all_projects() {
    for project in "${!projects[@]}"; do
        echo "Cleaning $project..."
        clean_project "$project"
    done
}

# Check if at least one argument is provided
if [ $# -lt 1 ]; then
    usage
fi

# Parse the command (build, build-all, clean, or clean-all)
command=$1
project=$2

case $command in
    build)
        # Check if the project name is specified and exists in the list
        if [ -z "$project" ] || [ -z "${projects[$project]}" ]; then
            echo "Error: Specify a valid project to build."
            usage
        fi
        build_project "$project"
        ;;
    build-all)
        build_all_projects
        ;;
    clean)
        # Check if the project name is specified and exists in the list
        if [ -z "$project" ] || [ -z "${projects[$project]}" ]; then
            echo "Error: Specify a valid project to clean."
            usage
        fi
        clean_project "$project"
        ;;
    clean-all)
        clean_all_projects
        ;;
    *)
        usage
        ;;
esac
