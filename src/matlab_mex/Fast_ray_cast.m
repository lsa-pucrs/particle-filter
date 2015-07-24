function [range]=Fast_ray_cast(xc,yc,theta,map,max_range,angles,mapscale,accuracy)

% Simulates the range readings of a robot with pose (x,y,t) on a given map
% Input parameters:  (x,y,theta,map,max_range,angles,mapscale,accuracy)
%   (x,y,theta): robot pose, 
%           map: binary matrix containning the map 1=occupied,0=free,
%     map_range: maximum range of sensors, 
%        angles: angles of each sensor with respect to the robot [rad]
%      mapscale: each grid in 'map' represents 1/mapscale meters 
%      accuracy: number of points to be tested in max_range
% Returns one range per angle.

disp('-------------------------------entro na funcao ----------------------------')
xc = xc*mapscale;
yc = yc*mapscale;
max_range = max_range*mapscale;

n = length(angles);
% If robot outside map
if yc > size(map,1)||xc > size(map,2) || xc <= 0 || yc <= 0
    disp('Outside map')
    range = zeros(n,1);
    return;
end
% If the robot is on top of an occupied area
if map(ceil(yc),ceil(xc))==1
    disp('On obstacle')
    range = zeros(n,1);
    return;
end

range = max_range*ones(n,1)
map = map';

for i=1:n    
    
    %computes direction
    dx = accuracy*cos(theta+angles(i));
    dy = accuracy*sin(theta+angles(i));
    % initialize the ray
    x=xc;
    y=yc;     
    dist = max_range;
        
    while dist <= max_range
        x = x + dx;
        y = y + dy;
        dist = sqrt((x-xc)^2 + (y-yc)^2);
        if ceil(x)>size(map,1) || ceil(y)>size(map,2) || ceil(x)<1 || ceil(y)<1
            %disp('outside')
            break;
        end
        if map(ceil(x),ceil(y))==1
            %disp('hit')
            disp('entro')
            i
            range(i) = dist
            break;
        end
    end
%    keyboard
end
range = range/mapscale;