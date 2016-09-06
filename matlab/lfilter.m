function filtered = lfilter(f, A)
    filtered = [];
    for i=1:size(A,2)
        if f(A(i))
            filtered = [filtered A(i)];
        end
    end
end