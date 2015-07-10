function [range]=F_ray_casting(xc,yc,theta,map,max_range,angles,mapscale,accuracy)

% Simulates the range readings of a robot with pose (x,y,t) on a given map
% Input parameters:  (x,y,theta,map,max_range,angles,mapscale,accuracy)
%   (x,y,theta): robot pose, 
%           map: binary matrix containning the map 1=occupied,0=free,
%     map_range: maximum range of sensors, 
%        angles: angles of each sensor with respect to the robot [rad]
%      mapscale: each grid in 'map' represents 1/mapscale meters 
%      accuracy: number of points to be tested in max_range
% Returns one range per angle.

switch nargin
  case 4,
    max_range = 1;
    angles = pi/180*[0 45 90 -45 -90 180];
    mapscale = 100;
    accuracy = max_range*mapscale;
  case 5,    
    angles = pi/180*[0 45 90 -45 -90 180];
    mapscale = 100;
    accuracy = max_range*mapscale;
  case 6,
    mapscale = 100;
    accuracy = max_range*mapscale;
  case 7,  
    accuracy = max_range*mapscale;
end

xc = xc*mapscale;
yc = yc*mapscale;
max_range = max_range*mapscale;

n = length(angles);
range = zeros(n,1);
% If robot outside map
if yc > size(map,1)||xc > size(map,2) || xc <= 0 || yc <= 0
    %disp('Outside map')
    return;
end
% If the robot is on top of an occupied area
if map(ceil(yc),ceil(xc))==1
    %disp('On obstacle')
    return;
end

r=linspace(0,max_range,accuracy); 
for i=1:n    
    % creates the ray
    x=xc+(r*cos(theta+angles(i)));
    y=yc+(r*sin(theta+angles(i)));
    
    % Removing points that are out of the map
    temp=[];
    for k=1:numel(x)
       if x(k)>size(map,2) || y(k)>size(map,1) || x(k)<=0 || y(k)<=0
           temp=[temp;k];
       end
    end
    x(temp)=[];
    y(temp)=[];
    
    % Rounding up
    xint=ceil(x);
    yint=ceil(y);
    
    % Finds the intersection
    b=[];
    for j=1:numel(xint)
        b=[b;map(yint(j),xint(j))];
    end
    ind=find(b==1);    

    % if intersection
    if ~isempty(ind)
        xb=x(ind(1));
        yb=y(ind(1));        
        dist=sqrt((xc-xb).^2 + (yc-yb).^2);
        range(i)=dist;  
    else% if no intersection
        range(i)=max_range;  
    end
end

range = range/mapscale;
