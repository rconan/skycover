function iterprobes(filename, varargin)

    dims = [12 4 4];
    probes = read_polygons(filename, dims);
    
    figure; hold on;
    ylim([-1500, 1500]);
    xlim([-1500, 1500]);
    axis equal;

    axes = [-0.951057, 0.309017, ...
            -0.587785, -0.809017, ...
            0.587785, -0.809017, ...
            0.951057, 0.309017];
        
    for i=1:4
        plot([0, 3600*axes(i*2-1)], [0, 3600*axes(i*2)], 'r');
    end

    darkgrey    = [0.662745, 0.662745, 0.662745];
    dimgray     = [0.411765, 0.411765, 0.411765];
    sgidarkgray = [0.333333, 0.333333, 0.333333];
        
    colors = [sgidarkgray; darkgrey; dimgray];

    npolygons = size(dims, 2) * 4;
    nconfigurations = size(probes, 2) / npolygons;
    probe_handles = double.empty(1, npolygons, 0);
    
    for i=1:nconfigurations
        offset = (i-1)*npolygons;
        probe_handles = [];
        for j=1:npolygons
            xcoords = lfilter(@(x) ~isnan(x), probes(offset+j).xs);
            ycoords = lfilter(@(x) ~isnan(x), probes(offset+j).ys);
            probe_handles(j) = fill(xcoords, ycoords, colors(mod(j, size(colors,2))+1, :));
        end
            
    if nargin > 1
        [x, y] = readstars(varargin{1});
        plot(x, y, 'b.', 'markersize', 12);
    end
        
        pause(2);
        
        if i < nconfigurations
            for j=1:npolygons
                delete(probe_handles(j));
            end 
        end
    end
end
