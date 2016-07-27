function iterprobes_tracking(filename)

    probes = read_polygons(filename);
    
    figure; hold on;
    ylim([-1, 1]);
    xlim([-1, 1]);
    axis equal;
    plot(0, 0);
    grid on;
    
    
    axes = [-0.951057, 0.309017, ...
            -0.587785, -0.809017, ...
            0.587785, -0.809017, ...
            0.951057, 0.309017];


    colors = ['r', 'g', 'b', 'y'];
    % for i=1:size(colors, 2)
    %     plot([0, axes(i*2-1)], [0, axes(i*2)], colors(i));
    % end
    
    nconfigurations = 60;
    num_valid_configs = size(probes, 2) / (nconfigurations * 4);
    
    probe_handles = zeros(1, 4);
    
    for i=1:num_valid_configs
        starfile = sprintf('starfiles/starfield%d.cat', i);
        [starsx, starsy] = readstars(starfile);
        
        leg = legend(starfile);
        set(leg, 'FontSize', 16);
        
        
        for j=1:nconfigurations
            
            offset = ((i-1) * nconfigurations * 4) + (j-1)*4;
            
            probe_handles = [];
            for k=1:4
                probe_handles(k) = fill(probes(offset+k).xs, probes(offset+k).ys, colors(1));
            end
            
            [rotatedx, rotatedy] = rotate2dcoord(starsx, starsy, j * (pi / 180));
            starplot = plot(rotatedx, rotatedy, 'b.', 'markersize', 12);
        
            pause(0.08);
        
            if j < nconfigurations
                for k=1:4
                    delete(probe_handles(k));
                end 
                delete(starplot);
            else
                pause(1.5);
                for k=1:4
                    delete(probe_handles(k));
                end 
                delete(starplot);
            end
        end
        
        % pause(2);
    end
end
