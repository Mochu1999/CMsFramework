#include "Camera.hpp"

std::array<float, 16> Camera::createScalingMatrix(float scaleX, float scaleY, float scaleZ) {
	std::array<float, 16> scalingMatrix = { 0 };
	scalingMatrix[0] = scaleX;
	scalingMatrix[5] = scaleY;
	scalingMatrix[10] = scaleZ;
	scalingMatrix[15] = 1.0f;
	return scalingMatrix;
}


array<float, 16> Camera::createOrthoMatrix() {
	array<float, 16> orthoMatrix = {};

	float left = 0.0f;
	float right = windowWidth;
	float bottom = 0.0f;
	float top = windowHeight;
	float nearZ = -1.0f;
	float farZ = 1.0f;

	orthoMatrix[0] = 2.0f / (right - left);
	orthoMatrix[5] = 2.0f / (top - bottom);
	orthoMatrix[10] = -2.0f / (farZ - nearZ);
	orthoMatrix[12] = -(right + left) / (right - left);
	orthoMatrix[13] = -(top + bottom) / (top - bottom);
	orthoMatrix[14] = -(farZ + nearZ) / (farZ - nearZ);
	orthoMatrix[15] = 1.0f;

	return orthoMatrix;
}

array<float, 16> Camera::createPerspectiveMatrix() {
	array<float, 16> perspectiveMatrix = {};

	float tanHalfFov = tan(fov / 2);

	perspectiveMatrix[0] = 1 / (aspectRatio * tanHalfFov);
	perspectiveMatrix[5] = 1 / (tanHalfFov);
	perspectiveMatrix[10] = -(farZ + nearZ) / (farZ - nearZ);
	perspectiveMatrix[11] = -1.0f;
	perspectiveMatrix[14] = -2 * farZ * nearZ / (farZ - nearZ);

	return perspectiveMatrix;
}

//we want to move the world so the camera frustrum ends clipped in the square of range -1,1 for the axes x and y; and same range for the
// near and far z planes
std::array<float, 16> Camera::createViewMatrix(const p3& right, const p3& up, p3 forward, const p3& cameraPos) {



	std::array<float, 16> viewMatrix = { 0 };

	//rotation: change of basis from the world axes to one aligned with the camera local axes right, up and forward respectlively
	//the forward element is negative because we consider -z to be forward
	viewMatrix[0] = right.x;
	viewMatrix[1] = up.x;
	viewMatrix[2] = -forward.x;

	viewMatrix[4] = right.y;
	viewMatrix[5] = up.y;
	viewMatrix[6] = -forward.y;

	viewMatrix[8] = right.z;
	viewMatrix[9] = up.z;
	viewMatrix[10] = -forward.z;

	//Translation. Moves the world en the opposite direction of the camera position so it ends up in the origin
	// It must be done with a dot product. Because otherwise when the camera's rotated, you wouldn't end in the origin because the camera
	// would be translated with respect of its local axes
	viewMatrix[12] = -dot3(right, cameraPos);
	viewMatrix[13] = -dot3(up, cameraPos);
	viewMatrix[14] = dot3(forward, cameraPos);


	viewMatrix[15] = 1.0f;

	return viewMatrix;
}









void Camera::calculateForward(p3& forward, const float rotationSpeed, const p3& rotationAxis)
{
	p3 intermForward;

	rotatePoint(forward, rotationSpeed, rotationAxis);
	intermForward = normalize3(forward);

	if (abs(intermForward.y) < 0.99)
		forward = intermForward;
}


void Camera::updateCamera() {

	p2 currentMPosVariation = gv.LastLMPos - gv.mPos;
	p2 currentMiddleMPosVariation = gv.LastMMPos - gv.mPos;


	if (gv.program == telemetry || gv.program == solar || gv.program == openCascade)
	{
		if (gv.cameraMode == drag && gv.isLmbPressed)
		{
			calculateForward(forward, currentMPosVariation.y * 0.001, right);

			right = normalize3(cross3(forward, { 0,1,0 }));
			up = cross3(right, forward);

			viewMatrix = createViewMatrix(right, up, forward, cameraPos);

			calculateForward(forward, -currentMPosVariation.x * 0.001, up);

			gv.LastLMPos = gv.mPos;
		}
		else if (gv.cameraMode == FPS)
		{
			calculateForward(forward, -currentMPosVariation.y * 0.003, right);

			right = normalize3(cross3(forward, { 0,1,0 }));
			up = cross3(right, forward);

			viewMatrix = createViewMatrix(right, up, forward, cameraPos);

			calculateForward(forward, currentMPosVariation.x * 0.003, up);

			gv.LastLMPos = gv.mPos;
		}
		else if (gv.cameraMode == centered)
		{

			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			{
				rotatePoint(cameraPos, centeredAngleRotation, { 0, 1, 0 });
				//rotate3D( cameraPos , 0, centeredAngleRotation, 0);
			}
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			{
				rotatePoint(cameraPos, -centeredAngleRotation, { 0, 1, 0 });

			}
			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			{
				rotatePoint(cameraPos, -centeredAngleRotation, right);

				if (up.y <= 0.1 && cameraPos.y > 0)
					rotatePoint(cameraPos, centeredAngleRotation, right);
			}
			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			{
				rotatePoint(cameraPos, centeredAngleRotation, right);

				if (up.y <= 0.1 && cameraPos.y < 0)
					rotatePoint(cameraPos, -centeredAngleRotation, right);
			}



			forward = -normalize3(cameraPos);
		}

		
	}

	if (gv.program == openCascade)
	{
		if (gv.isMmbPressed)
		{
			//cameraPos -= right * (currentMiddleMPosVariation.x * 0.05);
			//cameraPos -= forward * (currentMiddleMPosVariation.y * 0.05);
			//gv.totalMiddleMPosVariation.y = 10;

			////translate3DModelMatrix(gv.modelMatrixOCC, gv.totalMiddleMPosVariation);
			////cameraPos += gv.totalMiddleMPosVariation;

			//gv.LastMMPos = gv.mPos;
		}

	}

	//The quaternion method is intentionally incomplete. The true method would calculate the f and u for pitch and f and r for yaw
	// But instead of calculating everything there we are only calculating f and here forcing right to be with respect of the referenceUp
	// of the world and up is calculated from there
	right = normalize3(cross3(forward, { 0,1,0 }));
	up = cross3(right, forward);

	viewMatrix = createViewMatrix(right, up, forward, cameraPos);

	vpMatrix = multiplyMatrices(perspectiveMatrix, viewMatrix);


	shader3D.bind();
	shader3D.setUniform("u_CamPos", cameraPos);
	shader3D.setUniform("u_View", viewMatrix);


	//print(cursorToXZPlane());
	
}







//std::array<float, 16> Camera::create3DModelMatrix(const p3 translation) {
//
//	std::array<float, 16> modelMatrix = createIdentityMatrix();
//
//	//Translation components in the fourth column
//	modelMatrix[12] = translation.x;
//	modelMatrix[13] = translation.y;
//	modelMatrix[14] = translation.z;
//
//	return modelMatrix;
//}
//
//std::array<float, 16> Camera::create3DModelMatrix(const float angleDeg, p3 axis) {
//
//	std::array<float, 16> modelMatrix = createIdentityMatrix();
//
//	float theta = radians(angleDeg);
//
//	axis = normalize3(axis);
//	float x = axis.x;
//	float y = axis.y;
//	float z = axis.z;
//
//	float c = std::cos(theta);
//	float s = std::sin(theta);
//	float oneMinusC = 1.0f - c;
//
//	// 4) Fill the 3x3 rotation in column-major order
//	//    Using the formula:
//	//      r_xx = x*x*(1-c)+c,   r_xy = x*y*(1-c)-z*s,   r_xz = x*z*(1-c)+y*s
//	//      r_yx = y*x*(1-c)+z*s, r_yy = y*y*(1-c)+c,     r_yz = y*z*(1-c)-x*s
//	//      r_zx = z*x*(1-c)-y*s, r_zy = z*y*(1-c)+x*s,   r_zz = z*z*(1-c)+c
//
//
//	// Column 0
//	modelMatrix[0] = x * x * oneMinusC + c;    // r_xx
//	modelMatrix[1] = y * x * oneMinusC + z * s;  // r_yx
//	modelMatrix[2] = z * x * oneMinusC - y * s;  // r_zx
//
//	// Column 1
//	modelMatrix[4] = x * y * oneMinusC - z * s;  // r_xy
//	modelMatrix[5] = y * y * oneMinusC + c;    // r_yy
//	modelMatrix[6] = z * y * oneMinusC + x * s;  // r_zy
//
//	// Column 2
//	modelMatrix[8] = x * z * oneMinusC + y * s;  // r_xz
//	modelMatrix[9] = y * z * oneMinusC - x * s;  // r_yz
//	modelMatrix[10] = z * z * oneMinusC + c;    // r_zz
//
//
//
//	return modelMatrix;
//}
//
//
//
//std::array<float, 16> Camera::create3DModelMatrix(const float scale) {
//
//	std::array<float, 16> modelMatrix = createIdentityMatrix();
//
//
//	modelMatrix[0] *= scale;
//	modelMatrix[5] *= scale;
//	modelMatrix[10] *= scale;
//
//
//	return modelMatrix;
//}


void Camera::create3DModelMatrix(std::array<float, 16>& model3DMatrix, const p3 translation_, const float scale_) {

	if (translation != translation_)
	{
		print(translation);
		print(translation_);
		translate3DModelMatrix(model3DMatrix, translation_);
		translation = translation_;
	}

	if (scale != scale_)
	{
		print(scale);
		print(scale_);
		//scaling goes always at the end
		scale3DModelMatrix(model3DMatrix, scale_);
		//scale = scale_;
	}


}

void Camera::rotate3DModelMatrix(std::array<float, 16>& model3DMatrix, const float angleDeg, p3 axis) {

	model3DMatrix = identityMatrix; //CREO QUE NO SIEMPRE HACE FALTA// O SI? NO SÉ SI ES IRRELEVANTE

	float theta = radians(angleDeg);

	axis = normalize3(axis);
	float x = axis.x;
	float y = axis.y;
	float z = axis.z;

	float c = std::cos(theta);
	float s = std::sin(theta);
	float oneMinusC = 1.0f - c;

	// 4) Fill the 3x3 rotation in column-major order
	//    Using the formula:
	//      r_xx = x*x*(1-c)+c,   r_xy = x*y*(1-c)-z*s,   r_xz = x*z*(1-c)+y*s
	//      r_yx = y*x*(1-c)+z*s, r_yy = y*y*(1-c)+c,     r_yz = y*z*(1-c)-x*s
	//      r_zx = z*x*(1-c)-y*s, r_zy = z*y*(1-c)+x*s,   r_zz = z*z*(1-c)+c


	// Column 0
	model3DMatrix[0] = x * x * oneMinusC + c;    // r_xx
	model3DMatrix[1] = y * x * oneMinusC + z * s;  // r_yx
	model3DMatrix[2] = z * x * oneMinusC - y * s;  // r_zx

	// Column 1
	model3DMatrix[4] = x * y * oneMinusC - z * s;  // r_xy
	model3DMatrix[5] = y * y * oneMinusC + c;    // r_yy
	model3DMatrix[6] = z * y * oneMinusC + x * s;  // r_zy

	// Column 2
	model3DMatrix[8] = x * z * oneMinusC + y * s;  // r_xz
	model3DMatrix[9] = y * z * oneMinusC - x * s;  // r_yz
	model3DMatrix[10] = z * z * oneMinusC + c;    // r_zz

}

void Camera::translate3DModelMatrix(std::array<float, 16>& model3DMatrix, const p3 translation_) {

	//Translation components in the fourth column
	model3DMatrix[12] = translation_.x;
	model3DMatrix[13] = translation_.y;
	model3DMatrix[14] = translation_.z;
}

//Esto está mal, es acumulativo, no puede ir sin un rotate
void Camera::scale3DModelMatrix(std::array<float, 16>& model3DMatrix, const float scale_) {

	model3DMatrix[0] *= scale_;
	model3DMatrix[5] *= scale_;
	model3DMatrix[10] *= scale_;

	//mirar si esto se usa en rotadas
	//modelMatrix[0] *= scale_;
	//		modelMatrix[1] *= scale_;
	//		modelMatrix[2] *= scale_;
	//		modelMatrix[4] *= scale_;
	//		modelMatrix[5] *= scale_;
	//		modelMatrix[6] *= scale_;
	//		modelMatrix[8] *= scale_;
	//		modelMatrix[9] *= scale_;
	//		modelMatrix[10] *= scale_;
}


//std::array<float, 16> Camera::create3DModelMatrix(const p3 translation,const float angleDeg, p3 axis,const float scale) { //scale defaulted to 1
//
//	std::array<float, 16> modelMatrix = createIdentityMatrix();
//
//	float theta = radians(angleDeg);
//
//	axis = normalize3(axis);
//	float x = axis.x;
//	float y = axis.y;
//	float z = axis.z;
//
//	float c = std::cos(theta);
//	float s = std::sin(theta);
//	float oneMinusC = 1.0f - c;
//
//	// 4) Fill the 3x3 rotation in column-major order
//	//    Using the formula:
//	//      r_xx = x*x*(1-c)+c,   r_xy = x*y*(1-c)-z*s,   r_xz = x*z*(1-c)+y*s
//	//      r_yx = y*x*(1-c)+z*s, r_yy = y*y*(1-c)+c,     r_yz = y*z*(1-c)-x*s
//	//      r_zx = z*x*(1-c)-y*s, r_zy = z*y*(1-c)+x*s,   r_zz = z*z*(1-c)+c
//
//
//	// Column 0
//	modelMatrix[0] = x * x * oneMinusC + c;    // r_xx
//	modelMatrix[1] = y * x * oneMinusC + z * s;  // r_yx
//	modelMatrix[2] = z * x * oneMinusC - y * s;  // r_zx
//
//	// Column 1
//	modelMatrix[4] = x * y * oneMinusC - z * s;  // r_xy
//	modelMatrix[5] = y * y * oneMinusC + c;    // r_yy
//	modelMatrix[6] = z * y * oneMinusC + x * s;  // r_zy
//
//	// Column 2
//	modelMatrix[8] = x * z * oneMinusC + y * s;  // r_xz
//	modelMatrix[9] = y * z * oneMinusC - x * s;  // r_yz
//	modelMatrix[10] = z * z * oneMinusC + c;    // r_zz
//
//	// Column 3 (translation)
//	modelMatrix[12] = translation.x;
//	modelMatrix[13] = translation.y;
//	modelMatrix[14] = translation.z;
//
//	if (scale != 1)
//	{
//		//apply uniform scale to the 3×3 rotation block
//		modelMatrix[0] *= scale;
//		modelMatrix[1] *= scale;
//		modelMatrix[2] *= scale;
//		modelMatrix[4] *= scale;
//		modelMatrix[5] *= scale;
//		modelMatrix[6] *= scale;
//		modelMatrix[8] *= scale;
//		modelMatrix[9] *= scale;
//		modelMatrix[10] *= scale;
//	}
//
//	return modelMatrix;
//}
//
//
std::array<float, 16> Camera::create2DModelMatrix(const p2 translation, float angleDeg, float scale) {
	std::array<float, 16> modelMatrix = identityMatrix;
	float theta = radians(angleDeg);
	float c = std::cos(theta);
	float s = std::sin(theta);


	// Column 0: [scale * cosθ, scale * sinθ, 0, 0]
	modelMatrix[0] = scale * c;
	modelMatrix[1] = scale * s;

	// Column 1: [-scale * sinθ, scale * cosθ, 0, 0]
	modelMatrix[4] = -scale * s;
	modelMatrix[5] = scale * c;

	// Column 2: [0, 0, 1, 0] 

	// Column 3: [translation.x, translation.y, 0, 1]
	modelMatrix[12] = translation.x;
	modelMatrix[13] = translation.y;

	return modelMatrix;
}

std::array<float, 16> Camera::create2DModelMatrix(const p2 translation, float angleDeg, p2 scale) {
	std::array<float, 16> modelMatrix = identityMatrix; // Assume this returns a 4x4 identity matrix
	float theta = radians(angleDeg);
	float c = std::cos(theta);
	float s = std::sin(theta);


	// Column 0: [scale * cosθ, scale * sinθ, 0, 0]
	modelMatrix[0] = scale.x * c;
	modelMatrix[1] = scale.x * s;

	// Column 1: [-scale * sinθ, scale * cosθ, 0, 0]
	modelMatrix[4] = -scale.y * s;
	modelMatrix[5] = scale.y * c;

	// Column 2: [0, 0, 1, 0] 

	// Column 3: [translation.x, translation.y, 0, 1]
	modelMatrix[12] = translation.x;
	modelMatrix[13] = translation.y;

	return modelMatrix;
}


p3 Camera::cursorToXZPlane()
{
	matrix4x4 invVP = invertMatrix(vpMatrix);

	// Step 1: Cursor to NDC, Normalized‑Device Coordinates [–1 … +1]
	float ndcX = 2.0f * gv.mPos.x / windowWidth - 1.0f;
	float ndcY = 2.0f * gv.mPos.y / windowHeight - 1.0f;

	/*print(ndcX);
	print(ndcY);*/

	//step 2: z = −1 is the near plane, z = +1 the far plane in OpenGL clip coordinates. These two points define the ray.
	std::array<float, 4> clipNear = { ndcX, ndcY, -1.0f, 1.0f };
	std::array<float, 4> clipFar = { ndcX, ndcY,  1.0f, 1.0f };


	// Step 3: Unproject to world space
	std::array<float, 4> worldNear = multiplyMatVec(invVP, clipNear);
	std::array<float, 4> worldFar = multiplyMatVec(invVP, clipFar);

	// Perspective divide
	for (int i = 0; i < 3; ++i) {
		worldNear[i] /= worldNear[3];
		worldFar[i] /= worldFar[3];
	}

	p3 rayOrigin = { worldNear[0], worldNear[1], worldNear[2] };
	p3 rayDir = normalize3(p3{
		worldFar[0] - worldNear[0],
		worldFar[1] - worldNear[1],
		worldFar[2] - worldNear[2]
		});

	// Step 4: Ray-plane intersection (XZ plane at Y=0)
	if (fabs(rayDir.y) < 1e-6f) {
		// Ray is parallel to XZ plane → no intersection
		return { 0.0f, 0.0f, 0.0f }; // Or some invalid marker
	}

	float t = -rayOrigin.y / rayDir.y;
	p3 intersection = rayOrigin + rayDir * t;

	// Force result to XZ plane (y = 0)
	return { intersection.x, 0.0f, intersection.z };
}


void Camera::setCursorToXZPoint(const p3& pointInXZ)
{
	// Step 1: World point → Clip Space
	std::array<float, 4> worldPos = { pointInXZ.x, 0.0f, pointInXZ.z, 1.0f };

	// Multiply view matrix first
	std::array<float, 4> viewPos = multiplyMatVec(viewMatrix, worldPos);

	// Then project to clip space
	std::array<float, 4> clipPos = multiplyMatVec(perspectiveMatrix, viewPos);

	// Step 2: Perspective divide (to NDC)
	if (fabs(clipPos[3]) < 1e-6f)
		return; // Avoid division by zero

	float ndcX = clipPos[0] / clipPos[3];
	float ndcY = clipPos[1] / clipPos[3];

	// Step 3: NDC → Screen-space (pixels, bottom-left origin)
	float screenX = (ndcX + 1.0f) * 0.5f * windowWidth;
	float screenY = (ndcY + 1.0f) * 0.5f * windowHeight;

	// Step 4: Flip Y to match GLFW's top-left origin
	float glfwY = windowHeight - screenY;

	// Step 5: Move cursor
	glfwSetCursorPos(window, screenX, glfwY);
}
