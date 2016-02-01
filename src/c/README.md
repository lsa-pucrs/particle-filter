# INSTRUCTIONS

## Prerequisites
 * Have MATLAB installed <<<<
 * Have MAT File I/O Library installed (version 1.5.x) <<<<

## Usage

1. In MATLAB, go to the "inputs" folder and run the "main_mex.m" file. This will create four ".mat" files: car, dynamic_param, final and set_value.
2. Copy those files to the "filter" folder.
3. Modify the "comp.sh" file to include the right include paths (e.g. -I<your_matlab_installation_folder>/extern/include).
4. Compile the filter code "matcreat.c" with the command "sh comp.sh".
5. Run the filter with "./matcreat". This will generate a single file, "t_file.mat".
6. Copy this file to the "outputs" folder.
7. In MATLAB, go to the "outputs" folder and run the "mat_results.m" file. This will display the filter results in MATLAB.
