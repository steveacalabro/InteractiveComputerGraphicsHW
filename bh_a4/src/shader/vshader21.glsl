
attribute vec4 vPosition;
attribute vec4 aColor;
varying vec4 vColor;
uniform mat4 u_MVP;

void main()
{
    gl_Position = u_MVP*vPosition;
	//gl_Position = vPosition;
	vColor = aColor;	
}
