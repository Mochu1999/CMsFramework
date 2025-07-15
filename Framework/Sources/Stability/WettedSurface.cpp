#pragma once
#include "WettedSurface.hpp"


float WettedSurface::signedArea(const p3& a, const p3& b, const p3& c) {
	// returns is the determinant of |ab,ac| 
	return (b.x - a.x) * (c.z - a.z) - (c.x - a.x) * (b.z - a.z);
}

bool WettedSurface::isPointInTriangle(const p3& a, const p3& b, const p3& c, const p3& p) {
	float signAB, signBC, signCA;

	//we want the areas to be of a consistent sign, if all have the same sign it is because P is inside abc
	signAB = signedArea(p, a, b);
	signBC = signedArea(p, b, c);
	signCA = signedArea(p, c, a);


	//print(signAB);
	//print(signBC);
	//print(signCA);

	return ((signAB <= 0) && (signBC <= 0) && (signCA < 0));
}

bool WettedSurface::calculateIntersectionPoint(const p3& a, const p3& b, const p3& c, const p3& l1, const p3& l2, p3& p) {
	

	p3 normal = cross3(b - a, c - b);

	// Calculate D, the scalar component of the plane equation
	float D = -dot3(normal, a);

	float denominator = dot3(normal, l2 - l1);

	if (fabs(denominator) < 1e-6) // Use a small epsilon to handle floating-point inaccuracies
		return false;

	float t = -(dot3(normal, l1) + D) / denominator;

	p = l1 + (l2 - l1) * t;

	return isPointInTriangle(a, b, c, p);
}
