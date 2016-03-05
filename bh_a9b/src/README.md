CS537 Homework Assignment 9b: Procedural Texture Mapping
====================
Author: Bingyao Huang
Date: 03/05/2016
Language: C++



This code loads and displays a procedural textured triangle-mesh object. The texture implemented in Phong model, the user is able to choose between the two base diffuse textures: wood and cloth. A mirror like reflection procedural texture is applied to the base texture. The turbulence equation is given by:

float mirrorEffect(vec3 normal){
	float mirrorX = mod(asin(normal.x)*pow(normal.z, normal.y), fract(tan(pow(normal.z, normal.x))));
	float mirrorY = mod(cos(normal.y)*pow(normal.z, normal.x), fract(tan(pow(normal.y, normal.x))));

	float mirrorZ = mod(atan(normal.z)*pow(normal.y, normal.x), fract(tan(pow(normal.z, normal.y))));
	return  mirrorX*mirrorY*mirrorZ;
}

float turbulence(vec3 normal){
	float value = 0;
	for(float f=20;f<80;f*=3){
		value += abs(mirrorEffect(normal))/f;		
	}
	return value*1000;
}

Two lights are placed in the scene, one light source is attached to the camera, the other is attached to the object. The scene can be viewed in 3 different projection modes (perspective, orthographic and parallel). A rotating camera view of the model is displayed. The camera rotates on a cylinder around the model. Allow the user to interactively change the height, the orbit radius and speed of the camera. The camera rotates around and look at the objects.


***************************** Instructions *********************************** 
1. Press 'W', 'S' for increasing and decreasing the camera's Y position
2. Press 'A', 'D' for increasing and decreasing the camera's rotation raidus
3. Press 'Q', 'E' to increasing and decreasing the camera's rotation speed
4. Press 'Z', 'X' to increasing and decreasing the light's rotation raidus
5. Press 'C', 'V' to increasing and decreasing the light's Y position
6. Press 'B', 'N' to increasing and decreasing the light's rotation anglen
7. Press 'R' to reset camera
8. Press 'SPACE' to pause/resume animationn
9. Use mouse right click to change camera's projection mode and shading model
10. Press 'ESC' to exit program

###Contents:
	source files: src\*.cpp
	makefile: Makefile
	library folder: include
	

###Usage:	
	make: the default target compiles all the code
	make run: executes the program(s)
	make clean: deletes all the object files and executable files.
	
	

	

