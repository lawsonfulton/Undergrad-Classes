#version 330

//Name: Lawson Fulton
//UserID: ljfulton
//Student#: 20381453

uniform float phong_coef;
uniform vec4 specular_color;
uniform vec4 diffuse_color;
uniform vec4 ambient_color;

uniform vec4 camera_loc;

uniform mat4 viewMatrix;


uniform vec4 light_loc;// = vec4(-1,1,5, 1.0); //* mat?

in vec4 normal;
in vec4 pos;

out vec4 finalColor;

void main()
{
    vec4 L = -normalize(light_loc - pos);
    vec4 R = 2.0 * dot(L, normal) * normal - L;
    vec4 V = -normalize(camera_loc - pos);

    vec4 ambient_diffuse_factor = max(-dot(normal, L), 0.0) + ambient_color;
    vec4 specular_factor = max(pow(dot(R, V), phong_coef), 0.0) * vec4(1,1,1,1);
    
    finalColor = specular_factor * specular_color
        + ambient_diffuse_factor * diffuse_color;
}
