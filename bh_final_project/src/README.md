CS537 Final Project - Modeling Hierarchy
====================
#####Author: Bingyao Huang
#####Date: 03/06/2016
#####Language: C++

### Project Description
This project creates a 3-link SCARA "robot" that has 3 arms attached to a solid base. As described in [Interactive Computer Graphics - Extra Credit Assignment](https://www.cs.drexel.edu/~david/Classes/ICG/Assignments_new/extraCredit.html), 3 arms are attached hierarchically to each other by 4 joints, so they can rotate around the top of a post (Joint 1), rotates around a revolute joint (Joint 2), and translates along a prismatic joint (Joint 3). 

This project is implemented using OpenGL and allows the user interactively choose via mouse left click which arm to edit (either change the rotation or the translation around the attached joint). The chosen arm angle/translation can then be modifiable via key 'T' and 'Y'.

Each arm’s transformation is dependent on its parent arm, thus each arm has a unique composite transformation matrix that transforms itself from its model space to its parent's model space. The final model matrix is caluclated by recursively multiplying an arm’s model matrix with higher hierarchical model's matrix. 

Two lights are placed in the scene, one light source is attached to the camera, the other is attached to the object. The scene can be viewed in 3 different projection modes (perspective, orthographic and parallel). A rotating camera view of the model is displayed. The camera rotates on a cylinder around the model. Allow the user to interactively change the height, the orbit radius and speed of the camera. The camera rotates around and look at the world origin.

### Instructions
1. Press 'W', 'S' for + and - the camera's Y position
2. Press 'A', 'D' for + and - the camera's rotation raidus
3. Press 'Q', 'E' to + and - the camera's rotation speed
4. Use mouse left click to pick arm for transformation
5. Press 'T', 'Y' for + and - the selected arm's parameter
6. Press 'Z', 'X' to + and - the light's rotation raidus
7. Press 'C', 'V' to + and - the light's Y position
8. Press 'B', 'N' to + and - the light's rotation anglen
9. Press 'R' to reset camera
10. Press 'SPACE' to pause/resume animationn
11. Use mouse right click to change multiple rendering paramters
12. Press 'ESC' to exit program

###Contents:
	source files: src\*.cpp
	makefile: Makefile
	library folder: include
	

###Usage:	
	make: the default target compiles all the code
	make run: executes the program(s)
	make clean: deletes all the object files and executable files.
	
	

	

