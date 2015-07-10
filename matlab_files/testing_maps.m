% test the car on a map 
clear all;clc;
problem = [0 0];
%% parameters
ttotal = 270;
T  = .5;     % sampling time
R  = 0.02;   % wheel radius 2cm
L  = 0.08;   % distance between wheels 8cm
N  = 48;     % encoder ticks per rotation (must be a multiple of 720)

%% initial conditions
x0  = .2;       
y0  = .5;
th0 = 0;

vr = 1;
vl = 1;

x=x0; y=y0; th=th0;
odometry = [x y th]';

%% interface
scale = 20;
scale2= 12*scale;
map = F_load_map(scale);

%% Particle Filter parameters
alpha=[5 10 20 5];
alpha=[5 10 20 5];
global p
Np = 50;
p=zeros(3,Np);

for i=1:length(p)
    p(:,i) = [x0 y0 th0]';
end

P = [sum(p(1,:)) sum(p(2,:)) sum(p(3,:))]'/Np;
Pplot = [0 0 0]';

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
     if k == 488 vl=1.001;end

%% actual car   
   [t,cont] = ode23(@F_dif_drive_car,[0 T],[x(k) y(k) th(k)],options,vr,vl,R,L);
   x(k+1) = cont(end,1); y(k+1) = cont(end,2); th(k+1) = cont(end,3);
   
%% encoders
   [dtick_L dtick_R] = F_encoders( x(k+1)-x(k),y(k+1)-y(k),th(k+1)-th(k),L,N,R);
   % this if perturbs the encoder measurements
   if rem(k,55)==0
       dtick_L = dtick_L + 1;
   end
   odometry(:,k+1) = F_estimate_p(odometry(:,k),dtick_L,dtick_R,L,N,R); 
   hold on

%% animation
   %F_anima_map([x(k)*scale2 y(k)*scale2 th(k)], L*scale2, fig, map)  
   if rem(k,108)==0
      Pplot = [Pplot p];           
   end  
   
%% particles
   for i = 1:Np
       barp= F_estimate_p(p(:,i),dtick_L,dtick_R,L,N,R);        
       bp  = F_sample_odometry(p(:,i),barp,alpha);  
       bp(1) = max(bp(1),0.001);
       bp(2) = max(bp(2),0.001);
       if map(min(ceil(bp(2)*scale2),size(map,2)),min(ceil(bp(1)*scale2),size(map,1)))   
           bp(1) = (bp(1) + P(1))/2;
           bp(2) = (bp(2) + P(2))/2;
           %problem = [problem; bp(1) bp(2)];
       end
       p(:,i)=bp(:);
   end
   P = [sum(p(1,:)) sum(p(2,:)) sum(p(3,:))]'/Np;
   
    
end

%% results
F_anima_map([x(end)*scale2 y(end)*scale2 th(end)], L*scale2, fig, map)  

plot(x(1:end-5)*scale2,y(1:end-5)*scale2,'k','Color',[0.6 0.6 0.6],'LineWidth',2.5)
hold on
plot(odometry(1,:)*scale2,odometry(2,:)*scale2,'k--')
plot([odometry(1,1) odometry(1,1)+0.03*cos(odometry(3,1))]*scale2,[odometry(2,1) odometry(2,1)+0.03*sin(odometry(3,1))]*scale2,'b');
plot(odometry(1,end)*scale2,odometry(2,end)*scale2,'*','Color',[0.3 0.3 0.3],'LineWidth',3)
plot([odometry(1,end) odometry(1,end)+0.03*cos(odometry(3,end))]*scale2,[odometry(2,end) odometry(2,end)+0.03*sin(odometry(3,end))]*scale2,'Color',[0.3 0.3  0.3],'LineWidth',3)
grid on

for i = 1:size(Pplot,2)       
    plot(Pplot(1,i)*scale2,Pplot(2,i)*scale2,'r.')
    plot([Pplot(1,i) Pplot(1,i)+0.02*cos(Pplot(3,i))]*scale2,[Pplot(2,i) Pplot(2,i)+0.02*sin(Pplot(3,i))]*scale2,'k');
    %drawnow       
end 
grid on
plot(problem(:,1)*scale2,problem(:,2)*scale2,'*b')
return






