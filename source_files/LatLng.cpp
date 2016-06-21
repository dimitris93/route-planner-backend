#include "LatLng.h"
#include "Util.h"
#include <limits>
#include <iostream>
#include <cmath>

LatLng::LatLng() :
    lat_int(0),
    lng_int(0)
{

}

LatLng::LatLng(const double &lat, const double &lng) :
    lat_int(round(lat * COORDINATE_TO_INT)),
    lng_int(round(lng * COORDINATE_TO_INT))
{

}

LatLng::~LatLng()
{

}

double LatLng::getLat() const
{
    return lat_int * INT_TO_COORDINATE;
}

double LatLng::getLng() const
{
    return lng_int * INT_TO_COORDINATE;
}

pair<double, LatLng> LatLng::ProjectOnSegment(const double &lat, const double &lng,
                                              const LatLng &A, const LatLng &B)
{
    const double A_lat              = A.getLat();
    const double A_lng              = A.getLng();
    const double B_lat              = B.getLat();
    const double B_lng              = B.getLng();
    const double slope_vector_lat   = B_lat - A_lat;
    const double slope_vector_lng   = B_lng - A_lng;
    const double rel_coordinate_lat = lat - A_lat;
    const double rel_coordinate_lng = lng - A_lng;

    // Dot product of two un-normed vectors
    const double unnormed_ratio = slope_vector_lat * rel_coordinate_lat +
                                  slope_vector_lng * rel_coordinate_lng;
    // Squared length of the slope vector
    const double squared_length = slope_vector_lat * slope_vector_lat +
                                  slope_vector_lng * slope_vector_lng;

    // Avoid division by 0
    if (squared_length < std::numeric_limits<double>::epsilon())
    {
        return {0, A};
    }

    const double normed_ratio = unnormed_ratio / squared_length;

    double clamped_ratio = normed_ratio;
    if (clamped_ratio > 1)
    {
        clamped_ratio = 1;
    }
    else if (clamped_ratio < 0)
    {
        clamped_ratio = 0;
    }

    return {
        clamped_ratio,
        LatLng((1.0 - clamped_ratio) * A_lat + B_lat * clamped_ratio,
               (1.0 - clamped_ratio) * A_lng + B_lng * clamped_ratio)
    };
}

double LatLng::DistanceInMeters(const double &lat1, const double &lng1,
                                const double &lat2, const double &lng2)
{
    // Great circle distance
    const double x1 = lat1 * DEG_TO_RAD;
    const double y1 = lng1 * DEG_TO_RAD;
    const double x2 = lat2 * DEG_TO_RAD;
    const double y2 = lng2 * DEG_TO_RAD;
    const double x  = x2 - x1;
    const double y  = (y2 - y1) * cos((x1 + x2) / 2.0);
    return hypot(x, y) * EARTH_RADIUS_IN_METERS;

//    Haversine distance
//
//    double lat_arc = (lat1 - lat2) * DEG_TO_RAD;
//    double lng_arc = (lng1 - lng2) * DEG_TO_RAD;
//    double lat_h   = sin(lat_arc * 0.5);
//    double lng_h   = sin(lng_arc * 0.5);
//    lat_h *= lat_h;
//    lng_h *= lng_h;
//    double temp = cos(lat1 * DEG_TO_RAD) * cos(lat2 * DEG_TO_RAD);
//    return EARTH_RADIUS_IN_METERS * 2.0 * asin(sqrt(lat_h + temp * lng_h));
}

bool LatLng::Validate(const double &lat, const double &lng)
{
    return lat > -90 &&
           lat < 90 &&
           lng > -180 &&
           lng < 180;
}

bool LatLng::operator==(const LatLng &other)
{
    return lat_int == other.lat_int &&
           lng_int == other.lng_int;
}

string LatLng::toString() const
{
    return Util::DoubleToString(getLat(), 7) + " " +
           Util::DoubleToString(getLng(), 7);
}