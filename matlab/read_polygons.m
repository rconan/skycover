function polys = read_polygons(filename, dimensions)
    fid = fopen(filename, 'r');
    ndims = size(dimensions, 2);
    
    formatters = arrayfun(@(i) coord_reader(dimensions(i)), 1:ndims, 'UniformOutput', false);
    formatter = coord_reader(max(dimensions));

    data = [];
    nlines = 1;
    while (~feof(fid))
        data = [data; cell2mat(textscan(fid, formatter, 1, 'collectoutput', 1))];
        nlines = nlines + 1;
    end
    
    polys = Polygon(1, size(data,1));
    for i=1:size(data,1)
        xs = data(i, 1:2:end);
        ys = data(i, 2:2:end);
        polys(i) = Polygon(xs, ys);
    end
end