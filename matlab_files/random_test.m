clc; clear all; clf;

mex mexSeed.c
mex mexPrintRand1.c
mex mexPrintRand2.c

%% Set the random seed
mexSeed(200);

%% Print the random numbers
for i=1:10
    mexPrintRand1();
    mexPrintRand2();
end