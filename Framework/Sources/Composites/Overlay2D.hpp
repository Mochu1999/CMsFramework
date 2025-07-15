#pragma once

struct Overlay2D {

	Shader& shader2D;
	Camera& camera;


	Polygons2D background;
	Lines2D backgroundLines;

	Overlay2D(Shader& shader2D_, Camera& camera_) :shader2D(shader2D_), camera(camera_) {

		p2 corner = { 1300,0 };
		vector<p2> arc = createRoundedSquare({ corner.x,0 }, { windowWidth - corner.x,840 }, 25);
		background.addSet(arc);
		backgroundLines.addSet(arc);
	}


	void draw() {
		shader2D.bind();
		transparent();
		shader2D.setUniform("u_Model", camera.identityMatrix);
		shader2D.setUniform("u_Color", 0.035f, 0.065f, 0.085f, 1.0f);
		background.draw();

		shader2D.setUniform("u_Color", 40.0f / 255.0f, 239.9f / 255.0f, 239.0f / 255.0f, 1);
		glLineWidth(5);
		shader2D.setUniform("u_Model", camera.identityMatrix);
		backgroundLines.draw();
		glLineWidth(1);
	}
};