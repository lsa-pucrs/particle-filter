function bar_p = F_estimate_p(p,dtick_L,dtick_R,L,N,R)
%% Calculate wheel displacement
Dl = 2*pi*R*dtick_L/N;
Dr = 2*pi*R*dtick_R/N;
Dc = (Dl+Dr)/2;

%% Calculate [X,Y,TH] coordinates
bar_p(3) = p(3) + (Dr-Dl)/L;
bar_p(1) = p(1) + Dc*cos((p(3)+bar_p(3))/2);
bar_p(2) = p(2) + Dc*sin((p(3)+bar_p(3))/2);

end

