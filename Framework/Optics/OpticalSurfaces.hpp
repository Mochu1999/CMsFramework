#pragma once

//Light data
struct LightPoint
{
	vector<std::pair<p2, p2>>& rayOrigin;
	p2 position;
	void createLightPoint(p2 position_)
	{
		position = position_;

		rayOrigin.clear();

		//for (float i = 0.0f; i < 2.0f * PI - 10e-4f; i += radians(0.5f))
		for (float i = radians(90); i < radians(270); i += radians(2.5f))
		{
			rayOrigin.emplace_back(position, p2{ cos(i), sin(i) });
		}
	}

};

struct LightLine
{
	vector<std::pair<p2, p2>>& rayOrigin;
	p2 middlePoint;
	p2 dir; // unit direction the rays will follow
	float width = 0.0f;

	p2 t;  // unit tangent along the segment
	int samples = 40; //number of rays

	void createLightLine(p2 point1, p2 point2, p2 direction)
	{
		p2 d = point2 - point1;
		float len = magnitude2(d);

		if (len < eps)
		{
			cout << "invalid input in createLightLine" << endl;
			return;
		}

		width = len;
		middlePoint = (point1 + point2) * 0.5f;

		t = d / len;
		dir = normalize2(direction);


		rayOrigin.clear();

		float halfW = 0.5f * width;
		for (int i = 0; i <= samples; ++i)
		{
			float u = -halfW + (width * i / samples);
			p2 origin = middlePoint + t * u;

			rayOrigin.emplace_back(origin, dir);

		}
	}
};

struct PlanarSurface
{
	Lines2D body;

	p2 middlePoint;
	p2 mirrorN;
	float width = 0.0f;

	//unit tangent, useful for intersection
	p2 t;

	PlanarSurface()
	{
	}

	void createPlanarSurface(p2 point1, p2 point2)
	{
		p2 d = point2 - point1;
		width = magnitude2(d);

		if (width < eps)
		{
			cout << "invalid input in createPlanarSurface (point1 == point2)" << endl;
			return;
		}

		middlePoint = (point1 + point2) * 0.5f;

		t = d / width;

		mirrorN = normalize2(p2{ -t.y, t.x });

		//visual
		vector<p2> positions;
		positions.reserve(2);
		positions.push_back(point1);
		positions.push_back(point2);
		body.addSet(positions);
	}

	void createPlanarSurface(p2 middlePoint_, p2 mirrorN_, float width_)
	{
		if (width_ <= 0.0f)
		{
			cout << "invalid width_ input in createPlanarSurface" << endl;
			return;
		}
		if (abs(magnitude2(mirrorN_)) < eps)
		{
			cout << "invalid mirrorN_ input in createPlanarSurface" << endl;
			return;
		}

		middlePoint = middlePoint_;
		mirrorN = normalize2(mirrorN_);
		width = width_;

		t = normalize2(p2{ mirrorN.y, -mirrorN.x });

		//endpoints for Lines2D from midpoint + t*halfWidth
		float halfW = 0.5f * width;
		p2 p0 = middlePoint - t * halfW;
		p2 p1 = middlePoint + t * halfW;

		//visual
		vector<p2> positions;
		positions.reserve(2);
		positions.push_back(p0);
		positions.push_back(p1);
		body.addSet(positions);
	}

	void draw()
	{
		body.draw();
	}
};

struct SphericalSurface
{
	Lines2D body;


	float r;
	p2 center;

	float theta0;
	float theta1;

	SphericalSurface()
	{

	}
	//Allows you to go cw or ccw
	void createSphericalSurface(float r_, p2 center_, float theta0_, float theta1_)
	{
		if (r_ < 0)
		{
			cout << "invalid r_ input in createSphericalSurface" << endl;
			return;
		}
		r = r_;
		center = center_;


		theta0 = radians(theta0_);
		theta1 = radians(theta1_);

		int segments = (int)r * 3; //arbitrary, only visual
		if (segments < 50) segments = 50;
		float angleSegment = 2 * PI / segments; //angle bewteen points in radians

		vector<p2> positions;

		positions.reserve(segments + 1);



		float dtheta = (theta1 - theta0) / segments;

		for (int i = 0; i <= segments; ++i)
		{
			float theta = theta0 + dtheta * i;
			positions.push_back({ center.x + r * cos(theta),center.y + r * sin(theta) });
		}

		body.addSet(positions);
	}
	//so any given angle in radians stays between [0,2PI)
	float wrapAngle(float a) const
	{
		while (a <= 0) a += 2.0f * PI;
		while (a > 2 * PI) a -= 2.0f * PI;
		return a;
	}

	//is theta between theta0 and theta1
	bool isAngleBetween(float theta, float theta0, float theta1) const
	{
		theta = wrapAngle(theta);
		theta0 = wrapAngle(theta0);
		theta1 = wrapAngle(theta1);

		if (theta0 <= theta1)
			return (theta >= theta0 && theta <= theta1);
		else
			return (theta >= theta0 || theta <= theta1);
	}
};

//Only rays paralel to the axis vertex-focal point end in the focal point
struct ParabolicSurface
{
	Lines2D body;

	p2 vertex; //lower point in the parabola
	p2 focus; //focal point
	float width;

	//Saving these here to not recalculate them in generate rays
	p2 axis; //from vertex to focal point
	float fLen; //focal length
	p2 v, u; //unit of axis and its perpendicular
	float halfAperture;

	ParabolicSurface()
	{

	}
	void createParabolicalSurface(p2 vertex_, p2 focus_, float width_)
	{

		//A vertical parabola follows the equation:
		// y = vertex.y+(x-vertex.x)^2/(4*fLength)

		if (width_ <= 0.0f)
		{
			cout << "invalid width_ input in createParabolicalSurface" << endl;
			return;
		}
		if (abs(magnitude2(focus_ - vertex)) < eps)
		{
			cout << "invalid width_ input in createParabolicalSurface" << endl;
			return;
		}

		vertex = vertex_;
		focus = focus_;
		width = width_;

		//from vertex to focus
		axis = focus - vertex; //from focal point to vertex
		fLen = magnitude2(axis);

		// unit axis
		v = axis / fLen;
		// perpendicular to v, sideway axis
		u = { -v.y, v.x };

		//visual segments
		int segments = (int)(width * 3.0f);
		if (segments < 50) segments = 50;

		vector<p2> positions;
		positions.reserve(segments + 1);


		//We lay points from one side of the mirror
		halfAperture = -0.5f * width;
		float sideStep = width / segments;
		for (int i = 0; i <= segments; ++i)
		{
			//lengths that multiply the locals u,v
			//u0, sideway distance
			float u0 = halfAperture + sideStep * i;
			//v0, forward distance, given by parabola equation
			float v0 = (u0 * u0) / (4.0f * fLen);

			//Parabola in our specific axis
			//point = vertex + u*u0 + v*v0. Lower point+ the x and y of this specific i step
			positions.push_back(vertex + u * u0 + v * v0);

		}

		body.addSet(positions);
	}
	void draw()
	{
		body.draw();
	}
};
