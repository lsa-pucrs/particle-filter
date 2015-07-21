function [ p ] = fdraw_p(Np,varx,vary,vart,map,x,y,th,mapscale)
%DRAW_P Summary of this function goes here
%   Detailed explanation goes her

for k = 1:Np %Number of particles
   p(:,k) = [x + (rand(1,1)-.5)*varx;
             y + (rand(1,1)-.5)*vary;
             th+ (rand(1,1)-.5)*vart];

   while map(max(1,min(ceil(p(2,k)*mapscale),size(map,1))),max(1,min(ceil(p(1,k)*mapscale),size(map,2))))   
       p(:,k) = [x + (rand(1,1)-.5)*varx;
                 y + (rand(1,1)-.5)*vary;
                 th+ (rand(1,1)-.5)*vart];   
   end
end

end

