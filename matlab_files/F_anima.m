function [ ] = F_anima(pose,L,fig,axxis)
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
%
% Must add a map in this function as well
% ----------------------------------------------------------------------

figure(fig);
clf
% car "nose"
line([pose(1) pose(1)+L/2*cos(pose(3))],[pose(2) pose(2)+ L/2*sin(pose(3))]);
hold on
axis equal
axis(axxis);
% car "axis"
plot([pose(1)+L/2*cos(pose(3)+pi/2) pose(1)+L/2*cos(pose(3)-pi/2)],[pose(2)+L/2*sin(pose(3)+pi/2) pose(2)+L/2*sin(pose(3)-pi/2)],'k');
%car body
ang=pose(3)+ (-pi:pi/20:pi); 
xp=pose(1)+L/2*cos(ang);
yp=pose(2)+L/2*sin(ang);
plot(xp,yp,'k');
% car "wheels"
%plot(pose(1)+L/2*cos(pose(3)+pi/2),pose(2)+L/2*sin(pose(3)+pi/2),'ro','LineWidth',1);
%plot(pose(1)+L/2*cos(pose(3)-pi/2),pose(2)+L/2*sin(pose(3)-pi/2),'ro','LineWidth',1);
drawnow
%pause(0.001)


end

