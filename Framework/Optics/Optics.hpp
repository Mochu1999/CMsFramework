#pragma once

//TO DO
//Light from a line instead of a point
//Parabolic mirrors
// Linking multiple mirrors
//Lens apart from mirrors
//3D application



//Produces spherical aberration
//CONSTRUCTOR PARA THETAS QUE CHECKEE QUE THETA0 <THETA1
//SI isbetweenangles SOLO SE USA EN SPHERICAL MOVER A DENTRO
struct SphericalLens
{
	Lines2D body;
	vector<p2> positions;

	float r;
	p2 center;

	float theta0 = radians(90);
	float theta1 = radians(360);

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
			positions.push_back({ center.x + r * cos(theta),center.y + r * sin(theta) });
		}

		body.addSet(positions);
	}
};




struct Optics
{


	Shader& shader2D;
	GlobalVariables& gv;

	p2 lightPosition = { 1500,500 };
	Circles lightPoint;
	Lines2D rays;


	SphericalLens lens;

	//Does not interact with anything
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
		generateRays();
	}

	void draw()
	{


		shader2D.bind();
		transparent();
		shader2D.setUniform("u_Model", identityMatrix);
		shader2D.setUniform("u_Color", 1.0f, 1.0f, 1.0f, 0.3f);
		rays.draw();

		shader2D.setUniform("u_Color", 0.0f, 0.0f, 1.0f, 1.0f);
		//lightPoint.secondRayDirawCircunference();
		lens.body.draw();
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

	//Generate rays and and their ray tracing from a point
	void generateRays()
	{
		//Lines2D
		rays.clear();

		//Direct variables for the lens parameters
		const p2  lensCenter = lens.center;
		const float lensR = lens.r;
		const float theta0 = lens.theta0;
		const float theta1 = lens.theta1;

		const float Lmax = 1000000.0f; //It's meant to be infinite



		const int maxIntersections = 4;
		for (float i = 0.0f; i < 2.0f * PI - 10e-4; i += radians(0.05))
		{
			int intersections = 0;

			p2 rayOrigin = lightPosition;
			vector<p2> positions = { rayOrigin };
			//R(s) = ligthPos + s*rayDir, where R is each ray position and s the distance from the light
			//We are calculating that distance s (else there's no intersection)
			// direction the ray travels from lightPosition, already unit
			p2 rayDir = { cos(i), sin(i) };
			while (intersections < maxIntersections)
			{
				p2 lightLensDir = rayOrigin - lensCenter;
				//whatever circle follows the equation (x-Cx)^2+y(y-Cy)^2=r^2
				//If we treat these as a vec2 we end with |X-C|^2 = r^2    //"||" is the magnitude, ||^2 is the dot product
				//Now if we look for the intersection between R and the circle we end with
				//|(ligthPos+s*lightDir)-C|^2=r^2

				//If we expand the dot product, we end with the quasecondRayDiratic: A*s^2+B*s+C = 0
				//Where: 
				// A = lightDir·lightDir
				// B = 2(lightDir · (lightPosition - lensCenter)
				// C = (lightPosition - lensCenter) · (lightPosition - lensCenter) - r^2
				float A = 1;// cos^2i+sin^2i // dot2(rayDir, rayDir);
				float B = 2.0f * dot2(rayDir, lightLensDir);
				float C = dot2(lightLensDir, lightLensDir) - lensR * lensR;

				//Discriminat of the quasecondRayDiratic equation (B^2-4AC) >= 0
				float discr = B * B - 4.0f * A * C;

				//There won't be solution without a positive discriminant
				if (discr < eps)
				{
					break;
				}
				//we get the 2 solutions of the quasecondRayDiratic equation, 2 s, s0 will always the be smaller distance
				//THERE'S AN EDGE CASE IF LIGHT IS INSIDE THE CIRCLE
				float sqrtDiscr = sqrt(discr);
				float s0 = (-B - sqrtDiscr) / (2.0f * A);
				float s1 = (-B + sqrtDiscr) / (2.0f * A);

				//R(s) evaluated at the intersection point s
				p2 R;
				//an R will be defined in this loop no matter what, with this bool we can kill the ray if R isn't in between the angles
				bool hit = false; 
				for (float s : { s0, s1 })
				{
					if (s <= 10e-3) continue; //setting it too low actually provokes errors of self intersetion

					// possible R(s)
					R = rayOrigin + rayDir * s;

					// If R, which will be in the circunference, is in an angle in between that of our arc, then it's it
					float thetaCand = atan2(R.y - lensCenter.y, R.x - lensCenter.x);
					if (isAngleBetween(thetaCand, theta0, theta1))
					{
						hit = true;
						break;
					}
				}
				if (!hit)
				{
					positions.push_back(rayOrigin + rayDir * Lmax);
					break;
				}
				//if there is a hit, we are still at 
				positions.push_back(R);
				intersections++;

				// reflection normal
				p2 n = normalize2(R - lensCenter);
				rayDir = rayDir - n * (2.0f * dot2(rayDir, n));

				// move origin slightly off surface
				float side = (dot2(rayDir, n) > 0.0f) ? 1.0f : -1.0f;
				rayOrigin = R + n * (side * eps);
			}

			if (intersections>0 && intersections <= maxIntersections)
			{
				rays.addSet(positions);
			}

		}
	}


};