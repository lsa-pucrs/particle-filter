function [range]=ray_casting(xc,yc,theta,map,max_range,angles,Plot,imscale)

% Simulates the range readings of a robot with pose (x,y,t) on a given map
% Input parameters:  (x,y,theta,map,range,angles) - 
%   (x,y,theta): robot pose, 
%           map: binary matrix containning the map 1=occupied,0=free,
%         range: maximum range of sensors, 
%        angles: angles of each sensor with respect to the robot [rad]
%          Plot: Binary, 1 plots results, 0 does nothing
% Returns one range per angle.


n = length(angles);

switch nargin
  case 4,
    max_range = 100;
    angles = pi/180*[0 45 90 -45 -90 180];
    Plot = 0;
    imscale = 1;
  case 5,    
    angles = pi/180*[0 45 90 -45 -90 180];
    Plot = 0;
    imscale = 1;
  case 6,
    Plot = 0;
    imscale = 1;
  case 7,
    imscale = 1;
end

range=zeros(n,1);
% If robot outside map
if yc > size(map,1)||xc > size(map,2) || xc <= 0 || yc <= 0
    return;
end
% If the robot is on top of an occupied area
if map(ceil(yc),ceil(xc))==1
    return;
end

if Plot
    figure(1);
    imshow(imresize(map,imscale));    
    hold on;
    plot(xc*imscale,yc*imscale,'go','LineWidth',2);
    plot([xc*imscale xc*imscale+10*cos(theta)],[yc*imscale yc*imscale+10*sin(theta)],'g','LineWidth',2);
end


for i=1:n
    r=linspace(0,max_range,10);
    x=xc+(r*cos(theta+angles(i)));
    y=yc+(r*sin(theta+angles(i)));
    % Removing points out of map
    temp=[];
    for k=1:numel(x)
        if x(k)>size(map,2) || y(k)>size(map,1) || x(k)<=0 || y(k)<=0
            temp=[temp;k];
        end
    end
    x(temp)=[];
    y(temp)=[];
    % Computing Intersections
    xint=ceil(x);
    yint=ceil(y);
    

    b=[];
    for j=1:numel(xint)
        b=[b;map(yint(j),xint(j))];
    end
    ind=find(b==1);
    
    if Plot       
        plot(x*imscale,y*imscale,'r');
    end
        
    if ~isempty(ind)
        xb=x(ind(1));
        yb=y(ind(1));
        if Plot 
            plot(xb*imscale,yb*imscale,'g*');
        end
        
        dist=sqrt((xc-xb).^2 + (yc-yb).^2);
        range(i)=dist;  
    else
        range(i)=max_range;  
    end
%pause(0.000001);
end
% Converting to mm from pixels.
%range=range*10*pixeltocm;
% % % % % % % range=flipud(range);