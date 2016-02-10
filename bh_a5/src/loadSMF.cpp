// HW4
#include "main.h"
#include "BoxGeometry.h"
#include "MeshObject.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "Plane.h"
using namespace std;


//Mesh mesh;

int loadSFM(char* fileName, Mesh &mesh) {
	fstream file(fileName);
	string buffer;

	vector<point3> vertices;
	vector<point3> faces;


	// if not opened
	if (!file.is_open())
	{
		cout << "Unable to open file" << endl;
		return 0;
	}

	// read file line by line
	while (getline(file, buffer, '\n'))
	{
		//if (buffer[0] == 'v') {
		//	fprintf(stdout, "Got v\n");
		//}

		if (buffer[0] != 'v' && buffer[0] != 'f') {
			continue;
		}

		// read each element in this line
		stringstream currentLine(buffer);
		string currentColumn;
		vector<double> triangleElement;
		while (getline(currentLine, currentColumn, '\ '))
		{
			if (currentColumn[0] != 'v' && currentColumn[0] != 'f') {
				triangleElement.push_back(stod(currentColumn)); //! stod converts string to double
			}
		}

		if (triangleElement.size() != 3) {
			fprintf(stdout, "Only read %d elements from current line \n", triangleElement.size());
			return 0;
		}


		if (buffer[0] == 'v') {
			vertices.push_back(point3(triangleElement[0], triangleElement[1], triangleElement[2]));
		}
		else if (buffer[0] == 'f') {
			faces.push_back(point3(triangleElement[0], triangleElement[1], triangleElement[2]));
		}
		else {
			fprintf(stdout, "buffer[0] is not either 'v' or 'f' \n");
			return 0;
		}
		
		
	}

	Plane temp;

	// store data in mesh
	for (int i = 0; i < faces.size(); i++) {
		//! smf index starts from 1 instead of 0, so need to -1
		Triangle3D facet;

		facet.vertices[0] = vertices[int(faces[i][0]) - 1];
		facet.vertices[1] = vertices[int(faces[i][1]) - 1];
		facet.vertices[2] = vertices[int(faces[i][2]) - 1];

		
		temp.set3Points(facet.vertices[0], facet.vertices[2], facet.vertices[1]); // in this case clock wise is normal direction

		facet.normal = temp.normal;
		facet.centerOfMass = (facet.vertices[0] + facet.vertices[1] + facet.vertices[2]) / 3;
		mesh.push_back(facet);
	}

	return 1;
}