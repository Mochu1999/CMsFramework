void generateRays()
{
	//Lines2D
	rays.clear();

	//Direct variables for the lens parameters
	const p2  lensCenter = mirror.center;
	const float lensR = mirror.r;
	const float theta0 = mirror.theta0;
	const float theta1 = mirror.theta1;

	const float Lmax = 1000000.0f; //It's meant to be infinite



	const int maxIntersections = 4;
	for (float i = 0.0f; i < 2.0f * PI - 10e-4; i += radians(0.05))
	{
		int intersections = 0;

		p2 rayOrigin = position;
		vector<p2> positions = { rayOrigin };
		//R(s) = ligthPos + s*rayDir, where R is each ray position and s the distance from the light
		//We are calculating that distance s (else there's no intersection)
		// direction the ray travels from position, alreadv unit
		p2 rayDir = { cos(i), sin(i) };
		while (intersections < maxIntersections)
		{
			p2 lightLensDir = rayOrigin - lensCenter;
			//whatever circle follows the equation (x-Cx)^2+y(y-Cy)^2=r^2
			//If we treat these as a vec2 we end with |X-C|^2 = r^2    //"||" is the length, ||^2 is the dot product
			//Now if we look for the intersection between R and the circle we end with
			//|(ligthPos+s*lightDir)-C|^2=r^2

			//If we expand the dot product, we end with the quasecondRayDiratic: A*s^2+B*s+C = 0
			//Where: 
			// A = lightDir·lightDir
			// B = 2(lightDir · (position - lensCenter)
			// C = (position - lensCenter) · (position - lensCenter) - r^2
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
			bool finalHit = false;
			for (float s : { s0, s1 })
			{
				if (s <= 10e-3) continue; //setting it too low actually provokes errors of self intersetion

				// possible R(s)
				R = rayOrigin + rayDir * s;

				// If R, which will be in the circunference, is in an angle in between that of our arc, then it's it
				float thetaCand = atan2(R.y - lensCenter.y, R.x - lensCenter.x);
				if (mirror.isAngleBetween(thetaCand, theta0, theta1))
				{
					finalHit = true;
					break;
				}
			}
			if (!finalHit)
			{
				positions.push_back(rayOrigin + rayDir * Lmax);
				break;
			}
			//if there is a finalHit, we are still at 
			positions.push_back(R);
			intersections++;

			// reflection normal
			p2 n = normalize2(R - lensCenter);
			rayDir = rayDir - n * (2.0f * dot2(rayDir, n));

			// move origin slightly off surface
			float side = (dot2(rayDir, n) > 0.0f) ? 1.0f : -1.0f;
			rayOrigin = R + n * (side * eps);
		}

		if (intersections > 0 && intersections <= maxIntersections)
		{
			rays.addSet(positions);
		}

	}