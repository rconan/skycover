function [x, y] = readcoords2d(fpath)

    fmt = '%f';
    sizeA = [2 inf];
    fileID = fopen(fpath, 'r');
    fprintf('Trying to open %s',fpath);
    fprintf('Got fileID %d ', fileID);
    A = fscanf(fileID, fmt, sizeA);
    
    fclose(fileID);
    
    x = A(1,:);
    y = A(2,:);
    
end