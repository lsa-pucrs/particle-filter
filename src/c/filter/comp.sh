#!/bin/bash
gcc -c -g  -I/usr/local/MATLAB/MATLAB_Production_Server/R2015a/extern/include -I/usr/local/MATLAB/MATLAB_Production_Server/R2015a/simulink/include -DMATLAB_MEX_FILE -D_GNU_SOURCE -fexceptions -I/usr/local/MATLAB/MATLAB_Production_Server/R2015a/extern/include -DMX_COMPAT_32 -O -DNDEBUG  matcreat.c
gcc -O  -o  "matcreat"  matcreat.o  -Wl,-rpath-link,/usr/local/MATLAB/MATLAB_Production_Server/R2015a/bin/glnxa64 -L/usr/local/MATLAB/MATLAB_Production_Server/R2015a/bin/glnxa64 -lmat -lmx -lm -lstdc++
