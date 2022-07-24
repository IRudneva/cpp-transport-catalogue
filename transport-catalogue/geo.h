#pragma once
#include <cmath>
const double EARTH_RADIUS = 6371000;

namespace geo {

    struct Coordinates {
        double lat; 
        double lng; 
        bool operator==(const Coordinates& other) const;
        bool operator!=(const Coordinates& other) const;
    };

    class CoordinatesHasher {
    public:
        std::size_t operator()(const Coordinates& coords) const;
    };

    double ComputeDistance(Coordinates from, Coordinates to);
}  // namespace geo