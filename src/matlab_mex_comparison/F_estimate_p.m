function bar_p = F_estimate_p(p,dtick_L,dtick_R,L,N,R)
% ----------------------------------------------------------------------
%
%F_estimate_p(p,dtick_L,dtick_R,L,N,R)  This function receives the last 
%      position of a particle and the odometry measurements of each wheel 
%      and returns the future position of the particle.
%
% p        -> particle pose p(1)=x, p(2)=y and p(3)=th.
% dticks_L -> number of ticks in Left wheel (since last measurement)
% dticks_R -> number of ticks in Left wheel (since last measurement)
% delta    -> vector of covariances related to the variability of draws
%
% ----------------------------------------------------------------------

Dl = 2*pi*R*dtick_L/N;
Dr = 2*pi*R*dtick_R/N;
Dc = (Dl+Dr)/2;

bar_p(3) = p(3) + (Dr-Dl)/L;
bar_p(1) = p(1) + Dc*cos((p(3)+bar_p(3))/2);
bar_p(2) = p(2) + Dc*sin((p(3)+bar_p(3))/2);
end

