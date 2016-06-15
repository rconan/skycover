function [x, y] = readcoords2d(fpath)

    fmt = '%f';
    sizeA = [2 inf];
    fileID = fopen(fpath, 'r');
    
    A = fscanf(fileID, fmt, sizeA);
    
    fclose(fileID);
    
    x = A(1,:);
    y = A(2,:);
    
end