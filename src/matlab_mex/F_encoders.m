function [tick_L tick_R] = F_encoders(dx,dy,dth,L,N,R )
% ----------------------------------------------------------------------
%
%F_encoders(dx,dy,dth,L,N,R ) Returns left and right ticks of planar robot 
%     wheels given the distance between the poses (dx, dy, dth). 
%
%    dx,dy,dth -> distance between poses (dx1-dx0, dy1-dy0, dth1-dth0)
%    L         -> distance between wheels
%    N         -> number of ticks per revolution (multiple of 720)
%    R         -> radius of wheels
%
% Works well for large numbers of ticks, hence, either N is large (must 
% also be a multiple of 720) or the sampling time T is large.
%
% ----------------------------------------------------------------------


disp('---------------------------------begin Function---------------------------------')


persistent angL angR
if isempty(angL)
    angL = 0;
    angR = 0;
end

disp('First angL and angR:');
angL;
angR;

disp('First tick_R and tick_L:')
tick_R = 0;
tick_L = 0;

%% calculate the distance traveled by each wheel

disp('calculate the distance traveled by each wheel')
D = sqrt( dx^2 + dy^2);
Dr = (D + L/2*dth);
Dl = (D - L/2*dth);
% converts that to degrees
angR_end = angR + round(Dr/R*2*180/pi);
angL_end = angL + round(Dl/R*2*180/pi);

%% right wheel
disp('right wheel')
if(angR_end - angR) > 0 %positive rotation
    disp('entro no if do left')
    for angles = angR+1:angR_end
        if rem(angles,2*360/N)==0
            disp('entro')
            tick_R = tick_R + 1;
        end
    end
elseif(angR_end - angR) < 0 %negative rotation
    disp('entro no else do left')
    for angles = angR_end+1:angR
        if rem(angles,2*360/N)==0
            tick_R = tick_R - 1;
        end
    end
end

%% left wheel
disp('left wheel')
if(angL_end - angL) > 0 %positive rotation
    disp('entro no if do left')
    for angles = angL+1:angL_end
        if rem(angles,2*360/N)==0
            tick_L = tick_L + 1;
        end
    end
elseif(angL_end - angL) < 0 %negative rotation
    disp('entro no if do left')
    for angles = angL_end+1:angL
        if rem(angles,2*360/N)==0
            tick_L = tick_L - 1;
        end
    end
end
    
angL = angL_end
angR = angR_end

disp('end Function')
end

