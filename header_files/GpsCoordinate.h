#ifndef GPSCOORDINATE_H
#define GPSCOORDINATE_H

#include "Util.h"

using namespace std;

/*
 * A latitude,longitude coordinate with precision to 7 decimals.
 * Which translates to about 1cm accuracy.
 * The latitude and longitude are stored as integer.
 * We multiply or divide by 10.000.000 to switch between the actual and the compressed value.
 * This is because the float data type seems to not provide us with enough precision on decimal values.
 */
class GpsCoordinate
{
public:
	GpsCoordinate(const double& lat, const double& lng);
	~GpsCoordinate() = default;

	double               GetLat() const;                                     // Actual Latitude value as double
	double               GetLng() const;                                     // Actual Longitude value as double
	static GpsCoordinate ClosestPointToLineSegment(const GpsCoordinate& p,   // Project point to road segment.
												   const GpsCoordinate& A,   // Returns the projected point
												   const GpsCoordinate& B);
	static double        DistanceInMeters(const double& lat1,
										  const double& lng1,
										  const double& lat2,
										  const double& lng2);   // Distance in meters between 2 coordinates
	static bool          Validate(const double& lat,
								  const double& lng);                    // Returns true if it is a GPS coordinate
	bool                 operator==(const GpsCoordinate& other) const;   // Check if this coordinate == other
	string               ToString(int n_decimals = DECIMAL_PRECISION) const;

private:
	static const int        MAX_LAT                = 90;
	static const int        MIN_LAT                = -90;
	static const int        MAX_LNG                = 180;
	static const int        MIN_LNG                = -180;
	static const int        MAXIMUM_DIGITS         = 10;   // Maximum number of total digits without counting "."
	static const int        DECIMAL_PRECISION      = 7;    // Maximum number of digits after "."
	static const int        COORDINATE_TO_INT      = 10000000;
	static constexpr double INT_TO_COORDINATE      = 1.0 / 10000000;
	static constexpr double DEG_TO_RAD             = 3.141592653589793 / 180.0;
	static const int        EARTH_RADIUS_IN_METERS = 6378137;

	int lat_int;   // Compressed Latitude as integer
	int lng_int;   // Compressed Longitude as integer
};

#endif   // GPSCOORDINATE_H