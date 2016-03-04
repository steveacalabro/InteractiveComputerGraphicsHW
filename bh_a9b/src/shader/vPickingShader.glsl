#version 120
attribute vec3 aPosition;
varying vec4 vColor;
uniform mat4 u_MVP;
uniform vec3 u_Color;
void main()
{
	gl_Position = u_MVP * vec4(aPosition, 1.0);
	vColor = vec4(u_Color, 1.0);

}

