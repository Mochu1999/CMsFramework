#shader vertex
#version 330 core

// Assigns the slots in the Vertex Attribute Array where these attributes (position and normal) go
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

//uniforms are universal variables that can be accessed from other shaders and in main without the use of a VAO
uniform mat4 u_MVP;


out vec3 Normal;  //Output that goes to the fragment shader
out vec3 crntPos;


void main() {
	crntPos = aPos;
	Normal = aNormal;
	gl_Position = u_MVP * vec4(crntPos, 1.0); //gl_Position is a keyword
}




//#shader fragment
//#version 330 core

//out vec4 FragColor;

//in vec3 Normal;  // Receive the normal from the vertex shader
//in vec3 crntPos;

//uniform vec3 camPos;
//uniform int u_RenderType; // 0 for geometry with normals, 1 for geometry without normals

//void main() {

//    if (u_RenderType == 0) 
//    {
//        vec3 normal = normalize(Normal);

//        float ambient = 0.20f;
//        vec3 lightDirection = normalize(vec3(0.0f,50.0f,0.0f) - crntPos);
//	    float diffuse = max(dot(normal, lightDirection), 0.0f);

//        // specular lighting
//	    float specularLight = 0.50f;
//	    vec3 viewDirection = normalize(camPos - crntPos);
//	    vec3 reflectionDirection = reflect(-lightDirection, normal);
//	    float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 8);
//	    float specular = specAmount * specularLight;

//        FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f) * (diffuse + ambient+diffuse);
//    }
//    if (u_RenderType == 1) 
//    {
//        // Just output the color directly, as in your original shader
//        FragColor = vec4(1.0, 0.0, 0.0, 1.0); // For example, you could use a red color or change this to `u_Color`.
//    }
//}


#version 330 core


out vec4 FragColor;

in vec3 Normal;
in vec3 crntPos;

//flat in vec3 Normal;  // Ensure you use flat shading for normals

vec4 lightColor = vec4(1.0, 1.0, 1.0, 1.0);
vec3 lightPos = vec3(25, 50, 25);

uniform int u_RenderType;
uniform vec3 camPos;
uniform vec4 u_Color;

vec4 objectColor = vec4(0.0f, 0.0f, 1.0f, 1.0f);

void main()
{
	if (u_RenderType == 0)
	{
		float ambient = 0.15f;
		float specularLight = 0.7f;

		// diffuse lighting
		vec3 normal = normalize(Normal);
		vec3 lightDirection = normalize(lightPos - crntPos);
		float diffuse = max(dot(normal, lightDirection), 0.0f);

		// specular lighting
		vec3 viewDirection = normalize(camPos - crntPos);
		vec3 reflectionDirection = reflect(-lightDirection, normal);
		float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 8);
		float specular = specAmount * specularLight;

		// outputs final color
		FragColor = objectColor * lightColor * (diffuse + ambient + specular);
	}
	if (u_RenderType == 1)
	{
		FragColor = u_Color;
	}
}























//#shader vertex 
//#version 330 core

//// Assigns the slots in the Vertex Attribute Array where these attributes (position and normal) go
//layout(location = 0) in vec4 position; 
//layout(location = 1) in vec3 normal;

////uniforms are universal variables that can be accessed from other shaders and in main without the use of a VAO
//uniform mat4 u_MVP;
//uniform mat4 u_ViewMatrix;

//out vec3 fragNormal;  //Output that goes tp the fragment shader
////out vec3 fragcolor;
//out vec3 crntPos;

//void main() {
//    fragNormal = mat3(u_ViewMatrix) * normal; // Transform the normal
//    gl_Position = u_MVP * position; //gl_Position is a keyword
//}




//#shader fragment
//#version 330 core

//out vec4 color;
//in vec3 fragNormal;  // Receive the normal from the vertex shader

//uniform int u_RenderType; // 0 for geometry with normals, 1 for geometry without normals
//uniform u_color;

//void main() {

//    if (u_RenderType == 0) 
//    {
//        vec3 normal = normalize(fragNormal);

//        // Map the normal vector to RGB. We normalize the values to [0, 1] by scaling them from [-1, 1]
//        vec3 rgbColor = normal;

//        color = vec4(rgbColor, 1.0); // Set the fragment color to the normal's RGB value
//    }
//    if (u_RenderType == 1) 
//    {
//        // Just output the color directly, as in your original shader
//        color = vec4(1.0, 0.0, 0.0, 1.0); // For example, you could use a red color or change this to `u_Color`.
//    }
//}





//Esta es la original

//#version 330 core
//out vec4 color;
//in vec3 fragNormal;  // Receive the normal from the vertex shader

//uniform vec4 u_Color;
// vec3 lightDirection = vec3(0.0, -1.0, 0.0);

// float ambientStrength = 0.1; // Ambient light intensity

// uniform int u_RenderType; // 0 for geometry with normals, 1 for geometry without normals

//void main() {

//    if (u_RenderType == 0) 
//    {
//        vec3 normal = normalize(fragNormal);

//        float diffuse = max(dot(normal, -lightDirection), 0.0);


//        vec4 ambient = ambientStrength * u_Color;
//        color = (ambient + diffuse) * u_Color;
//        color = diffuse * u_Color;
//    }
//    if (u_RenderType == 1) 
//    {
//        color = u_Color;
//    }
//}