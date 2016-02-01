clear all; clc;
% 
% ttotal = 270;
% T  = .5;     % sampling time
% 
% for k = 1:ttotal/T
%           
%     if rem(k,60)==0
%         disp('entro-------------------------------------')
%         eval(['p_' num2str(k) ' = rand(3,50)'])
%         p = rand(3,50);
%         load(sprintf('k_%02d.mat', k), 'p', 'sonars')
%         p
%         sonars
%     end     
% end
% 
% load('datafile_539.mat', 'p', 'sonars')

load('set_value.mat', 'mapscale', 'L', 'map','angles','p')
%load('first_p.mat','p')
ttotal = 270;
T  = .5;     % sampling time

%% simulation
Pplot = [p]
plot_time = []
plot_sonar= []

load('dynamic_param.mat', 'sonars')
load('final.mat',  'x', 'y', 'th', 'odometry', 'see_sonar')
for k = 1:ttotal/T    
    if rem(k,60)==0
      load('t_file.mat', sprintf('p%02d', k));
%       load(sprintf('k_%02d.mat', k), 'p')
      plot_sonar = [plot_sonar see_sonar(:,k)];
      Pplot = [Pplot eval(sprintf('p%02d', k))];
      plot_time = [plot_time k];
    end     
end
 
Pplot = [Pplot p];     

%% results dynamic
d = 0.05;
fig = 1;
F_anima_map([x(length(x))*mapscale y(length(y))*mapscale th(length(th))], L*mapscale, fig, map,0*see_sonar(:,1)*mapscale,angles)
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