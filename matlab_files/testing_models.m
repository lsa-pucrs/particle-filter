%this is a test file
clear all;clc;
%% parameters

ttotal = 30;
T  = .5;   % sampling time
R  = 0.02;   % wheel radius 2cm
L  = 0.08;   % distance between wheels 8cm
N  = 48;     % encoder ticks per rotation (must be a multiple of 720)

% initial conditions
x0  = 0.3;       
y0  = 0.3;
th0 = 0;
%% Simulates the car
vr = 1.2;
vl = 1;

x=x0; y=y0; th=th0;
options = odeset('RelTol',1e-3);
for k = 1:ttotal/T
      % if k == (ttotal/T)/4   vl=1.0;  end
      % if k == (ttotal/T)/2   vl=2.0;  end
      % if k == (ttotal/T)*3/4 vr=1.0;  end
   [t,cont] = ode23(@F_dif_drive_car,[0 T],[x(k) y(k) th(k)],options,vr,vl,R,L);
   x(k+1) = cont(end,1); y(k+1) = cont(end,2); th(k+1) = cont(end,3);
end

%return
%% Senses the encoders
% initial rotation of wheels
angL=0;
angR=0;
% initial tick count
tick_L = 0;
tick_R = 0;
for k = 1:ttotal/T     
       [dtick_L dtick_R] = F_encoders( x(k+1)-x(k),y(k+1)-y(k),th(k+1)-th(k),L,N,R);
       tick_L = tick_L + dtick_L;
       tick_R = tick_R + dtick_R;
       Ticks(k,:) = [ tick_L tick_R];
end
Ticks

%return
%% Computing the odometry
odometry = [x0 y0 th0]';
for k = 1:ttotal/T
    [dtick_L dtick_R] = F_encoders( x(k+1)-x(k),y(k+1)-y(k),th(k+1)-th(k),L,N,R);
     odometry(:,k+1) = F_estimate_p(odometry(:,k),dtick_L,dtick_R,L,N,R); 
end

%return
%% Animates the car and odometry
axxis= [-0.5 2 -0.5 1.5];
scale = 10;
map = F_load_map(scale);
fig = 1;
for k = 1:ttotal/T
   F_anima([x(k) y(k) th(k)], L, fig, axxis)  
   %F_anima_map([x(k) y(k) th(k)]*scale, L, fig, map)  
end
plot(x,y)
plot(odometry(1,1),odometry(2,1),'o')
plot([odometry(1,1) odometry(1,1)+0.03*cos(odometry(3,1))],[odometry(2,1) odometry(2,1)+0.03*sin(odometry(3,1))],'b');
plot(odometry(1,end),odometry(2,end),'.k','LineWidth',2)
plot([odometry(1,end) odometry(1,end)+0.03*cos(odometry(3,end))],[odometry(2,end) odometry(2,end)+0.03*sin(odometry(3,end))],'k');

return
%% Identifying the uncertainty 
% This is done for a very large step, just to get a grasp of the meaning of
% the parameters in "alpha"
alpha=[0.1 0.01 0.05 0.01];

global p
Np = 50;
p=zeros(3,Np);
for i=1:length(p)
    p(:,i) = [x0+(rand(1)-0.5)*0 y0+(rand(1)-0.5)*0 th0+(rand(1)-0.5)*0]';
end

clf
hold on
for i = 1:Np
    p(:,i)     = F_sample_odometry(odometry(:,1),odometry(:,end),alpha); 
    plot(p(1,i),p(2,i),'k.')
    plot([p(1,i) p(1,i)+0.02*cos(p(3,i))],[p(2,i) p(2,i)+0.02*sin(p(3,i))],'k');
end
axis equal
axis(axxis)
plot(x,y)
plot(odometry(1,1),odometry(2,1),'o')
plot([odometry(1,1) odometry(1,1)+0.03*cos(odometry(3,1))],[odometry(2,1) odometry(2,1)+0.03*sin(odometry(3,1))],'b');
plot(odometry(1,end),odometry(2,end),'.k','LineWidth',2)
plot([odometry(1,end) odometry(1,end)+0.03*cos(odometry(3,end))],[odometry(2,end) odometry(2,end)+0.03*sin(odometry(3,end))],'k');
axis(axxis)
grid on
pause(2)

return
%% Testing Everything on a longer path
alpha=[5 10 20 5];
global p
Np = 50;
p=zeros(3,Np);

ttotal = 252;
x0=0; y0=0; th0=0;
for i=1:length(p)
    p(:,i) = [x0 y0 th0]';
end
Pplot = [0 0 0]';

vr = 1;
vl = 1;

x=x0; y=y0; th=th0;
odometry = [x y th]';

options = odeset('RelTol',1e-3);
fig = 1;
for k = 1:ttotal/T
    
    %path
     if k == 10  vl=0.7;  end
     if k == 60  vl=1.0;  end
     if k == 120 vr=0.7;  end
     if k == 170 vr=1.0;  end
     if k == 280 vr=0.5;  end
     if k == 320 vr=1.0;  end
     if k == 400 vl=0.7;  end
     if k == 467 vl=1.0;  end
     %if k == (ttotal/T)*3/4 vr=0.8;  end
   
   %actual car   
   [t,cont] = ode23(@F_dif_drive_car,[0 T],[x(k) y(k) th(k)],options,vr,vl,R,L);
   x(k+1) = cont(end,1); y(k+1) = cont(end,2); th(k+1) = cont(end,3);
   
   %encoders
   [dtick_L dtick_R] = F_encoders( x(k+1)-x(k),y(k+1)-y(k),th(k+1)-th(k),L,N,R);
   % this if perturbs the encoder measurements
   if rem(k,50)==0
       dtick_L = dtick_L + 1;
   end
   odometry(:,k+1) = F_estimate_p(odometry(:,k),dtick_L,dtick_R,L,N,R); 
   
   %particles
   for i = 1:Np
       barp   = F_estimate_p(p(:,i),dtick_L,dtick_R,L,N,R); 
       p(:,i) = F_sample_odometry(p(:,i),barp,alpha);        
   end
   
   %animation
   F_anima([x(k) y(k) th(k)], L, fig, axxis)      
   if rem(k,100)==0
      Pplot = [Pplot p];           
   end      
end
%return
 F_anima([x(end) y(end) th(end)], L, fig, axxis)      

plot(x,y,'k','Color',[0.6 0.6 0.6],'LineWidth',2.5)
hold on
plot(odometry(1,:),odometry(2,:),'k--')
plot([odometry(1,1) odometry(1,1)+0.03*cos(odometry(3,1))],[odometry(2,1) odometry(2,1)+0.03*sin(odometry(3,1))],'b');
plot(odometry(1,end),odometry(2,end),'*','Color',[0.3 0.3 0.3],'LineWidth',3)
plot([odometry(1,end) odometry(1,end)+0.03*cos(odometry(3,end))],[odometry(2,end) odometry(2,end)+0.03*sin(odometry(3,end))],'Color',[0.3 0.3  0.3],'LineWidth',3)
axis(axxis)
grid on

for i = 1:size(Pplot,2)       
    plot(Pplot(1,i),Pplot(2,i),'k.')
    plot([Pplot(1,i) Pplot(1,i)+0.02*cos(Pplot(3,i))],[Pplot(2,i) Pplot(2,i)+0.02*sin(Pplot(3,i))],'k');
    %drawnow       
end 
grid on
return






