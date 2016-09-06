function fmt = coord_reader(n)
    fmt = {};
    for i=1:n
        fmt{end+1} = '(%f, %f)';
    end
    fmt = strjoin(fmt);
end