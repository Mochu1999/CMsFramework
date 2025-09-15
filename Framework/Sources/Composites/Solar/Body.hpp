#pragma once



struct Body
{
	Sphere sphere;

	float mass;
	float radius;

	p3 position;
	p3 velocity;
	p3 acceleration; //must be calculated after calculating all bodies

	Body(float mass_, float radius_, p3 position_ = { 0,0,0 }, p3 velocity_ = { 0,0,0 } )
		:mass(mass_), radius(radius_), position(position_), velocity(velocity_)
		,sphere(radius_,1000)
	{
		sphere.addSet({ 0,0,0 }); //to be moved with the modelmatrix
	}

	void draw() //model matrix must be changed to positions from elsewhere
	{
		sphere.draw();
	}
};