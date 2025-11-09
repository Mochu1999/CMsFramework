#pragma once
#include "Common.hpp"
#include "Lines3D.hpp"



//In the documentationis is explained the catenary equations and logic
struct Mooring
{
	Lines3D line;
	vector<p3> positions;
	int n = 60;

	p3 fixedPoint;
	p3& connectionPoint;

	float v = 161; //m

	//Poliester rope
	float d = 0.2;
	float E = 10E+8;
	float rhoRope = 1380;
	float rhoWater = 1025;
	float A = PI * d * d / 4;
	float w = (rhoRope - rhoWater) * g * A; //N/m


	//horizontal distance,
	float H = sqrt((connectionPoint.x - fixedPoint.x) * (connectionPoint.x - fixedPoint.x)
		+ (connectionPoint.z - fixedPoint.z) * (connectionPoint.z - fixedPoint.z));

	float L =  1.03 * sqrt(H * H + v * v);

	float a;//catenary parameter [m], higher a higher tension

	Mooring(p3 fixedPoint_, p3& connectionPoint_)
		:fixedPoint(fixedPoint_), connectionPoint(connectionPoint_)
	{
		update();

		generateCurve();
		print(positions);
	}

	void update()
	{
		calculateA();

		print(w);
		print(a);
		print(L);
		print(H);
		print(v);

		float AL = L / a, BV = v / a, D = 0.5 * sqrt(AL * AL - BV * BV);
		float u = atanh(BV / AL);
		float s1 = a * sinh(u - asinh(D)), s2 = a * sinh(u + asinh(D));

		float T0 = w * a;
		float Tfixed = w * sqrt(a * a + s1 * s1);
		float Tconn = w * sqrt(a * a + s2 * s2);

		print(T0);
		print(Tfixed);
		print(Tconn);

		generateCurve();
		getCurveLength();
	}

	void calculateA()
	{
		float dx = connectionPoint.x - fixedPoint.x;
		float dz = connectionPoint.z - fixedPoint.z;
		float Hc = sqrt(dx * dx + dz * dz);
		float S = sqrt(L * L - v * v);

		float t0 = 0.0, t1 = 1.0;
		while (Hc * sinh(t1) - S * t1 <= 0.0) t1 *= 2.0;
		for (int i = 0; i < 60; ++i)
		{
			float tm = 0.5 * (t0 + t1);
			(Hc * sinh(tm) - S * tm <= 0.0 ? t0 : t1) = tm;
		}
		a = Hc / (2.0 * 0.5 * (t0 + t1));
	}

	void generateCurve()
	{
		positions.clear();
		positions.reserve(n + 1);

		// Use actual vertical gap so the last point is exactly connectionPoint
		double vY = double(connectionPoint.y) - double(fixedPoint.y);

		double AL = double(L) / double(a);
		double BV = vY / double(a);
		double D = 0.5 * sqrt(AL * AL - BV * BV);
		double u = atanh(BV / AL);
		double u1 = u - asinh(D), u2 = u + asinh(D);
		double du = (u2 - u1) / double(n);
		double denom = double(a) * (u2 - u1);

		for (int i = 0; i <= n; ++i)
		{
			double ui = u1 + i * du;
			double xLocal = double(a) * (ui - u1);
			double yLocal = double(a) * (cosh(ui) - cosh(u1));

			double t = xLocal / denom; // 0..1 along plan
			float x = float(double(fixedPoint.x) + (double(connectionPoint.x) - double(fixedPoint.x)) * t);
			float z = float(double(fixedPoint.z) + (double(connectionPoint.z) - double(fixedPoint.z)) * t);
			float y = float(double(fixedPoint.y) + yLocal);

			positions.push_back({ x, y, z });
		}

		line.addSet(positions);
	}

	void getCurveLength()
	{
		float total = 0.0f;
		for (size_t i = 1; i < positions.size(); ++i)
		{
			p3 d = { positions[i].x - positions[i - 1].x,
					 positions[i].y - positions[i - 1].y,
					 positions[i].z - positions[i - 1].z };
			total += sqrtf(d.x * d.x + d.y * d.y + d.z * d.z);
		}
		print(total);
		print(L);
	}

};