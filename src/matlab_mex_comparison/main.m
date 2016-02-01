%% Function that returns average filter time, run in MATLAB
function time = main(Np)
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
time_vec(1:1:ttotal/T) = 0;

%% initial conditions
x  = .2;
y  = .5;
th = 0;
odometry = [x y th]';

vr = 1;
vl = 1;

%% Particle Filter parameters
alpha     = [0 0 50 0]*1;
CovSonars = 2.5;
global p
%Np = 50;
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
    sonars = Fast_ray_cast(x(k+1),y(k+1),th(k+1),map,max_range,angles,mapscale,1);%
    sonars = sonars + abs(rand(5,1)-0.5)*covsonars;
    see_sonar(:,k) = sonars;
    
    %% encoders
    [dtick_L dtick_R] = F_encoders( x(k+1)-x(k),y(k+1)-y(k),th(k+1)-th(k),L,N,R);
    % this if perturbs the encoder measurements
    if rem(k,50)==0
        dtick_L = dtick_L + 1;
    end
    odometry(:,k+1) = F_estimate_p(odometry(:,k),dtick_L,dtick_R,L,N,R);
    hold on
    
    %% particle filter
    tic;
    for i = 1:Np
        % Estimate movement according to encoders
        barp   = F_estimate_p(p(:,i),dtick_L,dtick_R,L,N,R);
        
        % Throw in some variation in the RTR model
        p(:,i) = F_sample_odometry(p(:,i),barp,alpha);
        
        % Particle probability
        if map(max(1,min(ceil(p(2,i)*mapscale),size(map,1))),max(1,min(ceil(p(1,i)*mapscale),size(map,2))))
            w(i) = 0;   % if on obstacle or outside map
        else
            % Simulate the sonars for the particles
            z = Fast_ray_cast(p(1,i),p(2,i),p(3,i),map,max_range,angles,mapscale,1);%
            for j = 1:length(sonars)
                % Calculate the probability for the particle
                w(i) = w(i)*F_measurProb(z(j),sonars(j),CovSonars);
            end
        end
    end
    time_vec(k) = toc;
    w = w/sum(w);    % Normalize the probabilities
    
    % sampling wheel
    if rem(k,15)==0 && k>10 % choose "-1" for open loop simulation of the Part. Filt
        beta = 0;
        index = round(rand(1,1)*(Np-1)+1);      % index starts randomly
        mw = max(w);
        for i = 1:Np
            beta = beta+rand(1,1)*mw*2;
            while beta > w(index)
                beta = beta-w(index);
                index = index+1;
                if index > Np
                    index = 1;
                end
            end
            p2(:,i) = p(:,index);
        end
        p = p2;
        w(:) = 1;
    end
    P(:,k) = [sum(p(1,:)) sum(p(2,:)) sum(p(3,:))]'/Np;
    
    if k>120
        CovSonars=1;
    end
    
    %% animation
    if 0 %to anime or not to anime
        F_anima_map([x(k+1)*mapscale y(k+1)*mapscale th(k+1)], L*mapscale, fig, map,sonars*mapscale,angles)
        for i = 1:size(p,2)
            plot(p(1,i)*mapscale,p(2,i)*mapscale,'k.')
        end
        pause(eps)
    end
    
    %% results
    if rem(k,108)==0
        plot_sonar = [plot_sonar sonars];
        Pplot = [Pplot p];
        plot_time = [plot_time k]
    end
end
time = sum(time_vec)/k; % Returns average time for filter loop
return
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
end