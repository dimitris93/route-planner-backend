#include "GpsCoordinate.h"

GpsCoordinate::GpsCoordinate(const double& lat, const double& lng) :
	lat_int(round(lat * COORDINATE_TO_INT)),
	lng_int(round(lng * COORDINATE_TO_INT))
{
	// Check for invalid coordinates
	if (!Validate(lat, lng))
	{
		cout << "Latitude or Longitude does not meet requirements: { "
			 << Util::DoubleToString(lat) << ", "
			 << Util::DoubleToString(lng) << " }." << endl;
		exit(-1);
	}
}

double GpsCoordinate::GetLat() const
{
	return lat_int * INT_TO_COORDINATE;
}

double GpsCoordinate::GetLng() const
{
	return lng_int * INT_TO_COORDINATE;
}

GpsCoordinate GpsCoordinate::ClosestPointToLineSegment(const GpsCoordinate& P,
													   const GpsCoordinate& A,
													   const GpsCoordinate& B)
{
	// Check for invalid coordinates
	if (A == B)
	{
		cout << "Line segment AB cannot be defined when A == B.  " << A.ToString() << endl;
		exit(-1);
	}

	const double xa = A.GetLat();
	const double ya = A.GetLng();

	const double xb = B.GetLat();
	const double yb = B.GetLng();

	const double xp = P.GetLat();
	const double yp = P.GetLng();

	const double x_ap = xp - xa;
	const double y_ap = yp - ya;

	const double x_ab = xb - xa;
	const double y_ab = yb - ya;

	const double ap_dot_ab = x_ap * x_ab + y_ap * y_ab;
	const double ab_dot_ab = x_ab * x_ab + y_ab * y_ab;
	const double ratio     = ap_dot_ab / ab_dot_ab;

	if (ratio < 0)
	{
		return (A);
	}
	else if (ratio > 1)
	{
		return (B);
	}
	else
	{
		return {GpsCoordinate(xa * (1.0 - ratio) + xb * ratio,
							  ya * (1.0 - ratio) + yb * ratio)};
	}
}

double GpsCoordinate::DistanceInMeters(const double& lat1, const double& lng1,
									   const double& lat2, const double& lng2)
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

bool GpsCoordinate::Validate(const double& lat, const double& lng)
{
	int pos;

	string lat_str = Util::DoubleToString(lat);
	lat_str.erase(lat_str.find_last_not_of('0') + 1, string::npos);   // remove trailing zeros
	pos = lat_str.find('-');
	if (pos != string::npos)
	{
		lat_str.erase(pos, 1);   // remove minus sign, if present
	}
	pos = lat_str.find('.');
	if (pos != string::npos)
	{
		lat_str.erase(pos, 1);   // remove dot, if present
	}

	string lng_str = Util::DoubleToString(lng);
	lng_str.erase(lng_str.find_last_not_of('0') + 1, string::npos);   // remove trailing zeros
	pos = lng_str.find('-');
	if (pos != string::npos)
	{
		lng_str.erase(pos, 1);   // remove minus sign, if present
	}
	pos = lng_str.find('.');
	if (pos != string::npos)
	{
		lng_str.erase(pos, 1);   // remove dot, if present
	}

	return lat_str.length() <= MAXIMUM_DIGITS &&
		   lng_str.length() <= MAXIMUM_DIGITS &&
		   lat <= MAX_LAT &&
		   lat >= MIN_LAT &&
		   lng <= MAX_LNG &&
		   lng >= MIN_LNG;
}

bool GpsCoordinate::operator==(const GpsCoordinate& other) const
{
	return lat_int == other.lat_int &&
		   lng_int == other.lng_int;
}

string GpsCoordinate::ToString(int n_decimals) const
{
	return "{ " + Util::DoubleToString(GetLat()) + ", " + Util::DoubleToString(GetLng()) + " }";
}