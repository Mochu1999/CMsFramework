#include "Common.hpp"



//Variables
p2 cursor = { 0, 0 };
float windowHeight = 1080;
float windowWidth = 1920;



float fastInverseSqrt(float number) {
	long i;
	float x2, y;
	x2 = number * 0.5F;
	y = number;
	std::memcpy(&i, &y, sizeof(i)); // Safer type-punning
	i = 0x5f3759df - (i >> 1);
	std::memcpy(&y, &i, sizeof(y)); // Safer type-punning
	y = y * (1.5f - (x2 * y * y));
	return y;
}

void getCursorPos(GLFWwindow* window) {

	double intermX, intermY;

	glfwGetCursorPos(window, &intermX, &intermY);

	cursor = { static_cast<float>(intermX), static_cast<float>(windowHeight - intermY) };

}

float isRightOfLine(p2& A, p2& B, p2& P) {		 //is P to the right of AB?
	p2 AB = { B.x - A.x,  B.y - A.y };
	p2 AP = { P.x - A.x, P.y - A.y };

	return AB.x * AP.y - AB.y * AP.x;	//if negative it is to its right, if 0, P is on the infinite line of AB
}

std::array<float, 4> createQuaternion(float angle, const p3& axis) {
	std::array<float, 4> q;
	float s = sin(angle / 2);
	q[0] = cos(angle / 2); // w
	q[1] = axis.x * s; // x
	q[2] = axis.y * s; // y
	q[3] = axis.z * s; // z
	return q;
}

std::array<float, 4> inverseQuaternion(const std::array<float, 4>& q) {
	return { q[0], -q[1], -q[2], -q[3] }; // q^-1 = [w, -x, -y, -z] for a unit quaternion
}

//Quaternion multiplication is non-commutative (order matters)
std::array<float, 4> multiplyQuaternions(const std::array<float, 4>& a, const std::array<float, 4>& b) {
	std::array<float, 4> result;
	result[0] = a[0] * b[0] - a[1] * b[1] - a[2] * b[2] - a[3] * b[3]; // w
	result[1] = a[0] * b[1] + a[1] * b[0] + a[2] * b[3] - a[3] * b[2]; // x
	result[2] = a[0] * b[2] - a[1] * b[3] + a[2] * b[0] + a[3] * b[1]; // y
	result[3] = a[0] * b[3] + a[1] * b[2] - a[2] * b[1] + a[3] * b[0]; // z
	return result;
}

// q * p * q^-1
void rotatePoint(p3& point, const float& angle, const p3& axis) {
	std::array<float, 4> rotationQuaternion = createQuaternion(angle, axis);
	//p
	std::array<float, 4> pointQuat = { 0, point.x, point.y, point.z };
	std::array<float, 4> inverseQuat = inverseQuaternion(rotationQuaternion);


	std::array<float, 4> rotatedPointQuat = multiplyQuaternions(multiplyQuaternions(rotationQuaternion, pointQuat), inverseQuat);

	point = { rotatedPointQuat[1], rotatedPointQuat[2], rotatedPointQuat[3] };
}


void rotate3D(std::vector<p3>& vertices, const p3& centroid, float angleX, float angleY, float angleZ) {

	for (p3& vertex : vertices)
	{
		vertex -= centroid;
		rotatePoint(vertex, radians(angleX), { 1, 0, 0 });
		rotatePoint(vertex, radians(angleY), { 0, 1, 0 });
		rotatePoint(vertex, radians(angleZ), { 0, 0, 1 });
		vertex += centroid;
	}

}

void rotate3D(std::vector<p3>& vertices, float angleX, float angleY, float angleZ) {

	for (p3& vertex : vertices)
	{
		rotatePoint(vertex, radians(angleX), { 1, 0, 0 });
		rotatePoint(vertex, radians(angleY), { 0, 1, 0 });
		rotatePoint(vertex, radians(angleZ), { 0, 0, 1 });
	}

}



float isBelowTriangle(const p3& a, const p3& b, const p3& c, const p3& p) {

	//equivalent to the signed volume of a tetrahedron without the /6.0f
	return dot3(cross3(b - a, c - a), p - a);

}


void print_(const std::string& name, const std::array<float, 16>& matrix)
{
	std::stringstream ss;
	ss << name << " = " << std::endl;

	for (int row = 0; row < 4; ++row) {
		for (int col = 0; col < 4; ++col) {
			// For column-major order, access the matrix elements in this order:
			ss << matrix[col * 4 + row] << " ";
		}
		ss << std::endl; // Newline at the end of each row
	}

	std::cout << ss.str() << std::endl;
}


std::array<float, 16> multiplyMatrices(const std::array<float, 16>& a, const std::array<float, 16>& b) 
{
	std::array<float, 16> output = {};

	for (int i = 0; i < 4; ++i) { // row index
		for (int j = 0; j < 4; ++j) { // column index
			output[j * 4 + i] =
				a[0 * 4 + i] * b[j * 4 + 0] +
				a[1 * 4 + i] * b[j * 4 + 1] +
				a[2 * 4 + i] * b[j * 4 + 2] +
				a[3 * 4 + i] * b[j * 4 + 3];
		}
	}

	return output;
}

//Mat4x4 · vec4 = vec4
std::array<float, 4> multiplyMatVec(const std::array<float, 16>& m, const std::array<float, 4>& v) {
	std::array<float, 4> output = {};

	for (int i = 0; i < 4; ++i) { // row index
		output[i] =
			m[0 * 4 + i] * v[0] +
			m[1 * 4 + i] * v[1] +
			m[2 * 4 + i] * v[2] +
			m[3 * 4 + i] * v[3];
	}

	return output;
}


matrix4x4 invertMatrix(const matrix4x4& m)
{
	// Create augmented 4×8 matrix  [ M | I ]
	float aug[4][8] = {};
	for (int row = 0; row < 4; ++row)
	{
		for (int col = 0; col < 4; ++col)
			aug[row][col] = m[col * 4 + row];          // column‑major access

		aug[row][4 + row] = 1.0f;                      // Right‑hand identity
	}

	// Gauss‑Jordan elimination with partial pivoting
	for (int col = 0; col < 4; ++col)
	{
		// Pivot: find the row with largest absolute value in this column
		int pivot = col;
		float maxAbs = std::fabs(aug[pivot][col]);
		for (int r = col + 1; r < 4; ++r)
		{
			float absVal = std::fabs(aug[r][col]);
			if (absVal > maxAbs) { maxAbs = absVal; pivot = r; }
		}
		if (maxAbs < 1e-8f)   // determinant is zero
			throw std::runtime_error("invertMatrix: singular matrix");

		// Swap current row with pivot row
		if (pivot != col)
			for (int c = 0; c < 8; ++c)
				std::swap(aug[col][c], aug[pivot][c]);

		// Scale pivot row to make pivot element = 1
		float invPivot = 1.0f / aug[col][col];
		for (int c = 0; c < 8; ++c)
			aug[col][c] *= invPivot;

		// Eliminate this column from the other rows
		for (int r = 0; r < 4; ++r)
		{
			if (r == col) continue;
			float factor = aug[r][col];
			for (int c = 0; c < 8; ++c)
				aug[r][c] -= factor * aug[col][c];
		}
	}

	// Extract the right‑hand 4×4 block (the inverse)
	matrix4x4 inv{};
	for (int row = 0; row < 4; ++row)
		for (int col = 0; col < 4; ++col)
			inv[col * 4 + row] = aug[row][4 + col];    // back to column‑major

	return inv;
}


// Normalizes the quaternion [w, x, y, z] in-place
void normalizeQuaternion(std::array<float, 4>& q)
{
	float mag = std::sqrt(q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);
	if (mag > 0.0f) {
		q[0] /= mag;
		q[1] /= mag;
		q[2] /= mag;
		q[3] /= mag;
	}
}

float radians(float input) {
	return input * PI * inv180;
}


float degrees(float input) {
	return input * 180 * invPI;
}

