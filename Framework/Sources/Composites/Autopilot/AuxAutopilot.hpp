#pragma once



string formatFloat(float value)
{
	std::ostringstream oss;
	oss << std::fixed << std::setprecision(2) << value;
	return oss.str();
}


string lonLatToString(p2 lonLat)
{
	string lonStr, latStr;
	float lon = lonLat.x, lat = lonLat.y;

	if (lon >= 0)
		lonStr = formatFloat(lon) + "E";
	else if (lon < 0)
		lonStr = formatFloat(-lon) + "W";

	if (lat >= 0)
		latStr = formatFloat(lat) + "N";
	else if (lat < 0)
		latStr = formatFloat(-lat) + "S";

	return "{" + lonStr + ", " + latStr + "}";
}