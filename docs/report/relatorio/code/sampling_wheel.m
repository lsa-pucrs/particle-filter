% sampling wheel
index = round(rand(1,1)*(Np-1)+1);  % index starts randomly
beta = 0;
mw = max(w);
for i = 1:Np
    beta = beta+rand(1,1)*mw*2;
    while beta > w(index)
        beta = beta-w(index);
        index = index+1;
        if index > Np
            index = 1;
        end
    end
    p2(:,i) = p(:,index);
end
p = p2;
w(:) = 1;