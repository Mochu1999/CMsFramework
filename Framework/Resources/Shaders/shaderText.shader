#shader vertex 
#version 330 core

layout(location = 0) in vec4 positions;
layout(location = 1) in vec2 texCoord;


uniform mat4 u_OrthoProjection;

out vec2 v_TexCoord; 


void main() {
		gl_Position = u_OrthoProjection * positions;
		v_TexCoord = texCoord;	
    
}




#shader fragment
#version 330 core

uniform vec3 u_Color;
uniform sampler2D u_Texture; //No sé lo que hace, pero no se puede quitar
in vec2 v_TexCoord;

out vec4 FragColor;


void main()
{
    float alpha = texture(u_Texture, v_TexCoord).r;
    
    
    FragColor = vec4(u_Color, alpha);
}


