function [ ] = F_anima_map(pose,L,fig,map,sonars,angles)
% ----------------------------------------------------------------------
%
% F_anima(pose,L,fig,axxis) Receives the robot pose and plots it in the 
%                           enviroment
%
% Careful: this function clears the figure before ploting the robot (so we
% can run it as an animation.
%
%      pose  = [x y th]
%      L     = distance between wheels [m]
%      fig   = figure number
%      axxis = figure axis
%      map   = matrix with map (1=obstacle, 0=free)
%
% ----------------------------------------------------------------------


figure(fig);
clf
imshow(~map);
set(gca,'YDir','normal');
hold on

% car "nose"
plot([pose(1) pose(1)+L/2*cos(pose(3))],[pose(2) pose(2)+ L/2*sin(pose(3))],'k');

% car "axis"
plot([pose(1)+L/2*cos(pose(3)+pi/2) pose(1)+L/2*cos(pose(3)-pi/2)],[pose(2)+L/2*sin(pose(3)+pi/2) pose(2)+L/2*sin(pose(3)-pi/2)],'k');

%car body
ang = pose(3) + (-pi:pi/10:pi); 
xp  = pose(1) + L/2*cos(ang);
yp  = pose(2) + L/2*sin(ang);
plot(xp,yp,'k');

%sensors
if nargin > 4
   for i = 1:length(sonars) 
    plot([pose(1) pose(1)+sonars(i)*cos(pose(3)+angles(i))],[pose(2) pose(2)+sonars(i)*sin(pose(3)+angles(i))],'Color',[.6 .6 .6]);
   end
end

drawnow
%keyboard
pause(0.001)
end




