#pragma once

#include "Common.hpp"
#include "Shader.hpp"
/*
0  4  8  12
1  5  9  13
2  6  10 14
3  7  11 15
*/



struct Camera {

	float translationSpeed = 0.20f, rotationSpeed = 0.05f;

	GLFWwindow* window;
	Shader& shader3D, shader2D, shader2D_Instanced, shaderText;
	GlobalVariables& gv;

	float fov = 45.0f * PI / 180;
	float aspectRatio = windowWidth / windowHeight;
	float nearZ = 0.1f;
	float farZ = 3000.0f;

	float centeredAngleRotation = 0.1;
	float centeredRadius = 0;


	std::array<float, 16> perspectiveMatrix, viewMatrix, vpMatrix, orthoMatrix, identityMatrix;
	

	p3 cameraPos;

	//p3 forward = { 0.0f,-1.0f,0.0f }; //cuando dejó esto de funcionar?
	p3 forward = normalize3(p3{ 0.148691,-0.742031,-0.651083 });
	p3 right; //0 because are gettin recalculated anyways
	p3 up;

	p3 translation;
	float scale = 1;


	Camera(GLFWwindow* window_, Shader& shader3D_, Shader& shader2D_, Shader& shader2D_Instanced_, Shader& shaderText_, GlobalVariables& gv_)
		:window(window_), shader3D(shader3D_), shader2D(shader2D_), shader2D_Instanced(shader2D_Instanced_), shaderText(shaderText_), gv(gv_) 
	{
		identityMatrix = gv.identityMatrix;
		orthoMatrix = createOrthoMatrix();
		perspectiveMatrix = createPerspectiveMatrix();

		p3 objPos = { 5.0f, 2.0f, 1.0f };
		float angle = 3.14159f * 0.25f; // 45 degrees in radians
		p3 rotAxis = { 0.0f, 1.0f, 0.0f }; // Y axis

		//modelMatrix = create3DModelMatrix(objPos, angle, rotAxis);

		

		updateCamera();


		//Locations initializers
		{
			//3D
			shader3D.bind();
			shader3D.setUniform("u_Perspective", perspectiveMatrix);
			shader3D.setUniform("u_Model3D", identityMatrix);

			//2D
			shader2D.bind();
			shader2D.setUniform("u_OrthoProjection", orthoMatrix);
			shader2D.setUniform("u_Model", identityMatrix);


			//2D_Instanced
			shader2D_Instanced.bind();
			shader2D_Instanced.setUniform("u_OrthoProjection", orthoMatrix);

			//Text
			shaderText.bind();
			shaderText.setUniform("u_OrthoProjection", orthoMatrix);
		}
	}

	p3 cursorToXZPlane();
	void setCursorToXZPoint(const p3& pointInXZ);


	std::array<float, 16> createScalingMatrix(float scaleX, float scaleY, float scaleZ);


	array<float, 16> createOrthoMatrix();

	////Perspective and View
	array<float, 16> createPerspectiveMatrix();

	std::array<float, 16> createViewMatrix(const p3& right, const p3& up, p3 forward, const p3& cameraPos);

	


	/*std::array<float, 16> create3DModelMatrix(const p3 translation);
	std::array<float, 16> create3DModelMatrix(const p3 translation, float angleDeg, p3 axis);*/
	void create3DModelMatrix(std::array<float, 16>& model3DMatrix, const p3 translation_, const float scale_);
	/*std::array<float, 16> create3DModelMatrix(const float angleDeg, p3 axis);
	std::array<float, 16> create3DModelMatrix(const float angleDeg, p3 axis, const float scale);
	std::array<float, 16> create3DModelMatrix(const float scale);
	std::array<float, 16> create3DModelMatrix(const p3 translation, const float angleDeg, p3 axis,const float scale);*/

	void rotate3DModelMatrix(std::array<float, 16>& model3DMatrix, const float angleDeg, p3 axis);
	void translate3DModelMatrix(std::array<float, 16>& model3DMatrix, const p3 translation_);
	void scale3DModelMatrix(std::array<float, 16>& model3DMatrix, const float scale_);

	std::array<float, 16> create2DModelMatrix(const p2 translation, float angleDeg, float scale);
	std::array<float, 16> create2DModelMatrix(const p2 translation, float angleDeg, const p2 scale);

	//the rotations create a new forward vector and the other 2 are deduced from it
	void calculateForward(p3& forward, const float rotationSpeed, const p3& rotationAxis);





	//creates all the vector and updates the view and vp matrices
	void updateCamera();






};

