function draw_sensor_area(x, y)

    plot(x, y, '.');
    ylim([-.5, .5]);
    xlim([-.5, .5]);
    axis equal;
    
    rectangle('Position', [.05 -.2 .4 .4], 'Curvature', [1 1]);
    rectangle('Position', [-.45 -.2 .4 .4], 'Curvature', [1 1]);
    rectangle('Position', [-.2 .05 .4 .4], 'Curvature', [1 1]);
    rectangle('Position', [-.2 -.45 .4 .4], 'Curvature', [1 1]);
    
end
