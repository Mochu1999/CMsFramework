#pragma once

//common file that does not depend on other files
//importing Common everywhere might be slow. Look for pre compiled headers and alternatives to Common

using namespace std;

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <cmath>
#include <numeric> 
#include <algorithm>

#include <array>
#include <vector>
#include <unordered_set>
#include <set>
#include <unordered_map>
#include <map>
#include <list>
#include <deque>

#include <functional>
#include <random>

//not templated definitions of functions need to be separated from their declarations in .cpp

// Constants
constexpr double PI = 3.14159265358979323846;
constexpr double halfPI = PI / 2;
constexpr double invPI = 1.0f / PI;

constexpr float inv3 = 1.0f / 3.0f;
constexpr float inv180 = 1.0f / 180.0f;

/////////////////////////////global variables are after vec3

template<typename T>
struct vec2 {
	T x, y;

	vec2() : x(0), y(0) {} //initializing paramenters to 0 if not specified 

	vec2(T x, T y) : x(x), y(y) {} //normal constructor

	// Operator overloads for comparison
	bool operator == (const vec2& other) const {
		return x == other.x && y == other.y;
	}

	bool operator!=(const vec2& other) const {
		return !(*this == other);
	}

	// Operator overloads for arithmetic
	vec2 operator + (const vec2& other) const {
		return { x + other.x, y + other.y };
	}


	vec2 operator + (const T other) const { //PASSING WITHOUT REFERENCE MIGHT BE LESS EXPENSIVE, CAN YOU CHECK?
		return { x + other, y + other };
	}

	vec2 operator - (const vec2& other) const {
		return { x - other.x, y - other.y };
	}

	vec2 operator *(T scalar) const {
		return { x * scalar, y * scalar };
	}

	vec2 operator *(vec2<T> other) const {
		return { x * other.x, y * other.y };
	}

	vec2 operator /(T scalar) const {
		return { x / scalar, y / scalar };
	}

	// Compound assignment operators
	vec2& operator += (const vec2& other) {
		x += other.x;
		y += other.y;
		return *this;
	}

	vec2& operator -= (const vec2& other) {
		x -= other.x;
		y -= other.y;
		return *this;
	}

	vec2& operator *= (T scalar) {
		x *= scalar;
		y *= scalar;
		return *this;
	}

	vec2& operator /= (T scalar) {
		x /= scalar;
		y /= scalar;
		return *this;
	}
	bool operator < (const vec2& other) const {
		if (x == other.x) return y < other.y;
		return x < other.x;
	}
};



using p2 = vec2<float>;
using ui2 = vec2<unsigned int>;
using matrix4x4 = std::array<float, 16>;

//needed for the unordered_maps that take p as keys, it could be inside vec2, but this is more idiomatic
//umaps have average looking times of O(1), compared with O(logn) of map
struct p_hash {
	std::size_t operator()(const p2& point) const {
		return std::hash<float>()(point.x) ^ (std::hash<float>()(point.y) << 1);
	}
};


//there are a lot of hash functions, 
struct p_HashMultiplicative {
	std::size_t operator()(const p2& point) const {
		constexpr std::uint64_t k = 0x9ddfea08eb382d69ULL;
		std::uint64_t a, b;

		std::memcpy(&a, &point.x, sizeof(std::uint64_t));
		std::memcpy(&b, &point.y, sizeof(std::uint64_t));

		return (a * k) ^ (b * k);
	}
};

struct pair_hash_multiplicative {
	std::size_t operator()(const std::pair<p2, p2>& edge) const {
		p_HashMultiplicative ph;
		std::size_t h1 = ph(edge.first);
		std::size_t h2 = ph(edge.second);

		// Combine the two hashes
		return h1 ^ (h2 * 0x9e3779b97f4a7c15ULL); // Using another large constant
	}
};

//sum of products, is also equal to v1*v2*cos(theta)
template<typename T>
T dot2(const vec2<T>& v1, const vec2<T>& v2) {
	return v1.x * v2.x + v1.y * v2.y;
}

//In 2d geometry the cross product is a scalar value, not a vector, it represents the area of their parallelogram
template<typename T>
T cross2(const vec2<T>& v1, const vec2<T>& v2) {
	return v1.x * v2.y - v1.y * v2.x;
}

template<typename T>
float magnitude2(const vec2<T>& v) { //modulus
	float magnitudeSquared = v.x * v.x + v.y * v.y;
	return sqrt(magnitudeSquared);
}



template<typename T>
struct vec3 {
	T x, y, z;

	vec3() : x(0), y(0), z(0) {} //initializing paramenters to 0 if not specified 

	vec3(T x_, T y_, T z_) : x(x_), y(y_), z(z_) {} //normal constructor

	// operator overloads
	bool operator == (const vec3& other) const {
		return x == other.x && y == other.y && z == other.z;
	}

	bool operator!=(const vec3& other) const {
		return !(*this == other);
	}

	vec3 operator + (const vec3& other) const {
		return { x + other.x, y + other.y, z + other.z };
	}

	vec3 operator - (const vec3& other) const {
		return { x - other.x, y - other.y, z - other.z };
	}

	vec3 operator-() const {
		return { -x, -y, -z };
	}

	vec3 operator *(T scalar) const {
		return { x * scalar, y * scalar, z * scalar };
	}

	vec3 operator /(T scalar) const {
		return { x / scalar, y / scalar, z / scalar };
	}

	vec3& operator += (const vec3& other) {
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}

	vec3& operator += (T scalar) {
		x += scalar;
		y += scalar;
		z += scalar;
		return *this;
	}

	vec3& operator -= (const vec3& other) {
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}
	vec3& operator *= (T scalar) {
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}
	vec3& operator /= (T scalar) {
		x /= scalar;
		y /= scalar;
		z /= scalar;
		return *this;
	}
};

using p3 = vec3<float>;
using ui3 = vec3<unsigned int>;


//these must be out of vec3 to be accesible without the need of calling them like variable.cross()

//sum of products
template<typename T>
T dot3(const vec3<T>& v1, const vec3<T>& v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

template<typename T>
vec3<T> cross3(const vec3<T>& v1, const vec3<T>& v2) {
	return vec3<T>(
		v1.y * v2.z - v1.z * v2.y,
		v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y * v2.x
	);
}

float fastInverseSqrt(float number);

template<typename T>
vec3<T> normalize3(const vec3<T>& v) {
	T magnitudeSquared = v.x * v.x + v.y * v.y + v.z * v.z;
	if (magnitudeSquared == 0.0f) {
		return vec3<T>(0, 0, 0); // Return a zero vector to avoid division by zero
	}
	float magnitude_inv = fastInverseSqrt(magnitudeSquared);
	return vec3<T>(v.x * magnitude_inv, v.y * magnitude_inv, v.z * magnitude_inv);
}

template<typename T>
float magnitude3(const vec3<T>& v) { //modulus
	float magnitudeSquared = v.x * v.x + v.y * v.y + v.z * v.z;
	return sqrt(magnitudeSquared);
}








// Mover a gv
extern float windowHeight;
extern float windowWidth;

enum Programs { telemetry, MRS, solar, openCascade };
enum CameraModes { drag, FPS, centered };
enum VisualizationMode { wire, triangulated }; //OCC
enum MrsMode { mapMRS, mapCustom }; //1 MRS map, 2 custom map
enum CadMode { none, polyline, rectangle, circle, sphere, extrusion};

/////////////////////////////////////////////
struct GlobalVariables
{



	Programs program = telemetry;//to be changed in settings
	CameraModes cameraMode = drag;
	bool isRunning = true;
	p2 mPos = { 0,0 };

	bool isLmbPressed = 0;
	bool isMmbPressed = 0;
	p2 LastLMPos = { 0,0 };
	p2 LastMMPos = { 0,0 };
	p3 totalMiddleMPosVariation;
	p3 accumulativePositionChange; //LastMMPos measures mpos change, this is traspased to 3d with forward and right operations made on it

	p2 centerWindow;
	MrsMode mrsMode = mapMRS;
	matrix4x4 identityMatrix = { 1, 0, 0, 0, 0, 1, 0, 0,0, 0, 1, 0,0, 0, 0, 1 };
	matrix4x4 modelMatrixOCC = identityMatrix;

	VisualizationMode visualizationMode = triangulated;
	CadMode cadMode = none;
	vector <p3> cadPositions = { {0,0,0} };

	GlobalVariables()
	{
		centerWindow = { windowWidth / 2,windowHeight / 2 };
	}


};






void getCursorPos(GLFWwindow* window);


// General print template for all types
template<typename T>
void print_(const string& name, const T& item) {
	stringstream ss;
	ss << name << ": " << item;
	cout << ss.str() << endl << endl;
}

// Overload for vec2<T>
template<typename T>
void print_(const string& name, const vec2<T>& items) {
	stringstream ss;
	ss << name << ": ";
	ss << "{" << items.x << "," << items.y << "}";
	cout << ss.str() << endl << endl;
}



//vector<vec2<T>>
template<typename T>
void print_(const string& name, const vector<vec2<T>>& items) {
	stringstream ss;
	ss << name << ": {";
	for (size_t i = 0; i < items.size(); ++i) {
		ss << "{" << items[i].x << "," << items[i].y << "}";

		if (i != items.size() - 1) { // Check if it's not the last element
			ss << ",";
		}
	}
	cout << ss.str() << "}" << endl << endl;
}

//vec3<T>
template<typename T>
void print_(const string& name, const vec3<T>& items) {
	stringstream ss;
	ss << name << ": ";
	ss << "{" << items.x << "," << items.y << "," << items.z << "}";

	cout << ss.str() << endl << endl;
}


//vector<vec3<T>>
template<typename T>
void print_(const string& name, const vector<vec3<T>>& items) {
	stringstream ss;
	ss << name << ": {";
	for (size_t i = 0; i < items.size(); ++i) {
		ss << "{" << items[i].x << "," << items[i].y << "," << items[i].z << "}";

		if (i != items.size() - 1) { // Check if it's not the last element
			ss << ",";
		}
	}
	cout << ss.str() << "}" << endl << endl;
}

//vector<T>
template<typename T>
void print_(const string& name, const vector<T>& items) {
	stringstream ss;
	ss << name << ": ";
	ss << "{";
	for (size_t i = 0; i < items.size(); i++) {
		ss << items[i];

		if (i != items.size() - 1) { // Check if it's not the last element
			ss << ",";
		}
	}
	ss << "}";
	cout << ss.str() << endl << endl;
}


//matrix4x4
void print_(const std::string& name, const std::array<float, 16>& matrix);

#define print(var) print_(#var, var)





//Borrar
//aun con p chatgpt sigue diciendo que referencia podría no ser lo mejor. También sigue diciendo que un valor positivo es que p2 está a la derecha
template<typename T>
float crossProduct(const vec2<T>& p0, const vec2<T>& p1, const vec2<T>& p2) {
	vec2<T> v01 = { p1.x - p0.x, p1.y - p0.y };
	vec2<T> v12 = { p2.x - p1.x, p2.y - p1.y };

	return v01.x * v12.y - v01.y * v12.x;
}











//consts, cambiale el nombre y deja el nombre "is" para bools
float isRightOfLine(p2& A, p2& B, p2& P);



std::array<float, 4> createQuaternion(float angle, const p3& axis);

std::array<float, 4> inverseQuaternion(const std::array<float, 4>& q);

std::array<float, 4> multiplyQuaternions(const std::array<float, 4>& a, const std::array<float, 4>& b);

// q * p * q^-1
void rotatePoint(p3& point, const float& angle, const p3& axis);

void rotate3D(std::vector<p3>& vertices, const p3& centroid, float angleX, float angleY, float angleZ);
void rotate3D(std::vector<p3>& vertices, float angleX, float angleY, float angleZ);





float isBelowTriangle(const p3& a, const p3& b, const p3& c, const p3& p);



template<typename T>
vec3<T> centroid(const vec3<T>& p1, const vec3<T>& p2, const vec3<T>& p3) {
	return (p1 + p2 + p3) * inv3;
}

template<typename T>
vec3<T> normal(const vec3<T>& p1, const vec3<T>& p2, const vec3<T>& p3) {
	return normalize3(cross3(p2 - p1, p3 - p1));
}


std::array<float, 16> multiplyMatrices(const std::array<float, 16>& a, const std::array<float, 16>& b);
matrix4x4 invertMatrix(const matrix4x4& m);

std::array<float, 4> multiplyMatVec(const std::array<float, 16>& m, const std::array<float, 4>& v);

// Normalizes the quaternion [w, x, y, z] in-place
void normalizeQuaternion(std::array<float, 4>& q);


float radians(float input);


float degrees(float input);
