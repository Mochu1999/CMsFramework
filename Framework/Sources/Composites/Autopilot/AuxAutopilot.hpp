#pragma once






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



const float earthRadius = 6378137.0f;
//Web mercator projection. Output in mercator projected meters
// In the equator the distances are exact (cilindrical proj) but you lose accuracy the more away you are from it
// Mercator is only valid for visualization, otherwise use geodesic calculations
//assumes you wont have latitudes close to +-90 for now. lonlats in degrees
vector<p2> lonLatToMercator(const vector<p2> lonLats)
{
	vector<p2> positions;
	positions.reserve(lonLats.size());

	for (auto& ll : lonLats)
	{
		float lambda = radians(ll.x);   // lon in radians
		float phi = radians(ll.y);   // lat in radians



		positions.push_back({ earthRadius * lambda, earthRadius * log(tan((3.14159265359f / 4.0f) + (phi / 2.0f))) });
	}

	return positions;
}

p2 lonLatToMercator(const p2 lonLat) {

	float lambda = radians(lonLat.x);   // lon in radians
	float phi = radians(lonLat.y);   // lat in radians

	return { earthRadius * lambda, earthRadius * log(tan((3.14159265359f / 4.0f) + (phi / 2.0f))) };
}



//Comprobar
vector<p2> mercatorToLonLat(const vector<p2> coords) {
	vector<p2> lonLats;
	lonLats.reserve(coords.size());

	for (auto& coord : coords) {
		// Convertir x (Mercator) a longitud (lambda)
		float lambda = coord.x / earthRadius;

		// Convertir y (Mercator) a latitud (phi)
		float y = coord.y / earthRadius;
		float phi = 2.0f * atan(exp(y)) - PI / 2.0f;

		// Convertir radianes a grados
		float lon = lambda * 180.0f / PI;
		float lat = phi * 180.0f / PI;

		lonLats.push_back({ lon, lat });
	}

	return lonLats;
}

//Comprobar
p2 mercatorToLonLat(const p2 pos) {


	// Convertir x (Mercator) a longitud (lambda)
	float lambda = pos.x / earthRadius;

	// Convertir y (Mercator) a latitud (phi)
	float y = pos.y / earthRadius;
	float phi = 2.0f * atan(exp(y)) - PI / 2.0f;

	// Convertir radianes a grados
	float lon = lambda * 180.0f / PI;
	float lat = phi * 180.0f / PI;



	return { lon, lat };
}

//the map format that gives you longitudes and latitudes is WGS84 (EPSG:4326)

//For very small angles the function collapses as the cosine is 0.9999... and rounded to 1
//Calculates the distance of 2 spherical points in meter (input in degrees)
float calculateDistanceOld(const p2 point1, const p2 point2) {


	float lon1 = radians(point1.x);
	float lat1 = radians(point1.y);
	float lon2 = radians(point2.x);
	float lat2 = radians(point2.y);


	//θ = arccos(sin(φ1)sin(φ2) + cos(φ1)cos(φ2)cos(Δλ))
	float numerator = std::sin(lat1) * std::sin(lat2) + std::cos(lat1) * std::cos(lat2) * std::cos(lon2 - lon1);

	// See if this even happens
	if (numerator > 1.0f) numerator = 1.0f;
	if (numerator < -1.0f) numerator = -1.0f;

	float theta = std::acos(numerator);

	return earthRadius * theta;
}

//Haversine formula, good for small angles. Logic to be revised but it's working
float calculateDistance(p2 a, p2 b) {
	const double R = 6378137.0;
	double lon1 = radians(a.x), lat1 = radians(a.y);
	double lon2 = radians(b.x), lat2 = radians(b.y);
	double dlon = lon2 - lon1, dlat = lat2 - lat1;
	double h = sin(dlat / 2) * sin(dlat / 2)
		+ cos(lat1) * cos(lat2) * sin(dlon / 2) * sin(dlon / 2);
	double c = 2 * atan2(sqrt(h), sqrt(1 - h));
	return float(R * c);
}


float meterSecondToKnot(const float input)
{
	return input * 3600 / 1852;
}



p3 lonLatsTo3DVector(p2 lonLat)
{
	float lon = radians(lonLat.x);
	float lat = radians(lonLat.y);

	return { cos(lat) * cos(lon),cos(lat) * sin(lon),sin(lat) };
}

//it must be checked before entering the function that start and end do not coincide
vector<p2> divideGreatCircle(p2 lonLatStart, p2 lonLatEnd, float distanceThreshold)
{
	vector<p2> intermediatePoints;

	p3 start = lonLatsTo3DVector(lonLatStart);
	p3 end = lonLatsTo3DVector(lonLatEnd);

	//normal of the plane (unnormalized for the angle)
	p3 normal = cross3(start, end);
	float totalAngle = atan2(magnitude3(normal), dot3(start, end));

	normal = normalize3(normal);

	float stepAngle = distanceThreshold / earthRadius; //radians



	for (float angle = stepAngle; angle < totalAngle; angle += stepAngle)
	{
		p3 currentPoint = start;
		rotatePoint(currentPoint, angle, normal);

		intermediatePoints.push_back({ degrees(atan2(currentPoint.y, currentPoint.x)) ,degrees(asin(currentPoint.z)) });
	}


	return intermediatePoints;

}



//surveyor's formula
float computeArea(const vector<p2>& polygon)
{
	float area = 0.0f;

	for (int i = 0; i < polygon.size() - 1; ++i)
	{
		area += polygon[i].x * polygon[i + 1].y - polygon[i + 1].x * polygon[i].y;
	}
	return area * 0.5f;
}




//returns the input with polygons swapped to CCW
vector<vector<p2>> setPolygonsCCW(vector<vector<p2>>& input)
{
	vector<vector<p2>> output = input;

	for (auto& polygon : output)
	{
		if (computeArea(polygon) < 0)
		{
			cout << "Detected cw polygon" << endl;
			std::reverse(polygon.begin(), polygon.end());
		}
	}

	return output;
}


//DEPRECATED, it is usefull because ascii, but too slow
//The function expect to finds in each line of the file polylines
//First the number of vertices, then all the points separated by commas
//After storing of the polylines (actual world coordinates), it projects them all
// , sets the first polyline as a frame, returns the 3 variables as references
void readTextOfLonLats(std::string relativePath, std::vector<std::vector<p2>>& allLonLats, Lines2D& mercator, Lines2D& frame)
{
	std::string basePath = "Resources/";
	std::string path = basePath + relativePath;

	std::ifstream inFile(path);
	if (!inFile)
	{
		std::cerr << "Error opening file for reading: " << path << "\n";
		return;
	}

	std::string line;
	while (std::getline(inFile, line))
	{
		if (line.empty())
			continue;


		std::stringstream ss(line);

		size_t numPoints = 0;
		if (!(ss >> numPoints)) //ss >> numPoints  reads the first token it can parse
		{
			// If we can't read size, format might be wrong or we reached end of file
			continue;
		}

		std::vector<p2> polyline;
		polyline.reserve(numPoints);

		for (std::size_t i = 0; i < numPoints; ++i)
		{
			p2 point;

			if (!(ss >> point.x >> point.y))
				break;

			// skip spaces/tabs only (faster than std::ws)
			while (ss.peek() == ' ' || ss.peek() == '\t')
				ss.get();

			if (ss.peek() == ',')
				ss.get(); // removes comma

			while (ss.peek() == ' ' || ss.peek() == '\t')
				ss.get();

			polyline.push_back(point);
		}


		allLonLats.push_back(polyline);
	}


	//Starting with the first point after the frame
	for (size_t i = 1; i < allLonLats.size(); i++)
	{
		vector<p2>interm = lonLatToMercator(allLonLats[i]);
		mercator.addSet(interm);
	}
	frame.addSet(lonLatToMercator(allLonLats[0]));
}




void readMapBinary(const std::string& nameText, std::vector<std::vector<p2>>& map, Lines2D& mercator, Lines2D& frame)
{
    std::string path = "Resources/MRS/" + nameText;

    std::ifstream inFile(path, std::ios::binary);
    if (!inFile) {
        std::cerr << "Error opening file for reading: " << path << "\n";
    }

    while (true)
    {
        size_t size;
        inFile.read(reinterpret_cast<char*>(&size), sizeof(size)); //single polygon size
        if (!inFile) break;  // EOF reached or error

        std::vector<p2> interm(size);
        inFile.read(reinterpret_cast<char*>(interm.data()), size * sizeof(p2));
        if (!inFile) break;  

        map.push_back(std::move(interm));
    }

	//map = setPolygonsCCW(map);
	

	//Starting with the first point after the frame
	for (size_t i = 1; i < map.size(); i++)
	{
		vector<p2>interm = lonLatToMercator(map[i]);
		mercator.addSet(interm);
	}
	frame.addSet(lonLatToMercator(map[0]));
}



void writeMapBinary(const vector<vector<p2>>& map, string nameText) {

	std::string path = "Resources/MRS/" + nameText;

	std::ofstream outFile(path, std::ios::binary);
	if (outFile)
	{
		for (auto& polygon : map)
		{

			size_t size = polygon.size();
			outFile.write(reinterpret_cast<const char*>(&size), sizeof(size));


			outFile.write(reinterpret_cast<const char*>(polygon.data()), size * sizeof(p2));
		}
	}
	else
	{
		std::cerr << "Error opening file for writing." << std::endl;
	}
	outFile.close();
}