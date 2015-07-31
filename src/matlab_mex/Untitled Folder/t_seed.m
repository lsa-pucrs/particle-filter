clear all;clc;close all;

mex mexSeed.c
mex mexEncoder.c
mexSeed(); % Seed as time(NULL)

t_absolute_value()