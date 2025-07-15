#pragma once


#include "Common.hpp"
//en lonLatToMercator hay un float(PI), hay que modificar PI
//RESERVES

//QUITO EL ÚLTIMO PUNTO PARA QUE NO DE LA VUELTA ENTERA

//Horizontal lines of a unity globe. Radians
//For the middle line Starts creating in -PI (-1,0,0) and ends in +PI (-1,0,0) to have the 0 centered (1,0,0). 
vector<p3> createLatitude(float angleLatitude, int points = 100) {

	vector<p3> positions;
	float angleBetweenLongitudes = 2.0f * PI / points;

	//Height of the xz plane
	float y = sin(angleLatitude);

	//the distance from the OY axis to all the circle points
	float r = cos(angleLatitude);

	for (int i = 0; i <= points; i++)
	{
		float lon = -PI + i * angleBetweenLongitudes;
		positions.push_back({ r * cos(lon), y, r * sin(lon) });
	}

	return positions;
}


//Vertical lines of a globe. 
//For the middle line starts and ends in the same point as createLatitudes
vector<p3> createLongitude(float angleLongitude, int points = 100) {

	vector<p3> positions;
	float angleBetweenLatitudes = 2.0f * PI / points;

	for (int i = 0; i <= points; i++) {
		float lat = -PI + i * angleBetweenLatitudes;

		float x = cos(lat) * cos(angleLongitude);
		float y = sin(lat);
		float z = cos(lat) * sin(angleLongitude);

		positions.push_back({ x, y, z });
	}
	return positions;
}

//// .x is longitude, .y is latitude
//vector<p2> lonLat(const vector<p3> positions_) {
//	vector<p2> positions;
//
//	for (size_t i = 0; i < positions_.size(); i++)
//	{
//		positions.push_back({ atan2f(positions_[i].z, positions_[i].x) ,asinf(positions_[i].y) });
//	}
//	return positions;
//}


float earthRadius = 6378137.0f;
//Web mercator projection. Outoput in mercator projected meters
// In the equator the distances are exact (cilindrical proj) but you lose accuracy the more away you are from it
// Mercator is only valid for visualization, otherwise use geodesic calculations
//assumes you wont have latitudes close to +-90 for now. lonlats in degrees
vector<p2> lonLatToMercator(const vector<p2> lonLats) {
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

//If the distance doesn't make sense dude to very small angles use Haversine formula
//Calculates the distance of 2 spherical points in meter (input in degrees)
float calculateDistance(const p2 point1, const p2 point2) {


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


float meterSecondToKnot(const float input)
{
	return input * 3600 / 1852;
}











//// Esta función devuelve la distancia sobre la esfera (de radio 'radius')
//// entre el punto1(lat1, lon1) y el punto2(lat2, lon2), 
//// asumiendo que lat1, lon1, lat2, lon2 ya están en radianes.
//double calculateDistance(p2 point1,p2 point2)
//{
//	float lon1 = radians(point1.x);
//	float lat1 = radians(point1.y);
//	float lon2 = radians(point2.x);
//	float lat2 = radians(point2.y);
//	//--------------------------------------------------------------------------
//	// 1) Convertir cada punto (lat, lon) en coordenadas esféricas a vector 3D.
//	//    Suponiendo un sistema con:
//	//      - lat = 0 en el ecuador (positivo hacia el norte),
//	//      - lon = 0 en el meridiano principal (positivo hacia el este),
//	//      - radius = distancia desde el centro de la esfera.
//	//--------------------------------------------------------------------------
//
//	// Primer vector (x1, y1, z1)
//	double x1 = earthRadius * std::cos(lat1) * std::cos(lon1);
//	double y1 = earthRadius * std::cos(lat1) * std::sin(lon1);
//	double z1 = earthRadius * std::sin(lat1);
//
//	// Segundo vector (x2, y2, z2)
//	double x2 = earthRadius * std::cos(lat2) * std::cos(lon2);
//	double y2 = earthRadius * std::cos(lat2) * std::sin(lon2);
//	double z2 = earthRadius * std::sin(lat2);
//
//	//--------------------------------------------------------------------------
//	// 2) Calcular el producto punto (dot product) de ambos vectores
//	//--------------------------------------------------------------------------
//	double dot = (x1 * x2) + (y1 * y2) + (z1 * z2);
//
//	//--------------------------------------------------------------------------
//	// 3) Calcular las magnitudes de cada vector (que idealmente será 'radius' 
//	//    para ambos si no hay errores de redondeo, pues los 2 puntos están 
//	//    en la misma esfera).
//	//--------------------------------------------------------------------------
//	double mag1 = std::sqrt(x1 * x1 + y1 * y1 + z1 * z1);
//	double mag2 = std::sqrt(x2 * x2 + y2 * y2 + z2 * z2);
//
//	//--------------------------------------------------------------------------
//	// 4) Calcular el ángulo entre los dos vectores usando la definición
//	//    dot(A,B) = |A| * |B| * cos(angulo)
//	//--------------------------------------------------------------------------
//	double cosAngle = dot / (mag1 * mag2);
//
//	// Debido a posibles errores de redondeo, forzamos cosAngle a estar
//	// en el rango [-1, 1] antes de hacer acos
//	if (cosAngle > 1.0)  cosAngle = 1.0;
//	if (cosAngle < -1.0) cosAngle = -1.0;
//
//	double angle = std::acos(cosAngle);
//
//	//--------------------------------------------------------------------------
//	// 5) La distancia sobre la superficie de la esfera es:
//	//         distancia = radio * (ángulo en radianes)
//	//--------------------------------------------------------------------------
//	return earthRadius * angle;
//}

