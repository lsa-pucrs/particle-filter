function [ wp ] = F_measurProb( x, mu, sigma)
%MEASURPROB ( x, mu, sigma)
%   Measures the probability of a variable "x" given it's normal curve, i.e., 
% "mu" and "sigma".
%
%  wp = exp(-((mu-x)^2)/(2*sigma^2))/sqrt(2*pi*(sigma^2));
%
%

wp = exp(-((x-mu)^2)/(2*sigma^2)) / sqrt(2*pi*sigma^2);
end 

