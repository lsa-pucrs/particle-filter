% test the car on a map 
clear all;clc;close all;

%% Build mex
mex mexFP.c
mex mexSeed.c
mex mexEncoder.c
mex mexFastRayCast.c
%% Set the random seeds
mexSeed(); % Seed as time(NULL)

diary diary_8.txt

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
covsonars = 0.5;

%% initial conditions
x  = .2;       
y  = .5;
th = 0;
odometry = [x y th]';

vr = 1;
vl = 1;

%% Particle Filter parameters
alpha     = [2 50 50 2]*1;
CovSonars = 0.5;
global p
Np = input('Digite o numero de particulas: ');
w(1:Np) = 1;
p = zeros(3,Np);
w = ones (1,Np);
varx = .01;
vary = .01;
vart = .01;

%Start all particles on the right position
p = fdraw_p(length(p),varx,vary,vart,map,x,y,th,mapscale);
%Start all particles randomly
%p = fdraw_p(length(p),2,2,1,map,x+1,y+0.5,th,mapscale);
P = [sum(p(1,:)) sum(p(2,:)) sum(p(3,:))]'/Np;
fig = 1;

if 0 % to plot, or not to plot?
    F_anima_map([x(end)*mapscale y(end)*mapscale th(end)], L*mapscale, fig, map,0,angles)    
    hold on
    plot(x*mapscale, y*mapscale,'k','Color',[0.6 0.6 0.6],'LineWidth',2.5)
    plot(x*mapscale, y*mapscale,'k','Color',[0.6 0.6 0.6],'LineWidth',2.5)
    grid on
    for i = 1:size(p,2)       
        plot(p(1,i)*mapscale,p(2,i)*mapscale,'k.')
        plot([p(1,i) p(1,i)+0.02*cos(p(3,i))]*mapscale,[p(2,i) p(2,i)+0.02*sin(p(3,i))]*mapscale,'k');
    end 
    return
end

%% simulation
Pplot = [p];
plot_time = [];
plot_sonar= [];
options = odeset('RelTol',1e-3);
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
   sonars = mexFastRayCast(x(k+1),y(k+1),th(k+1),map,max_range,angles,mapscale,1, covsonars);
   see_sonar(:,k) = sonars;

%% encoders
   
   [dtick_L dtick_R] = mexEncoder( x(k+1)-x(k),y(k+1)-y(k),th(k+1)-th(k),L,N,R);
   %[dtick_L dtick_R] = F_encoders( x(k+1)-x(k),y(k+1)-y(k),th(k+1)-th(k),L,N,R);
   % this if perturbs the encoder measurements
   if rem(k,50)==0
       dtick_L = dtick_L + 1;
   end
   odometry(:,k+1) = F_estimate_p(odometry(:,k),dtick_L,dtick_R,L,N,R); 
   hold on
%% particle filter
    [p, w, P, CovSonars] = mexFP(p,dtick_L,dtick_R,L,N,R,alpha,map,max_range,angles,mapscale,1,sonars,CovSonars,w,k);                
   
%% animation
    if 0%to anime or not to anime
        F_anima_map([x(k+1)*mapscale y(k+1)*mapscale th(k+1)], L*mapscale, fig, map,sonars*mapscale,angles)                   
        for i = 1:size(p,2)       
            plot(p(1,i)*mapscale,p(2,i)*mapscale,'b.')    
        end     
        pause(eps)
    end
    
%% results    
    if rem(k,60)==0
      plot_sonar = [plot_sonar sonars];  
      Pplot = [Pplot p];           
      plot_time = [plot_time k]
    end     
end
Pplot = [Pplot p];     

%% results dynamic
d = 0.05;
fig = 1;
F_anima_map([x(end)*mapscale y(end)*mapscale th(end)], L*mapscale, fig, map,0*see_sonar(:,1)*mapscale,angles)
plot(x(1:end-5)*mapscale, y(1:end-5)*mapscale,'k','Color',[0.6 0.6 0.6],'LineWidth',2.5)
for i = 1:length(Pplot)
    plot(Pplot(1,i)*mapscale,Pplot(2,i)*mapscale,'k.')
    line([Pplot(1,i) Pplot(1,i)+d*cos(Pplot(3,i))]*mapscale,[Pplot(2,i) Pplot(2,i)+d*sin(Pplot(3,i))]*mapscale,'Color','k');
end
plot(odometry(1,:)*mapscale,odometry(2,:)*mapscale,'--','Color',[0.5 0.5 0.5])
plot([odometry(1,1) odometry(1,1)+0.05*cos(odometry(3,1))]*mapscale,[odometry(2,1) odometry(2,1)+0.05*sin(odometry(3,1))]*mapscale,'Color',[0.3 0.3 0.3]);
plot(odometry(1,end)*mapscale,odometry(2,end)*mapscale,'.','Color',[0.3 0.3 0.3],'LineWidth',1.5)
plot([odometry(1,end) odometry(1,end)+d*cos(odometry(3,end))]*mapscale,[odometry(2,end) odometry(2,end)+d*sin(odometry(3,end))]*mapscale,'Color',[0.3 0.3  0.3],'LineWidth',1.5)
return
