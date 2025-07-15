#shader vertex 
#version 330 core

layout(location = 0) in vec2 positions;

uniform mat4 u_OrthoProjection;
uniform mat4 u_Model;


void main() {

	gl_Position = u_OrthoProjection * u_Model * vec4(positions, 0.0, 1.0);

}




#shader fragment
#version 330 core

uniform vec4 u_Color;

out vec4 fragColor;

void main()
{
	fragColor = u_Color;
}



