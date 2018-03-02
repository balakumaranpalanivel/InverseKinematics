#version 330

// Note: Input to this shader is the vertex positions that we specified for the triangle. 
// Note: gl_Position is a special built-in variable that is supposed to contain the vertex position (in X, Y, Z, W)
// Since our triangle vertices were specified as vec3, we just set W to 1.0.

in vec3 vPosition;
in vec4 vColor;
out vec4 color;

void main()
{
	gl_Position = vec4(vPosition.x, vPosition.y, vPosition.z, 1.0);
	color = vColor;
}