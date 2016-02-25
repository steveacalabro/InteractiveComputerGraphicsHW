#version 120
attribute vec3 aPosition;
attribute vec3 aColor;
varying vec4 vColor;
uniform mat4 u_MVP;

void main()
{
	gl_Position = u_MVP * vec4(aPosition, 1.0);
	vColor = vec4(aColor, 1.0);

}

