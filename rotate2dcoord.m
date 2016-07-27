function [newx, newy] = rotate2dcoord(x, y, theta)

    newx = x.*cos(theta) - y.*sin(theta);
    newy = x.*sin(theta) + y.*cos(theta);

end