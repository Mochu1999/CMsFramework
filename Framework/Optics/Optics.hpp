#pragma once


#include "Common.hpp"
#include "OpticalSurfaces.hpp"


enum class SurfaceType { Plane, Sphere, Parabola };

struct SurfaceHandle
{

	SurfaceType type;
	size_t id;
	//Every surface must know where it does belong
	int lensId; // -1 if none
	SurfaceHandle(SurfaceType type_, size_t id_, int lensId_ = -1)
	: type(type_), id(id_), lensId(lensId_) 
	{
	}
};


struct Lens
{
	SurfaceHandle surface0;
	SurfaceHandle surface1;
	float ior; //index of refraction inside the lens

};

struct Optics
{
	//origin position and dir for ech ray determined by the created light
	vector<std::pair<p2, p2>> rayOrigin;

	//This is not very elegant, but the two light types are defined at the start, but in the constructor you develop only one
	LightPoint lightPoint;
	LightLine lightLine;


	Shader& shader2D;
	GlobalVariables& gv;

	Lines2D rays;



	//Using deque instead of vector because a vector of Lines2D won't break only if I don't reallocate OpenGL stuff from it with reserve
	//And so I use deque that works in block memories. But it does have more overhead
	deque<PlanarSurface> planarSurfaces;
	deque<ParabolicSurface> parabolicSurfaces;
	deque<SphericalSurface> sphericalSurfaces;


	//Just a SurfaceHandle for now. FOR THE LONG TERM MAKE A SPECIFIC MIRROR STRUCT
	deque<SurfaceHandle> mirrors;
	deque<Lens> lenses;

	Optics(Shader& shader2D_, GlobalVariables& gv_)
		:shader2D(shader2D_), gv(gv_), lightPoint(rayOrigin), lightLine(rayOrigin)
	{

		lightLine.createLightLine({ 100,350 }, { 100,650 }, { 1,0 });
		//lightPoint.createLightPoint({ 300,500 });
		//createParabolicMirror({ 100,500 }, { 300,500 }, 300);

		//createSphericalMirror(100, { 800,800 }, 90, 360);

		////createSphericalSurface(200, { 300,500 }, 270,450);

		/*createParabolicMirror({ 1900,500 }, { 700,500 }, 300);
		createPlanarMirror({ 700,500 }, { 1,1 }, 60);*/

		//SurfaceHandle s0 = createSphericalSurface(600, { 1100, 500 }, 160, 200);
		SurfaceHandle s0 = createParabolicSurface({ 800,500 }, { 900,500 }, 300);
		SurfaceHandle s1 = createPlanarSurface({ 1100,350 }, { 1100,650 });
		createLens(s0, s1, 1.5f);

		generateRays();
	}
	SurfaceHandle createParabolicSurface(p2 vertex_, p2 focus_, float width_)
	{
		parabolicSurfaces.emplace_back();
		parabolicSurfaces.back().createParabolicalSurface(vertex_, focus_, width_);

		return { SurfaceType::Parabola, parabolicSurfaces.size() - 1 };
	}
	SurfaceHandle createSphericalSurface(float r_, p2 center_, float theta0_, float theta1_)
	{
		sphericalSurfaces.emplace_back();
		sphericalSurfaces.back().createSphericalSurface(r_, center_, theta0_, theta1_);

		return { SurfaceType::Sphere, sphericalSurfaces.size() - 1 };
	}
	SurfaceHandle createPlanarSurface(p2 p1_, p2 p2_)
	{
		planarSurfaces.emplace_back();
		planarSurfaces.back().createPlanarSurface(p1_, p2_);

		return { SurfaceType::Plane, planarSurfaces.size() - 1 };
	}

	SurfaceHandle createPlanarSurface(p2 middle_, p2 normal_, float width_)
	{
		planarSurfaces.emplace_back();
		planarSurfaces.back().createPlanarSurface(middle_, normal_, width_);

		return { SurfaceType::Plane, planarSurfaces.size() - 1 };
	}
	void createParabolicMirror(p2 vertex_, p2 focus_, float width_)
	{
		SurfaceHandle h = createParabolicSurface(vertex_, focus_, width_);
		mirrors.push_back(h);
	}
	void createSphericalMirror(float r_, p2 center_, float theta0_, float theta1_)
	{
		SurfaceHandle h = createSphericalSurface(r_, center_, theta0_, theta1_);
		mirrors.push_back(h);
	}
	void createPlanarMirror(p2 p1_, p2 p2_)
	{
		SurfaceHandle h = createPlanarSurface(p1_, p2_);
		mirrors.push_back(h);
	}
	void createPlanarMirror(p2 middlePoint_, p2 mirrorN_, float width_)
	{
		SurfaceHandle h = createPlanarSurface(middlePoint_, mirrorN_, width_);
		mirrors.push_back(h);
	}

	void createLens(const SurfaceHandle& s0, const SurfaceHandle& s1, float ior_)
	{
		lenses.push_back({ s0, s1, ior_ });
	}


	void draw()
	{
		shader2D.bind();
		transparent();
		shader2D.setUniform("u_Model", identityMatrix);
		shader2D.setUniform("u_Color", 1.0f, 1.0f, 1.0f, 0.3f);
		rays.draw();

		shader2D.setUniform("u_Color", 0.0f, 0.0f, 1.0f, 1.0f);
		glLineWidth(4.0f);
		drawSurfaces();
		glLineWidth(1);

	}
	void drawSurfaces()
	{
		for (auto& mirror : mirrors)
		{
			drawSurface(mirror);
		}
		for (auto& lens : lenses)
		{
			drawSurface(lens.surface0);
			drawSurface(lens.surface1);
		}

	}
	void drawSurface(SurfaceHandle handle)
	{
		switch (handle.type)
		{
		case SurfaceType::Plane:
			planarSurfaces[handle.id].draw();
			break;

		case SurfaceType::Sphere:
			sphericalSurfaces[handle.id].body.draw();
			break;

		case SurfaceType::Parabola:
			parabolicSurfaces[handle.id].draw();
			break;
		}
	}
	//ALTERED FOR LENSES, INVALID FOR MIRRORS
	//In rayOrigin are set all rays that we will try to compute, they are computed here
	void generateRays()
	{
		const float Lmax = 1000000.0f;
		const int maxIntersections = 6;
		rays.clear();

		for (auto& ray : rayOrigin)
		{
			p2 origin = ray.first;
			p2 dir = ray.second;
			int intersections = 0;

			float currentIOR = 1.0f; // ray starts in air

			vector<p2> positions = { origin };

			while (intersections < maxIntersections)
			{
				p2 nextR; // intersection point of the ray, R(s)
				p2 nextN; //Surface normal on R

				size_t hitLensIdx = 0;
				if (!isThereAHit(origin, dir, nextR, nextN, hitLensIdx))
				{
					//no intersection with any surface,pushed the ray at infinity (won't show later if intersections was 0)
					positions.push_back(origin + dir * Lmax);
					break;
				}

				positions.push_back(nextR);
				intersections++;

				//MIRROR SPECIFIC
				//calculating rayDir with the normal of the surface in our point given by finalHit.n
				/*dir = dir - nextN * (2.0f * dot2(dir, nextN));
				dir = normalize2(dir);*/

				//LENS SPECIFIC
				float cosI = dot2(dir, nextN);
				bool entering = cosI < 0.0f;
				if (!entering)
					nextN = -nextN;
				float ior0 = currentIOR;
				float ior1 = entering ? lenses[hitLensIdx].ior : 1.0f;

				p2 newDir;
				bool refracted = refractSnell(dir, nextN, ior0, ior1, newDir);

				if (!refracted) // total internal reflection
				{
					newDir = dir - nextN * (2.0f * dot2(dir, nextN));
					newDir = normalize2(newDir);
				}
				else
				{
					currentIOR = ior1;
				}
				dir = newDir;


				// move origin slightly off surface
				origin = nextR + dir * eps;
			}

			if (intersections > 0 && intersections <= maxIntersections)
				rays.addSet(positions);
		}
	}

	//transmittedDir is our output, the direction the ray will have after entering or passing the lens
	//ior0: current medium (in textbooks appears as n)
	//ior1: next medium
	//ior0·sin(theta0) = ior1·sin(theta1)
	// , with theta0 the angle between incidentRay and surface normal and theta1 between transmittedRay and surfaceNormal
	//sin(theta0) = ior1/ior0·sin(theta1) = eta·sin(theta1)
	//sin^2(theta1)=eta^2·sin^2(theta0), substituing with sin^2+cos^2=1
	//cos^2(theta1)=1-eta^2(1-cos^2(theta0))
	bool refractSnell(p2& incidentDir, p2& n, float ior0, float ior1, p2& transmittedDir)
	{
		//a·b = |a||b|cos(theta), but as both are unit vectors
		// , it's just the cosine of the incident with respect to the surface (and pointing outward the surface instead of inward)
		float cosTheta0 = -dot2(incidentDir, n);
		cosTheta0 = clamp(cosTheta0, -1.0f, 1.0f);
		//refractivity coefficient
		float eta = ior0 / ior1;

		float sqrdCosTheta1 = 1.0f - eta * eta * (1.0f - cosTheta0 * cosTheta0);

		if (sqrdCosTheta1 < 0.0f)
			return false; // total internal reflection, when sin(theta1)>1

		//We want to get the transmitedDir T = Tt + Tn 
		//The normal component of incidentDir is In = -cos(theta0)·n
		// And so if I = It - cos(theta0)·n, It = I + cos(theta0)·n
		//And the total incidentDir are the sum of that normal and tangential components I=It+In
		//And by applying the snell law the tangential part of the transmited is eta by the tangential of the incident we get Tt = eta·It
		//So T = Tt + Tn = eta·It - cos(theta1)·n = eta(I + cos(theta0)·n) - cos(theta1)·n
		//where cos(theta1) is the square root of what we just got
		transmittedDir = normalize2(incidentDir * eta + n * (eta * cosTheta0 - sqrt(sqrdCosTheta1)));

		return true;
	}



	//Will check for intersections with every surface and will return if isHitted and if so, finalHit will contain the data of the closest one
	bool isThereAHit(const p2& rayOrigin, const p2& rayDir, p2& nextR, p2& nextN, size_t& hitLensIdx)
	{
		bool isHitted = false;
		float closestS = std::numeric_limits<float>::max();

		//MIRRORS AND LENSES CAN'T BE TOGETHER YET
		/*for (const SurfaceHandle& mirror : mirrors)
		{
			p2 R;
			float s;
			p2 n;

			if (intersectMirror(mirror, rayOrigin, rayDir, R, s, n) && s < closestS)
			{
				closestS = s;
				nextR = R;
				nextN = n;
				isHitted = true;
			}
		}*/
		for (size_t i = 0; i < lenses.size(); ++i)
		{
			const Lens& lens = lenses[i];

			for (const SurfaceHandle& lensHandle : { lens.surface0, lens.surface1 })
			{
				p2 R;
				float s;
				p2 n;

				if (intersectMirror(lensHandle, rayOrigin, rayDir, R, s, n) && s < closestS)
				{
					closestS = s;
					nextR = R;
					nextN = n;

					hitLensIdx = i;

					isHitted = true;
				}
			}
		}
		return isHitted;
	}

	bool intersectMirror(const SurfaceHandle& m, const p2& rayOrigin, const p2& rayDir, p2& R, float& s, p2& n)
	{
		switch (m.type)
		{
		case SurfaceType::Parabola:
			return intersectParabola(parabolicSurfaces[m.id], rayOrigin, rayDir, R, s, n);

		case SurfaceType::Sphere:
			return intersectSphere(sphericalSurfaces[m.id], rayOrigin, rayDir, R, s, n);

		case SurfaceType::Plane:
			return intersectPlane(planarSurfaces[m.id], rayOrigin, rayDir, R, s, n);
		}
		return false;
	}

	//tries to intersect a parabola with the current hit data
	bool intersectParabola(const ParabolicSurface& mirror, const p2& rayOrigin, const p2& rayDir, p2& R, float& s, p2& n)
	{
		const p2 vertex = mirror.vertex;
		const float fLen = mirror.fLen;
		const p2 v = mirror.v;
		const p2 u = mirror.u;
		const float halfAperture = -mirror.halfAperture; //positive value

		p2 lightLensDir = rayOrigin - vertex;

		//The parabola equation we are trying to solve is y = vertex.y+(x-vertex.x)^2/(4*fLen)
		//using X = x-vertex.x, Y = y-vertex; we get to X^2 - Y*f*fLen = 0
		//But local coordinates must be used as the parabola axis isn't always {0,1}

		//We are trying to git the distance s in: rayPosition(s) = rayOrigin + s * rayDir
		//When we created the graphic parabola, we knew u0,v0; and wanted to know the world point
		// now we now one world point, given by rayOrigin, and we want to know how far sideways and forward
		// are they from the vertex, that's what u0 and v0 are here.
		// Using lightLensDir, with the subtracted vertex, means we have the vertex centered
		//with the dot product, we get how far the origin is from the vertex in local coordinates (perp to axis)
		//origin = vertex+ u*u0+v*v0; being u and v unit vectors
		float u0 = dot2(lightLensDir, u);
		float v0 = dot2(lightLensDir, v);
		//we also have that rayDir=u·du+v·dv. Just our already unit rayDir expressed in locals
		float du = dot2(rayDir, u);
		float dv = dot2(rayDir, v);
		//du^2+dv^2 = 1

		//u = u0 + du·s, v = v0 + dv·s
		// v = u^2/(4·fLen)
		//(u0 + du·s)^2 = (v0 + dv·s)·4·fLen  //and we develop the equation
		// uo^2 + du^2·s^2 + 2·u0·du·s - 4·fLen·v0 -4·dv·s·fLen
		// Then we solve for s, s^2·A+S·B+C=0
		// s^2· (du^2) + s· (2·u0·du - 4·fLen·dv) + (u0^2-4·fLen·v0) = 0
		float A = du * du;
		float B = 2.0 * u0 * du - 4.0 * fLen * dv;
		float C = u0 * u0 - 4.0 * fLen * v0;

		bool isHitted = false;
		//u and v are the unit vectors of the parabola, uDist and vDist are how sideway and forward the ray displaces from the vertex
		float uDist = 0.0;
		float vDist = 0.0;
		//In spherics we tried to solve for the s distance, here we will also solve s, but inmediately transform it into locals uDist and vDist
		float bestS = 0.0f;
		//When A= 0, the system becomes linear: B*s + C = 0
		if (fabs(A) < eps)
		{
			//if B is also 0, then it exists this and does nothing
			if (fabs(B) > eps)
			{
				float s = -C / B;
				float uDistTest = u0 + du * s;
				// < than 0 would be locally behind the ray, 10e-3 because we push off the ray that little after intersection
				//u <= halfAperture or it would think the parabola is infinite
				if (s > 10e-3 && fabs(uDistTest) <= halfAperture)
				{
					uDist = uDistTest; isHitted = true; bestS = s;
				}
			}
		}
		else
		{
			float s0, s1;
			stableQuadraticSolver(A, B, C, s0, s1);

			//it is standard in ray tracing to check for the smaller first
			//It ensures that the first intersection is found first
			if (s0 > s1) swap(s0, s1);

			//tests if the closest finalHit is further than our pushoff and closer than halfAperture
			// If so it is the point
			for (float s : { s0, s1 })
			{
				if (s <= 10e-3) continue;
				float uDistTest = u0 + du * s;
				if (fabs(uDistTest) <= halfAperture)
				{
					uDist = uDistTest; isHitted = true; bestS = s;
					break;
				}
			}

		}

		if (!isHitted)
			return false;

		// vDist from the parabola formula
		vDist = (uDist * uDist) / (4.0f * fLen);

		//point in the parabola
		R = vertex + u * uDist + v * vDist;

		// gradient grad(u^2 - 4f·v) = (2u, -4f)
		p2 nLocal = { (2.0f * uDist), (-4.0f * fLen) };
		//and then to globals, this is the normal of the surface at our point, not rayDir
		n = normalize2(u * nLocal.x + v * nLocal.y);


		s = bestS;

		return true;
	}




	//tries to intersect a spherical mirror with the current hit data
	bool intersectSphere(const SphericalSurface& mirror, const p2& rayOrigin, const p2& rayDir, p2& R, float& s, p2& n)
	{
		const p2 lensCenter = mirror.center;
		const float lensR = mirror.r;
		const float theta0 = mirror.theta0;
		const float theta1 = mirror.theta1;

		//vector from center to ray origin
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

		float A = dot2(rayDir, rayDir); // cos^2i+sin^2i should be one, but it actually isn't due to floating point error
		float B = 2.0f * dot2(rayDir, lightLensDir);
		float C = dot2(lightLensDir, lightLensDir) - lensR * lensR;

		float s0, s1;
		stableQuadraticSolver(A, B, C, s0, s1);




		if (s0 > s1) swap(s0, s1);

		bool isHitted = false;
		float bestS = 0.0f;
		//R(s) evaluated at the intersection point s
		R;

		for (float s : { s0, s1 })
		{
			if (s <= 10e-3f) continue; //setting it too low actually provokes errors of self intersetion

			// possible R(s)
			p2 Rcandidate = rayOrigin + rayDir * s;

			// If Rcandidate, which is in the circunference, is in an angle in between that of our arc, then it's valid
			float theta = atan2(Rcandidate.y - lensCenter.y, Rcandidate.x - lensCenter.x);

			if (mirror.isAngleBetween(theta, theta0, theta1))
			{
				R = Rcandidate;
				bestS = s;
				isHitted = true;
				break;
			}
		}

		if (!isHitted)
			return false;

		// surface normal pointing the correct direction, independent from rayDir
		n = normalize2(R - lensCenter);

		s = bestS;

		return true;
	}

	//tries to intersect a planar mirror with the current hit data
	bool intersectPlane(const PlanarSurface& mirror, const p2& rayOrigin, const p2& rayDir, p2& R, float& s, p2& n)
	{
		const p2 middlePoint = mirror.middlePoint;
		const p2 mirrorN = mirror.mirrorN;
		const p2 t = mirror.t;
		const float halfWidth = 0.5f * mirror.width;

		//Plane equation:
		// dot2((X - middlePoint), mirrorN) = 0
		// Where X is whatever point in space. The logic is that the 2 orthogonal vectors dot product will be 0
		// So when X belongs to the plane, only then that condition will be 0

		//Ray follows R(s) = rayOrigin + s * rayDir
		// R will be our X, the point that we are checking to be in the plane

		//Substituting R into plane equation:
		// dot((rayOrigin + s*rayDir - middlePoint), mirrorN) = 0

		//But we need to solve for s, so we are taking it out using the principles: 
		// dot(a+b,n)=dot(a,n)+dot(b,n); thus separating the rayOrigin part into it's single dot
		// dot(s·a,n)=sdot(a,n); being s an scalar, and thus separating it to:
		// dot(rayOrigin - middlePoint, mirrorN) + s * dot(rayDir, mirrorN) = 0
		// which solving for s turns into:
		// s = −dot(rayOrigin−middlePoint, mirrorN)/dot(rayDir, mirrorN)

		float denom = dot2(rayDir, mirrorN);

		// denom == 0 is ray parallel to plane, we are ignoring it
		if (fabs(denom) < eps)
			return false;

		s = -dot2(rayOrigin - middlePoint, mirrorN) / denom;

		// rejecting intersections behind ray origin
		if (s <= 10e-3f)
			return false;

		R = rayOrigin + rayDir * s;

		// Now we check if R lies within the finite segment width
		// Project (R - middlePoint) onto tangent direction
		float tDist = dot2(R - middlePoint, t);

		if (fabs(tDist) > halfWidth)
			return false;

		// mirrorN pointing to the correct place, independent from rayDir
		n = mirrorN;



		return true;
	}
};



