#ifndef LATLNG_H
#define LATLNG_H

#include <string>

using namespace std;

/*
 * A latitude,longitude coordinate with precision to 7 decimals.
 * Which means about 1cm accuracy.
 * We store the latitude and longitude as integer.
 * We multiply or divide by 10.000.000 to switch between the actual and the compressed value.
 */
class LatLng
{
public:
    LatLng();                                                         // Create the coordinate
    LatLng(const double &lat, const double &lng);                     // Create the coordinate
    ~LatLng();                                                        // Destroy the coordinate

    double                      getLat() const;                       // Actual Latitude value as double
    double                      getLng() const;                       // Actual Longitude value as double
    static pair<double, LatLng> ProjectOnSegment(const double &lat,
                                                 const double &lng,   // Project point to road segment.
                                                 const LatLng &A,     // Returns the clamp ratio
                                                 const LatLng &B);    // and the projected point
    static double               DistanceInMeters(const double &lat1,
                                                 const double &lng1,
                                                 const double &lat2,
                                                 const double &lng2); // Distance in meters between 2 coordinates
    static bool                 Validate(const double &lat,
                                         const double &lng);          // Returns true if it is a GPS coordinate
    bool                      operator==(const LatLng &other);        // Check if this coordinate == other
    string toString() const;

    int lat_int; // Compressed Latitude as integer
    int lng_int; // Compressed Longitude as integer

private:
    static const int        COORDINATE_TO_INT      = 10000000;
    static constexpr double INT_TO_COORDINATE      = 1.0 / 10000000;
    static constexpr double DEG_TO_RAD             = 3.141592653589793 / 180.0;
    static const int        EARTH_RADIUS_IN_METERS = 6378137;
};

#endif // LATLNG_H