function rotate_starfield_movie(starfile, theta)

    figure; hold on;
    
    [starsx, starsy] = readstars(starfile);
    
    paddingx = mean(abs(starsx));
    paddingy = mean(abs(starsy));
    
    xlim([min(starsx)-paddingx, max(starsx)+paddingx]);
    ylim([min(starsy)-paddingy, max(starsy)+paddingy]);
     
    for i=1:theta
        [newx, newy] = rotate2dcoord(starsx, starsy, i * (pi / 180));
        p = plot(newx, newy, 'b.');
        pause(0.1)
        delete(p);
    end

end