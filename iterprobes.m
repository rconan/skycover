function iterprobes(filename)

    probes = read_polygons(filename);
    
    figure; hold on;
    ylim([-1, 1]);
    xlim([-1, 1]);
    axis equal;

    axes = [-0.951057, 0.309017, ...
            -0.587785, -0.809017, ...
            0.587785, -0.809017, ...
            0.951057, 0.309017];


    colors = ['r', 'g', 'b', 'y'];
    for i=1:size(colors, 2)
        plot([0, axes(i*2-1)], [0, axes(i*2)], colors(i));
    end
    
    nconfigurations = size(probes, 2) / 4;
    
    for i=1:nconfigurations
        offset = (i-1)*4;
        probe_handles = [];
        for j=1:4
            probe_handles(j) = fill(probes(offset+j).xs, probes(offset+j).ys, colors(j));
        end
        
        pause(0.1);
        
        if i < nconfigurations
            for j=1:4
                delete(probe_handles(j));
            end 
        end
    end
end
