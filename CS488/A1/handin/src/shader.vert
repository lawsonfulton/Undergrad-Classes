#version 330 

in vec3 vert;
in vec3 colour;
uniform mat4 mvpMatrix;
uniform vec4 solidColor;
uniform bool useSolidColor;

out vec4 vColour;

void main()
{	
	if(useSolidColor) {
		vColour = solidColor;
	} else {
		vColour = vec4(colour,1.0);
	}
	
    gl_Position = mvpMatrix * vec4(vert, 1.0);
}
