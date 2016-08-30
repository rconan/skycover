function plotphasing(filename)
    [x, y] = readcoords2d(filename);
    std_axes(13, 19, 0, 1);
    plot(x, y, 'b-')
    title('Phasing w/ M3 Obscuration', 'FontSize', 14);
    xlabel('Magnitude', 'FontSize', 16);
    ylabel('P(Valid Configuration)', 'FontSize', 16);
    
    for i=1:size(x,2)
        txt = sprintf('(%g, %g)', x(i), y(i));
        text(x(i)-0.1, y(i)-0.1, txt, 'FontSize', 14, 'Rotation', 30);
    end
end