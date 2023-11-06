@echo off
setlocal enabledelayedexpansion

:: Use the current working directory as the source and output directory
set "source_dir=%cd%"
set "output_dir=%cd%\csv_files"

:: Create the output directory if it doesn't exist
if not exist "!output_dir!" (
    mkdir "!output_dir!"
)

:: Define the list of .tbl files to process
set "files=customer.tbl lineitem.tbl nation.tbl orders.tbl part.tbl partsupp.tbl region.tbl supplier.tbl"

:: Loop through each file in the list
for %%f in (%files%) do (
    set "input_file=!source_dir!\%%f"
    set "output_file=!output_dir!\%%~nf.csv"
    
    echo Processing: !input_file!
    
    if not exist "!input_file!" (
        echo !input_file! not found.
    ) else (
        for /f "usebackq tokens=*" %%a in ("!input_file!") do (
            set "line=%%a"
            echo !line:~0,-1!>> "!output_file!"
        )
    )
)

echo Conversion and clean up complete.

endlocal
