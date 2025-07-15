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



//LETRAS TRANSPARENTES Y COLORES, CAMBIAR PIPELINE EN GENERAL
#shader fragment
#version 330 core


uniform sampler2D u_Texture; //No sé lo que hace, pero no se puede quitar. Pinta que va a ser necesario
in vec2 v_TexCoord;

out vec4 FragColor;


void main()         //FUNCIONA PERO CONTINUA EL MISTERIO DE U_TEXTURE Y LAS POLLADAS DE LAS LOCATIONS
{
    // Sample the glyph's alpha (assuming the red channel is used as a mask)
    float alpha = texture(u_Texture, v_TexCoord).r;
    
    // Output white text with the glyph's alpha.
    // The background will be transparent. Blending needs to be activated for it
    FragColor = vec4(1.0, 1.0, 1.0, alpha);
}


