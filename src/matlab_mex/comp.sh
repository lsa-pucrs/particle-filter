#!/bin/bash
#gcc -c -g -lmatio -lz -I/usr/local/MATLAB/R2014a/extern/include -I/usr/local/MATLAB/R2014a/simulink/include -DMATLAB_MEX_FILE -ansi -D_GNU_SOURCE  -fexceptions -I/usr/local/MATLAB/R2014a/extern/include  -DMX_COMPAT_32 -O -DNDEBUG  matcreat.c
gcc -c -g -lmatio -lz -I/usr/local/MATLAB/R2014a/extern/include -I/usr/local/MATLAB/R2014a/simulink/include -DMATLAB_MEX_FILE -D_GNU_SOURCE  -fexceptions -I/usr/local/MATLAB/R2014a/extern/include  -DMX_COMPAT_32 -O -DNDEBUG  matcreat.c
gcc -O  -o  "matcreat"  matcreat.o  -Wl,-rpath-link,/usr/local/MATLAB/R2014a/bin/glnxa64 -L/usr/local/MATLAB/R2014a/bin/glnxa64 -lmat -lmx -lm -lstdc++ -lmatio -lz
