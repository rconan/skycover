function iterprobes_movie(M, filename)
    v = VideoWriter(filename);
    open(v);
    writeVideo(v, M);
    close(v);
end