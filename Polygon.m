classdef Polygon
    properties
        xs
        ys
    end
    methods
        function polygons = Polygon(varargin)
            if nargin == 2
                polygons.xs = cell2mat(varargin(1));
                polygons.ys = cell2mat(varargin(2));
            elseif nargin > 0
                polygons = repmat(polygons,varargin{:});
            end
        end
    end
end
