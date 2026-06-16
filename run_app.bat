@echo off

:: 1. Prompt the user for input
set /p user_string="Enter the name of the study: "

:: 2. Execute the .exe file with the input string
echo Performing calculations and storing results
"transient_calc.exe" "%user_string%"

:: 3. Execute the Python script with the same input string
echo Running data visualization routine
python data_vis.py "%user_string%"

pause