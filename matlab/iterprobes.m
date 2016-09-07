function iterprobes(filename, varargin)

    dims = [4 4 4 4];
    probes = read_polygons(filename, dims);
    
    figure; hold on;
    ylim([-1500, 1500]);
    xlim([-1500, 1500]);
    axis equal;

    axes = [ 0,  1400, ...
            -1400,  0, ...
             0, -1400, ...
             1400,  0];
        
    circle(0, 0, 0.1 * 3600, 'k');
    circle(0, 0, -.167 * 3600, 'k');
        
    for i=1:4
        plot([0, axes(i*2-1)], [0, axes(i*2)], 'k');
    end

    darkgrey    = [0.662745, 0.662745, 0.662745];
    dimgray     = [0.411765, 0.411765, 0.411765];
    sgidarkgray = [0.333333, 0.333333, 0.333333];
        
    colors = [sgidarkgray; dimgray; darkgrey];

    npolygons = 12;
    nconfigurations = size(probes, 2) / npolygons;
    probe_handles = double.empty(1, npolygons, 0);
    
    for i=1:4
        [x1, y1] = rotate2dcoord(-axes(2*i-1), -axes(2*i), 30 * (pi / 180));
        % plot([axes(2*i-1), axes(2*i-1)+x], [axes(2*i), axes(2*i)+y], 'r');
        
        [x2, y2] = rotate2dcoord(-axes(2*i-1), -axes(2*i), -30 * (pi / 180));
        % plot([axes(2*i-1), axes(2*i-1)+x], [axes(2*i), axes(2*i)+y], 'r');
        
        % h = fill([axes(2*i-1), axes(2*i-1)+x1, axes(2*i-1)+x2], ...
        %          [axes(2*i),   axes(2*i)+y1,   axes(2*i)+y2], 'r');
             
        % set(h, 'facealpha', 0.5);
    end

    
    for i=1:nconfigurations
        starfile = sprintf('../starfiles/starfield%d.cat', i);
        [starsx, starsy] = readstars(starfile);
        
        
        offset = (i-1)*npolygons;
        probe_handles = [];
        for j=1:npolygons
            xcoords = lfilter(@(x) ~isnan(x), probes(offset+j).xs);
            ycoords = lfilter(@(x) ~isnan(x), probes(offset+j).ys);
            probe_handles(j) = fill(xcoords, ycoords, colors(mod(j, size(colors,1))+1, :));
        end
        
        starplot = plot(starsx, starsy, 'b.', 'markersize', 12);
            
    if nargin > 1
        [x, y] = readstars(varargin{1});
        plot(x, y, 'b.', 'markersize', 12);
    end
    
    pause(1.5);
        
        if i < nconfigurations
            for j=1:npolygons
                delete(probe_handles(j));
            end
            delete(starplot);
        end
    end
end
