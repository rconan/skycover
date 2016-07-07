function polys = read_polygons(filename)
    fid = fopen(filename, 'r');
    data = cell2mat(textscan(fid,'(%f, %f) (%f, %f) (%f, %f) (%f, %f)','collectoutput',1));
   
    polys = Polygon(1, size(data,1));
    for i=1:size(data,1)
        xs = data(i, 1:2:end);
        ys = data(i, 2:2:end);
        polys(i) = Polygon(xs, ys);
    end
end
