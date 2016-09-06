function draw_probes(filename)
    data = read_polygons(filename);
    
    figure; hold on;
    for i=1:size(data,2)
        fill(data(i).xs, data(i).ys, 'r');
    end
end