function p = F_sample_odometry(p,bar_p,a)
% ----------------------------------------------------------------------
%
%F_sample_odometry(p,bar_p,a) Table 5.6 of "Probabilistic Robotics,"
%                                                        by S.Thrun, et.al. 
%
%  This function receives the last position of a particle and the
%odometry measurements of each wheel and returns the future position of the
%particle:
%
% p         -> previous particle pose p(1)=x, p(2)=y and p(3)=th.
% bar_p     -> estimate of future pose
% a         -> vector of 4 variances (uncertainty)
%      a(1) - effect of Rotation    on Rotations
%      a(2) - effect of Translation on Rotations
%      a(3) - effect of Translation on Translation
%      a(4) - effect of Rotation    on Translation
%
% ----------------------------------------------------------------------


rot_1 = atan2(bar_p(2)-p(2),bar_p(1)-p(1))-p(3);
trans = sqrt((bar_p(2)-p(2))^2 + (bar_p(1)-p(1))^2);
rot_2 = bar_p(3) - p(3) - rot_1;

drot_1 = rot_1 - (a(1)*rot_1^2 + a(2)*trans^2)*randn(1);
dtrans = trans - (a(3)*trans^2 + a(4)*(rot_1^2 + rot_2^2))*randn(1);
drot_2 = rot_2 - (a(1)*rot_2^2 + a(2)*trans^2)*randn(1);

p(1) = p(1) + dtrans*cos(p(3)+drot_1);
p(2) = p(2) + dtrans*sin(p(3)+drot_1);
p(3) = p(3) + drot_1 + drot_2;


end


%drot_1 = rot_1 - F_sample_normal(a(1)*rot_1^2 + a(2)*trans^2);
%dtrans = trans - F_sample_normal(a(3)*trans^2 + a(4)*(rot_1^2 + rot_2^2));
%drot_2 = rot_2 - F_sample_normal(a(1)*rot_2^2 + a(2)*trans^2);
