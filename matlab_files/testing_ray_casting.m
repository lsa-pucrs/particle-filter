% test the car on a map 
clear all;clc;
%% map
% The map is typed in as a 24x30 matrix
mapstreacth = 10; % so we dont have to type in a 240x300 matrix
map = F_load_map(mapstreacth);
mapscale  = 120; % each grid in 'map' represents 1/mapscale meters

%% parameters
ttotal = 270;
T  = .5;     % sampling time
R  = 0.02;   % wheel radius 2cm
L  = 0.08;   % distance between wheels 8cm
N  = 48;     % encoder ticks per rotation (must be a multiple of 720)
max_range = 1;
angles = [0 90 -90 45 -45]*pi/180;

%% initial conditions
x  = .2;       
y  = .5;
th = 0;

vr = 1;
vl = 1;

%% simulation
options = odeset('RelTol',1e-3);
fig = 1;
for k = 1:ttotal/T 
%% path
     if k == 10  vl=0.7;  end
     if k == 60  vl=1.0;  end
     if k == 120 vr=0.7;  end
     if k == 170 vr=1.0;  end
     if k == 310 vr=0.5;  end
     if k == 345 vr=1.0;  end
     if k == 430 vl=0.7;  end
     if k == 488 vl=1.001;  end

%% actual car   
   [t,cont] = ode23(@F_dif_drive_car,[0 T],[x(k) y(k) th(k)],options,vr,vl,R,L);
   x(k+1) = cont(end,1); y(k+1) = cont(end,2); th(k+1) = cont(end,3);
      
%% ray casting
   %sonars = F_ray_casting(x(k+1),y(k+1),th(k+1),map,max_range,angles,mapscale,max_range*mapscale);
   sonars = Fast_ray_cast(x(k+1),y(k+1),th(k+1),map,max_range,angles,mapscale,1);%
   
%% animation
   F_anima_map([x(k+1)*mapscale y(k+1)*mapscale th(k+1)], L*mapscale, fig, map,sonars*mapscale,angles)    
    
end

%% results
F_anima_map([x(end)*mapscale y(end)*mapscale th(end)], L*mapscale, fig, map,sonars*mapscale,angles)    

plot(x(1:end-5)*mapscale, y(1:end-5)*mapscale,'k','Color',[0.6 0.6 0.6],'LineWidth',2.5)






