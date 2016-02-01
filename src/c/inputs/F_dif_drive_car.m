function dx = F_dif_drive_car(t,x,vr,vl,R,L)
% ----------------------------------------------------------------------
%
%F_dif_drive_car(t,x,vr,vl,R,L) Simulate a differential drive car
%
%   Receives the initial pose p0 = [x0 y0 th0], the velocities of the 
% wheels and the simulation time "T". Returns, the car pose p = [x y th].
%
% Example:
%
%   x=x0; y=y0; th=th0;
%   options = odeset('RelTol',1e-3);
%   for k = 1:ttotal/T
%     [t,cont] = ode23(@F_dif_drive_car,[0 T],[x(k) y(k) th(k)],options,vr,vl,R,L);
%     x(k+1) = cont(end,1); y(k+1) = cont(end,2); th(k+1) = cont(end,3);
%   end
%
% ----------------------------------------------------------------------

dx = zeros(3,1);
    dx(1) = R/2*(vr+vl)*cos(x(3));
    dx(2) = R/2*(vr+vl)*sin(x(3));    
    dx(3) = R/L*(vr-vl);
end

