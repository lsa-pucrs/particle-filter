function [ HUGE ] = Compute_sonars(map,max_range,angles,mapscale,theta_accuracy);%
%COMPUTE_SONARS Summary of this function goes here
%   Detailed explanation goes here

tic
[n m] = size(map)
l = length(angles)
HUGE = zeros(m,n,360/theta_accuracy,l);
for j=1:m % x position
    tic
    for k = 1:n % y position
        if map(k,j) == 1
            for i = 1:round(360/theta_accuracy) % theta position
                HUGE(k,j,i,:)=zeros(1,l);
            end
        else
            for i = 1:round(360/theta_accuracy) % theta position                
                HUGE(k,j,i,:)=Fast_ray_cast(j,k,i,map,max_range,angles,1,1);
            end
        end
    end
    toc
    j
end
toc



end

