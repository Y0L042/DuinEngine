#!/usr/bin/env bash

echo "========================================================="
echo "Running Doxygen on specified Doxyfiles"
echo "========================================================="
echo

# -- Project #1 : Duin --
echo "=== Generating docs for Project #1 ==="
# If you are in WSL or Cygwin, convert the Windows path to a Unix-like path, for example:
# doxygen "/mnt/c/Projects/CPP_Projects/Duin/Duin/doxygen/Doxyfile"
# Otherwise, if your environment understands backslashes, adjust accordingly:
doxygen "./Duin/Doxyfile"

echo
echo "All done!"

# Equivalent to 'pause' in Windows:
read -r -p "Press [ENTER] to exit..."
