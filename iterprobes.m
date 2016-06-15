function iterprobes(filename)

    probes = read_polygons(filename);
    
    figure; hold on;
    ylim([-1, 1]);
    xlim([-1, 1]);
    axis equal;
    % draw_sensor_area(starsx, starsy);

    nconfigurations = size(probes, 2) / 4;
    
    for i=1:nconfigurations
        offset = (i-1)*4;
        probe_handles = [];
        for j=1:4
            probe_handles(j) = fill(probes(offset+5-j).xs, probes(offset+5-j).ys, 'r');
        end
        
        % pause(10);
        
        if i < nconfigurations
            for j=1:4
                delete(probe_handles(j));
            end 
        end
    end
end
