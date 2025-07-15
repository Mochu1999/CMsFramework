#shader vertex 
#version 330 core

// Assigns the slots in the Vertex Attribute Array where these attributes (position and normal) 
layout(location = 0) in vec3 positions;
layout(location = 1) in vec3 normals;


//uniforms are universal variables that can be accessed from other shaders and in main without the use of a VAO
uniform mat4 u_Model;
uniform mat4 u_Perspective;
uniform mat4 u_View;


out vec3 fragNormals;  //Output that goes to the fragment shader
out vec3 fragPositions;


void main() {

	fragPositions = positions;
	fragNormals = mat3(u_Model) * normals;
	gl_Position = u_Perspective * u_View * u_Model * vec4(fragPositions,1.0);
}




#shader fragment
#version 330 core



in vec3 fragNormals;
in vec3 fragPositions;

vec4 lightColor = vec4(1.0, 1.0, 1.0, 1.0);

uniform int u_fragmentMode; //1 for lit, 0 for full color
uniform vec3 u_CamPos;
uniform vec4 u_Color;
uniform vec3 u_lightPos;


out vec4 fragColor;

void main()
{
	if (u_fragmentMode == 0) 
	{
		float ambient = 0.25f;
		float specularLight = 0.7f;

		// diffuse lighting
		vec3 normal = normalize(fragNormals);
		vec3 lightDirection = normalize(u_lightPos - fragPositions);
		float diffuse = max(dot(normal, lightDirection), 0.0f);

		// specular lighting
		vec3 viewDirection = normalize(u_CamPos - fragPositions);
		vec3 reflectionDirection = reflect(-lightDirection, normal);
		float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 8);
		float specular = specAmount * specularLight*1.2;

		// outputs final color
		fragColor =  u_Color * lightColor * (diffuse + ambient + specular);
	}
	if (u_fragmentMode == 1) 
    {
        fragColor = u_Color;
    }

}



