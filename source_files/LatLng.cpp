#include "LatLng.h"

LatLng::LatLng() :
	lat_int(0),
	lng_int(0)
{
}

LatLng::LatLng(const string& lat, const string& lng) :
	LatLng(stod(lat),
		   stod(lng))
{
}

LatLng::LatLng(const double& lat, const double& lng) :
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

double LatLng::GetLat() const
{
	return lat_int * INT_TO_COORDINATE;
}

double LatLng::GetLng() const
{
	return lng_int * INT_TO_COORDINATE;
}

LatLng LatLng::ClosestPointToLineSegment(const LatLng& p,
										 const LatLng& A,
										 const LatLng& B)
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

	const double xp = p.GetLat();
	const double yp = p.GetLng();

	const double ap_vec_x = xp - xa;
	const double ap_vec_y = yp - ya;

	const double ab_vec_x = xb - xa;
	const double ab_vec_y = yb - ya;

	const double ap_dot_ab = ap_vec_x * ab_vec_x + ap_vec_y * ab_vec_y;
	const double ab_dob_ab = ab_vec_x * ab_vec_x + ab_vec_y * ab_vec_y;
	const double ratio     = ap_dot_ab / ab_dob_ab;

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
		return {LatLng(xa * (1.0 - ratio) + xb * ratio,
					   ya * (1.0 - ratio) + yb * ratio)};
	}
}

double LatLng::DistanceInMeters(const double& lat1, const double& lng1,
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

bool LatLng::Validate(const double& lat, const double& lng)
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

bool LatLng::operator==(const LatLng& other) const
{
	return lat_int == other.lat_int &&
		   lng_int == other.lng_int;
}

string LatLng::ToString(int n_decimals) const
{
	return "{ " + Util::DoubleToString(GetLat()) + ", " + Util::DoubleToString(GetLng()) + " }";
}