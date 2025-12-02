#pragma once

struct Buoy
{
	Polyhedra body;
	Polyhedra support;
	Polyhedra alternator;

	vector<p3> oPositions;
	vector<p3> positions;

	float x = 0;
	float y = -0.73;
	float vx = 0;
	float vy = 0;
	float ax = 0;
	float ay = 0;

	float theta = radians(0);// rad
	float omega = 0;// rad/s
	float alpha = 0;// rad/s^2

	float lambda = 1;

	//EXCEL, A CAMBIAR
	float draft = 80;
	float m = 585606; //kg //support + structure

	float A33; //added mass
	float C33; //restoring coefficient

	float naturalFrequency, waveFrequency; //rad/s
	float period, wavePeriod; //s
	float k;//1/m

	float criticalDamping, damping;
	float Ffk;
	float RAO;

	float waveHeight = 2, waveAmplitude = waveHeight / 2; //m

	p3 oCconnectionPoint = { 0,-5,0 };
	p3 connectionPoint = { 0,-5,0 };

	Buoy()
	{
		/*Polyhedra stl;
		readSTL(stl, "Alternator.stl");
		writeSimplePolyhedra(stl, "Alternator.bin");

		readSTL(stl, "Buoy.stl");
		writeSimplePolyhedra(stl, "Buoy.bin");

		readSTL(stl, "Support.stl");
		writeSimplePolyhedra(stl, "Support.bin");*/

		body.addPolyhedra("Cube.bin");
		oPositions = body.positions;
		updatePositions();

		alternator.addPolyhedra("Alternator.bin");
		support.addPolyhedra("Support.bin");

		//excel();

	}

	void updatePositions()
	{
		positions = oPositions; //reseting from reference
		

		std::array<float, 4> q = createQuaternion(theta, { 0,0,1 });
		std::array<float, 4> qInv = inverseQuaternion(q);

		for (auto& p : positions)
		{
			// rotate around Z
			std::array<float, 4> pQuat = { 0, p.x, p.y, p.z };
			auto rotated = multiplyQuaternions(multiplyQuaternions(q, pQuat), qInv);

			// update and apply translation
			p.x = rotated[1] + x;
			p.y = rotated[2] + y;
			p.z = rotated[3] + 0;
		}
		{//connection point
			connectionPoint = oCconnectionPoint;
			// rotate around Z
			std::array<float, 4> pQuat = { 0, connectionPoint.x, connectionPoint.y, connectionPoint.z };
			auto rotated = multiplyQuaternions(multiplyQuaternions(q, pQuat), qInv);

			// update and apply translation
			connectionPoint.x = rotated[1] + x;
			connectionPoint.y = rotated[2] + y;
			connectionPoint.z = rotated[3] + 0;
		}
		body.positions=positions;
		body.isBufferUpdated = true;
		
	}

	
	//void excel()
	//{
	//	float D = 20, T = 80;
	//	p3 cog = { 0,0,0 };

	//	waveFrequency = 1.5;

	//	float area = PI * D * D / 4;

	//	m = area * T * rho;

	//	C33 = rho * g * area;

	//	A33 = (2.0 / 3 * PI * D * D * D / 8) * rho; //Volume of the semi sphere

	//	naturalFrequency = pow(C33 / (m + A33), 0.5);

	//	period = 2 * PI / naturalFrequency;

	//	criticalDamping = 2 * pow((m + A33) * C33, 0.5);
	//	damping = criticalDamping * 0.05;

	//	wavePeriod = 2.0 * PI / waveFrequency;
	//	//print(wavePeriod);
	//	k = waveFrequency * waveFrequency / g;
	//	//print(waveFrequency);


	//	Ffk = C33 * waveAmplitude * exp(-k * draft);
	//	//print(Ffk);

	//	RAO = Ffk / waveAmplitude / (-waveFrequency * waveFrequency * (m + A33) + C33);
	//	//print(RAO);

	//	RAO = Ffk / waveAmplitude / pow(pow((-waveFrequency * waveFrequency * (m + A33) + C33), 2) + pow(waveFrequency * damping, 2), 0.5);
	//	//print(RAO);
	//}
	void printOffshore()
	{
		print(A33);
		print(C33);
		print(naturalFrequency);
		print(period);
	}
};