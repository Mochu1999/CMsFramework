#pragma once


struct Optics
{
	struct SphericalLens
	{
		Lines2D body;
		vector<p2> positions;

		float r;
		p2 center;

		float theta0 = radians( - 40);
		float theta1 = radians(40);

		SphericalLens(float r_, p2 center_) :r(r_), center(center_)
		{
			generateSphericalLens();
		}

		void generateSphericalLens()
		{
			int segments = (int)r * 3; //arbitrary, only visual
			if (segments < 50) segments = 50;
			float angleSegment = 2 * PI / segments; //angle bewteen points in radians

			positions.reserve(segments + 1);

			

			float dtheta = (theta1 - theta0) / segments;

			for (int i = 0; i <= segments; ++i)
			{
				float theta = theta0 + dtheta * i;
				positions.push_back({center.x + r * cos(theta),center.y + r * sin(theta)});
			}

			body.addSet(positions);
		}
	};

	Shader& shader2D;
	GlobalVariables& gv;

	p2 lightPosition = { 1500,500 };
	Circles lightPoint;
	Lines2D rays;


	SphericalLens lens;

	void generateRadialRays(float stepDeg, float length)
	{
		rays.clear();

		const float step = stepDeg * (PI / 180.0f);


		for (float a = 0.0f; a < 2.0f * PI - 10e-6; a += step)
		{
			p2 dir = { std::cos(a), std::sin(a) };
			p2 end = lightPosition + dir * length;
			rays.addSet({ lightPosition, end });
		}
	}

	Optics(Shader& shader2D_, GlobalVariables& gv_)
		:shader2D(shader2D_), gv(gv_)
		, lightPoint(10), lens(200, { 300,500 })
	{
		lightPoint.addSet(lightPosition);
		//generateRadialRays(1, 10000);
		traceRaysOnMirror();
	}

	static float wrapAngle(float a)
	{
		while (a <= -PI) a += 2.0f * PI;
		while (a > PI) a -= 2.0f * PI;
		return a;
	}

	static bool isAngleBetween(float theta, float theta0, float theta1)
	{
		theta = wrapAngle(theta);
		theta0 = wrapAngle(theta0);
		theta1 = wrapAngle(theta1);

		// Normal case: interval does not cross the -pi/pi boundary
		if (theta0 <= theta1)
			return (theta >= theta0 && theta <= theta1);

		// Wrapped case: interval crosses boundary
		return (theta >= theta0 || theta <= theta1);
	}

	void traceRaysOnMirror()
	{
		rays.clear();

		// Mirror analytic parameters (store these in Lens)
		const p2  C = lens.center;
		const float R = lens.r;
		const float t0 = lens.theta0;
		const float t1 = lens.theta1;

		const float eps = 1e-4f;
		const float step = radians(1.0f);
		const float Lmax = 10000.0f;

		for (float a = 0.0f; a < 2.0f * PI - 1e-6f; a += step)
		{
			// --- primary ray ---
			p2 O = lightPosition;
			p2 D = { std::cos(a), std::sin(a) }; // already unit

			// ---- analytic ray-circle intersection ----
			p2 oc = O - C;

			float A = dot2(D, D);              // =1
			float B = 2.0f * dot2(D, oc);
			float Cc = dot2(oc, oc) - R * R;

			float disc = B * B - 4.0f * A * Cc;
			if (disc < 0.0f)
			{
				// no hit: draw full ray
				rays.addSet({ O, O + D * Lmax });
				continue;
			}

			float s = std::sqrt(disc);
			float tHit0 = (-B - s) / (2.0f * A);
			float tHit1 = (-B + s) / (2.0f * A);

			float tHit = -1.0f;
			if (tHit0 > eps) tHit = tHit0;
			else if (tHit1 > eps) tHit = tHit1;

			if (tHit < 0.0f)
			{
				rays.addSet({ O, O + D * Lmax });
				continue;
			}

			p2 P = O + D * tHit;

			// ---- arc-angle gate ----
			float theta = std::atan2(P.y - C.y, P.x - C.x);

			if (!isAngleBetween(theta, t0, t1))
			{
				// hits circle outside mirror segment
				rays.addSet({ O, O + D * Lmax });
				continue;
			}

			// ---- draw incoming segment ----
			rays.addSet({ O, P });

			// ---- reflection ----
			p2 N = normalize2(P - C);          // circle normal
			p2 Dr = D - N * (2.0f * dot2(D, N));

			// ---- draw reflected segment ----
			p2 P2 = P + N * eps;
			rays.addSet({ P2, P2 + Dr * Lmax });
		}
	}

	void draw()
	{


		shader2D.bind();
		transparent();
		shader2D.setUniform("u_Model", identityMatrix);
		shader2D.setUniform("u_Color", 1.0f, 1.0f, 1.0f, 0.3f);
		rays.draw();

		shader2D.setUniform("u_Color", 0.0f, 0.0f, 1.0f, 1.0f);
		//lightPoint.drawCircunference();
		lens.body.draw();
	}
};