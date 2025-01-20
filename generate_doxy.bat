@echo off
echo =========================================================
echo Running Doxygen on specified Doxyfiles
echo =========================================================
echo.

rem -- Project #1 : Duin --
echo === Generating docs for Project #1 ===
doxygen ".\Duin\Doxyfile"

echo.
echo All done! Press any key to exit...
pause
