function plotstars(starfile)
    
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
    
    [x, y] = readstars(starfile);
    plot(x, y, 'b.');
end