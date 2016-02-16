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


void averageNormals(vector<point3> &vertices, vector<point3> &faces, vector<vec3> &normals) {
	Plane plane;
	vector<vec3> faceNormals;
	for (int i = 0; i < faces.size(); i++) {
		plane.set3Points(vertices[int(faces[i][0]) - 1], vertices[int(faces[i][2]) - 1], vertices[int(faces[i][1]) - 1]);
		faceNormals.push_back(plane.normal);
	}


	for (int i = 0; i < vertices.size(); i++) {
		int numOfShares = 0;
		for (int j = 0; j < faces.size(); j++) {
			int v0 = int(faces[j][0] - 1);
			int v1 = int(faces[j][1] - 1);
			int v2 = int(faces[j][2] - 1);

			if (i == v0 || i == v1 || i == v2) {
				normals[i] += faceNormals[j];
				numOfShares++;
			}
		}
		normals[i] /= numOfShares;
		normals[i] = normalize(normals[i]);
	}
}


int loadSFM(char* fileName, Mesh &mesh) {
	fstream file(fileName);
	string buffer;

	vector<point3> vertices;
	vector<vec3>   normals;
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
			normals.push_back(vec3(0.0));
		}
		else if (buffer[0] == 'f') {
			faces.push_back(point3(triangleElement[0], triangleElement[1], triangleElement[2]));
		}
		else {
			fprintf(stdout, "buffer[0] is not either 'v' or 'f' \n");
			return 0;
		}
		
		
	}


	averageNormals(vertices, faces, normals);


	Plane temp;

	// store data in mesh
	for (int i = 0; i < faces.size(); i++) {
		//! smf index starts from 1 instead of 0, so need to -1
		Triangle3D facet;

		facet.faces[0] = int(faces[i][0]) - 1;
		facet.faces[1] = int(faces[i][1]) - 1;
		facet.faces[2] = int(faces[i][2]) - 1;

		facet.vertices[0] = vertices[facet.faces[0]];
		facet.vertices[1] = vertices[facet.faces[1]];
		facet.vertices[2] = vertices[facet.faces[2]];

		facet.normals[0] = normals[facet.faces[0]];
		facet.normals[1] = normals[facet.faces[1]];
		facet.normals[2] = normals[facet.faces[2]];
	
		
		temp.set3Points(facet.vertices[0], facet.vertices[2], facet.vertices[1]); // in this case clock wise is normal direction

		facet.triangleNormal = temp.normal;
		facet.centerOfMass = (facet.vertices[0] + facet.vertices[1] + facet.vertices[2]) / 3;
		mesh.push_back(facet);
	}

	return 1;
}
