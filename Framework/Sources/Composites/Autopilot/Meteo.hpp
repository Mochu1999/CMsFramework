#pragma once

#include "Common.hpp"
#include "Icons.hpp"



struct Meteo
{
	Shader& shader2D;
	Shader& shader2D_Instanced;
	Camera& camera;
	TimeStruct& tm;

	struct MerraDataPoint {
		p2 lonLat;
		float pgenTot, rhoA, speed, speedMax;
		float tlml, tsh, tstar, ulml, ustar, vlml;
	};

	//Int is the timekey, for each time you get a vector of points
	std::unordered_map<int, std::vector<MerraDataPoint>> allProcessedData;

	int currentTimeKey = 240030;
	unsigned int lastCounterUpdateMRS = 0;

	vector<p2> currentLonLats;
	vector<InstanceAttributes> currentInstancing;
	ArrowsMRS arrows;
	std::string currentTimeString;

	Meteo(Shader& shader2D_, Shader& shader2D_Instanced_, Camera& camera_, TimeStruct& tm_)
		:shader2D(shader2D_), shader2D_Instanced(shader2D_Instanced_), camera(camera_), arrows(shader2D, shader2D_Instanced, camera),tm(tm_)
	{
		readCSV();

		//printHourData(10030);
		extractData(currentTimeKey);
		//compareSpeedFromComponents(10030);
	}



	void draw(std::array<float, 16>& mapModel2DMatrix)
	{

		arrows.draw(mapModel2DMatrix);
	}

	void readCSV() {
		std::ifstream file("Resources/MRS/merra202405.csv");
		if (!file.is_open()) {
			std::cerr << "Failed to open file\n";
			return;
		}

		std::string line;
		std::getline(file, line); // skip header

		while (std::getline(file, line)) {
			std::stringstream ss(line);
			std::string timeStr;
			std::getline(ss, timeStr, ',');

			int day = std::stoi(timeStr.substr(8, 2));
			int hour = std::stoi(timeStr.substr(11, 2));
			int minute = std::stoi(timeStr.substr(14, 2));
			int timeKey = day * 10000 + hour * 100 + minute;

			MerraDataPoint point;
			float lat, lon;
			ss >> lat; ss.ignore();
			ss >> lon; ss.ignore();
			point.lonLat = lonLatToMercator({ lon, lat });
			ss >> point.pgenTot; ss.ignore();
			ss >> point.rhoA;    ss.ignore();
			ss >> point.speed;   ss.ignore();
			ss >> point.speedMax; ss.ignore();
			ss >> point.tlml;    ss.ignore();
			ss >> point.tsh;     ss.ignore();
			ss >> point.tstar;   ss.ignore();
			ss >> point.ulml;    ss.ignore();
			ss >> point.ustar;   ss.ignore();
			ss >> point.vlml;

			allProcessedData[timeKey].push_back(point);
		}

		file.close();
	}

	void extractData(int timeKey) {
		currentLonLats.clear();
		currentInstancing.clear();

		if (!allProcessedData.count(timeKey)) {
			std::cout << "No data for key " << timeKey << '\n';
			return;
		}

		const auto& points = allProcessedData[timeKey];
		for (const auto& point : points) {
			p2 windVec = { point.ulml, point.vlml };
			float angle = atan2(windVec.y, windVec.x);

			currentLonLats.push_back(point.lonLat);
			currentInstancing.push_back({ point.lonLat, angle, {5000 * point.speed, 5000 * point.speed} });
		}
		arrows.update(currentInstancing);

	}

	//Hay discrepancias tochas. Enseñar a Juanan
	void compareSpeedFromComponents(int timeKey) {
		if (!allProcessedData.count(timeKey)) {
			std::cout << "No data for key " << timeKey << '\n';
			return;
		}

		const auto& points = allProcessedData[timeKey];
		float maxDiff = 0.0f;
		float sumDiff = 0.0f;

		for (size_t i = 0; i < points.size(); ++i) {
			const auto& point = points[i];
			float computedSpeed = std::sqrt(point.ulml * point.ulml + point.vlml * point.vlml);
			float diff = std::abs(computedSpeed - point.speed);

			std::cout << "Point " << i
				<< ": SPEED = " << point.speed
				<< ", Computed = " << computedSpeed
				<< ", Diff = " << diff << '\n';

			sumDiff += diff;
			if (diff > maxDiff) maxDiff = diff;
		}

		std::cout << "Max diff: " << maxDiff << '\n';
		std::cout << "Average diff: " << (points.empty() ? 0.0f : sumDiff / points.size()) << '\n';
	}


	void printHourData(int timeKey) {
		if (!allProcessedData.count(timeKey)) {
			std::cout << "No data for key " << timeKey << '\n';
			return;
		}

		std::cout << "TimeKey: " << timeKey << '\n';
		for (const auto& point : allProcessedData[timeKey]) {
			std::cout << "  Lat: " << point.lonLat.y
				<< ", Lon: " << point.lonLat.x
				<< ", PGENTOT: " << point.pgenTot
				<< ", RHOA: " << point.rhoA
				<< ", SPEED: " << point.speed
				<< ", SPEEDMAX: " << point.speedMax
				<< ", TLML: " << point.tlml
				<< ", TSH: " << point.tsh
				<< ", TSTAR: " << point.tstar
				<< ", ULML: " << point.ulml
				<< ", USTAR: " << point.ustar
				<< ", VLML: " << point.vlml
				<< '\n';
		}
	}
	
	float getCurrentSpeed() {
		if (!allProcessedData.count(currentTimeKey) || allProcessedData[currentTimeKey].empty())
			return 0.0f;

		return allProcessedData[currentTimeKey][0].speed;
	}
};