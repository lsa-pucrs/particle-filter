%% Particle Filter
for i = 1:Np
    %% Prediction
    % estimate movement according to encoders
    barp   = F_estimate_p(p(:,i),dtick_L,dtick_R,L,N,R);
    % throw in some variation in the RTR model
    p(:,i) = F_sample_odometry(p(:,i),barp,alpha);
    
    %% Update
    % compute particle probability
    if map(max(1,min(ceil(p(2,i)*mapscale),size(map,1))),max(1,min(ceil(p(1,i)*mapscale),size(map,2))))
        w(i) = 0;   % if on obstacle or outside map
    else % else compute the particle probability based on sonars
        % simulate the sonars for the particles
        z = Fast_ray_cast(p(1,i),p(2,i),p(3,i),map,max_range,angles,mapscale,1);%
        for j = 1:length(sonars)
            w(i) = w(i)*F_measurProb(z(j),sonars(j),CovSonars);
        end
    end 
end