attribute vec3 aPosition;
attribute vec3 aNormal;
attribute vec3 aCenterOfMass;
//attribute vec4 aColor;
//varying vec4 vNormal;
varying vec4 vColor;
uniform mat4 u_MVP;



struct Light{
	vec4 position;
	vec4 intensity;
	float kd;
};

void main()
{
	gl_Position = u_MVP * vec4(aPosition,1.0);
	vec4 normal = u_MVP * vec4(aNormal, 0.0); // remember to transform normal too

	Light light0, light1;
	light0.position = vec4(-0.5, 2.0, 0.0, 1.0);
	light0.intensity = vec4(0.1, 0.5, 0.3, 1.0);
	light0.kd = 0.8;

	light1.position = vec4(0.5, 0.8, 0.0, 1.0);
	light1.intensity = vec4(0.6, 0.1, 0.3, 1.0);
	light1.kd = 0.4;

	// per vertex shading
	//vec4 color1 = light0.intensity * light0.kd * clamp(dot(light0.position - vec4(aPosition, 1.0), normal), 0.0, 1.0);
	//vec4 color2 = light1.intensity * light1.kd * clamp(dot(light1.position - vec4(aPosition, 1.0), normal), 0.0, 1.0);

	// per triangle flat shading using center of mass 
	vec4 color1 = light0.intensity * light0.kd * clamp(dot(light0.position - vec4(aCenterOfMass, 1.0), normal), 0.0, 1.0);
	vec4 color2 = light1.intensity * light1.kd * clamp(dot(light1.position - vec4(aCenterOfMass, 1.0), normal), 0.0, 1.0);
	vColor = clamp(color1 + color2, 0.0, 1.0);
}
