%% Function to plot filter time between MATLAB and MEX
% Goes from 100 to 1000 particles, at 100 increments, and then plots.
% Change the "Ni" and "Nf" parameters to change the particle interval.
% Change the "Nstep" parameter to change the particle increment.

clear all; clc; close all;

% Parameters
Ni = 100;
Nstep = 100;
Nf = 1000;
matlab_times = (Ni:Nstep:Nf)/Nstep;
mex_times = (Ni:Nstep:Nf)/Nstep;

% Iteration loop
for i=Ni:Nstep:Nf
    disp(['Running MATLAB with ', int2str(i), ' particles']);
    matlab_times(i/Nstep) = main(i);
    
    disp(['Running MEX with', int2str(i), ' particles']);
    mex_times(i/Nstep) = main_mex(i);
end

% Plotting
figure
hold on
plot(Ni:Nstep:Nf,matlab_times,'LineWidth',2);
plot(Ni:Nstep:Nf,mex_times,'LineWidth',2);
xlabel('Número de Partículas');
ylabel('Tempo (s)');
legend('MATLAB','MEX','Location', 'Northwest');
hold off