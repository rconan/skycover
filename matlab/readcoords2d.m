function [x, y, r] = readcoords2d(fpath)

    fmt = '%f';
    sizeA = [3 inf];
    fileID = fopen(fpath, 'r');
    
    A = fscanf(fileID, fmt, sizeA);
    
    fclose(fileID);
    
    x = A(1,:);
    y = A(2,:);
    r = A(3,:);
    
end