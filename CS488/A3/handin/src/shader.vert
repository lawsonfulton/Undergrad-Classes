#version 330 

//Name: Lawson Fulton
//UserID: ljfulton
//Student#: 20381453

in vec3 vert;
uniform mat4 projMatrix;
uniform mat4 viewMatrix;

uniform mat4 transRotMatrix;
uniform mat4 scaleMatrix;
uniform mat4 itScaleMatrix;

out vec4 normal;
out vec4 pos;

void main()
{	
	gl_Position = projMatrix * transRotMatrix * scaleMatrix * vec4(vert, 1.0);


    pos = transRotMatrix * scaleMatrix * vec4(vert, 1.0);

    normal =  normalize(itScaleMatrix * vec4(vert,0));
}
