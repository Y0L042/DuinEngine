#!/bin/sh

# Call the premake executable with vs2022 as an argument
vendor/bin/premake/premake5 vs2022

# Pause by prompting the user to press a key
read -p "Press any key to continue..."

