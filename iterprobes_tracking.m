function iterprobes_tracking(filename, wsize)

    probes = read_polygons(filename, [12 4 4]);
    
    figure; hold on;
    ylim([-wsize, wsize]);
    xlim([-wsize, wsize]);
    axis equal;
    plot(0, 0);
    grid on;
    
    axes = [ 0,  1, ...
            -1,  0, ...
             0, -1, ...
             1,  0];
         
    circle(0, 0, 0.1 * 3600);
    circle(0, 0, -.167 * 3600);
        
    for i=1:4
        plot([0, 3600*axes(i*2-1)], [0, 3600*axes(i*2)], 'r');
    end

    darkgrey    = [0.662745, 0.662745, 0.662745];
    dimgray     = [0.411765, 0.411765, 0.411765];
    sgidarkgray = [0.333333, 0.333333, 0.333333];
        
    colors = [sgidarkgray; darkgrey; dimgray];
    
    npolygons = 12;
    nconfigurations = 60;
    probe_handles = double.empty(1, npolygons, 0);
    num_valid_configs = size(probes, 2) / (nconfigurations * npolygons);
    
    for i=1:num_valid_configs
        starfile = sprintf('starfiles/starfield%d.cat', i);
        [starsx, starsy] = readstars(starfile);
        
        % leg = legend(starfile);
        % set(leg, 'FontSize', 16);
        
        for j=1:nconfigurations
            
            offset = ((i-1) * nconfigurations * npolygons) + (j-1)*npolygons;
            
            probe_handles = [];
            for k=1:npolygons
                xcoords = lfilter(@(x) ~isnan(x), probes(offset+k).xs);
                ycoords = lfilter(@(x) ~isnan(x), probes(offset+k).ys);
                probe_handles(k) = fill(xcoords, ycoords, colors(mod(k, size(colors,2))+1, :));
            end
            
            [rotatedx, rotatedy] = rotate2dcoord(starsx, starsy, (j-1) * (pi / 180));
            starplot = plot(rotatedx, rotatedy, 'b.', 'markersize', 12);
        
            pause(0.08);
        
            if j < nconfigurations
                for k=1:npolygons
                    delete(probe_handles(k));
                end 
                delete(starplot);
            elseif i < num_valid_configs
                pause(1.5);
                for k=1:npolygons
                    delete(probe_handles(k));
                end 
                delete(starplot);
            end
        end
        
        pause(1);
    end
end
