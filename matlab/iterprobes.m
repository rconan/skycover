function iterprobes(starfield_dir, filename)

    dims = [4 4 4 4];
    probes = read_polygons(filename, dims);
    size(probes, 2)
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

    npolygons = 13;
    nconfigurations = size(probes, 2) / npolygons
    probe_handles = double.empty(1, npolygons, 0);
    
    for i=1:nconfigurations
        starfile = sprintf('%s/starfield%d.cat', starfield_dir, i);
        [starsx, starsy, starsr] = readstars(starfile);
        
        [stars_theta,stars_rho] = cart2pol(starsx,starsy);
        idx = (stars_rho<=600) & (stars_rho>=360);
        starsx(~idx) = [];
        starsy(~idx) = [];
        starsr(~idx) = [];
        stars_rho(~idx) = [];
        stars_theta(~idx) = [];

        offset = (i-1)*npolygons;
        probe_handles = zeros(1,npolygons);
        for j=1:npolygons-1
            xcoords = lfilter(@(x) ~isnan(x), probes(offset+j).xs);
            ycoords = lfilter(@(x) ~isnan(x), probes(offset+j).ys);
            probe_handles(j) = fill(xcoords, ycoords, colors(mod(j, size(colors,1))+1, :));
        end
        j=npolygons;
        xcoords = lfilter(@(x) ~isnan(x), probes(offset+j).xs);
        ycoords = lfilter(@(x) ~isnan(x), probes(offset+j).ys);
        probe_handles(j) = fill(xcoords, ycoords, 'r','FaceAlpha',0.5);
        
        in = inpolygon(starsx,starsy,xcoords,ycoords);
        starsx(in) = [];
        starsy(in) = [];
        starsr(in) = [];
        stars_rho(in) = [];
        stars_theta(in) = [];
        
        %starplot = plot(starsx, starsy, 'b.', 'markersize', 12);
        starplot = scatter(starsx, starsy,[],starsr,'filled');
        
        %{
        stars_theta = stars_theta + pi;
        asterism_zenith = zeros(4,1);
        asterism_azimuth = zeros(4,1);
        asterism_magnitude = zeros(4,1);
        for k=1:4
            idx = find((stars_theta>=(k-1)*pi/2) & (stars_theta<=k*pi/2));
            quadrant_starsr = starsr(idx);
            quadrant_brightest_star_idx = find(quadrant_starsr==min(quadrant_starsr));
            qidx = idx(quadrant_brightest_star_idx(1));
            hl(k) = line(starsx(qidx), starsy(qidx),...
                'Marker','o','MarkerSize',10,...
                'MarkerEdgeColor','r','LineStyle','none');
            asterism_zenith(k) = stars_rho(qidx)/60;
            asterism_azimuth(k) = stars_theta(qidx);
            asterism_magnitude(k) = starsr(qidx);
        end
        [asterism_magnitude,idx] = sort(asterism_magnitude);
        asterism_zenith = asterism_zenith(idx);
        asterism_azimuth = asterism_azimuth(idx);
        
        data = [asterism_zenith,asterism_azimuth,asterism_magnitude];
        dlmwrite('asterisms',data,'-append')
        %}
        colorbar()
        axis equal tight
        grid
    
    pause(1);
        
        if i < nconfigurations
            for j=1:npolygons
                delete(probe_handles(j));
                %delete(hl)
            end
            delete(starplot);
        end
    end
end
