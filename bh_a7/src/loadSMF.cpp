// HW4
#include "main.h"
#include "BoxGeometry.h"
#include "MeshObject.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <array>
#include "Plane.h"
using namespace std;


void averageNormals(vector<point3> &vertices, vector<point3> &faces, vector<vec3> &normals) {
	// for Gourau shading
	Plane plane;
	vector<vec3> faceNormals;
	for (int i = 0; i < faces.size(); i++) {
		plane.set3Points(vertices[int(faces[i][0]) - 1], vertices[int(faces[i][1]) - 1], vertices[int(faces[i][2]) - 1]);
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


int loadBezir(char* fileName, Mesh &mesh) {
	fstream file(fileName);
	string buffer;

	vector<point3> controlPoints;
	vector<vector<point3>> meshVerts;
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

		// read each element in this line
		stringstream currentLine(buffer);
		string currentColumn;
		vector<double> coord;
		while (getline(currentLine, currentColumn, '\ '))
		{
			coord.push_back(stod(currentColumn)); //! stod converts string to double
		}

		if (coord.size() != 3) {
			fprintf(stdout, "Only read %d elements from current line \n", coord.size());
			return 0;
		}
		else {
			controlPoints.push_back(point3(coord[0], coord[1], coord[2]));
		}
	}

	int resolution = 10;
	constructMesh(controlPoints, resolution, mesh);

	return 1;
}


void constructMesh(const vector<point3> &controlPoints, int resolution, Mesh &mesh) {
	vector<vector<point3>> meshVerts;
	vector<point3> vertices;
	vector<vec3>   normals;
	vector<point3> faces;

	meshVerts = interpolateBezirPatch(controlPoints, resolution);

	tessellate(meshVerts, vertices, faces, normals);

	// export to smf file
	string outputSMF = "..\\src\\resources\\bezierPatch.obj";
	exportSMF(outputSMF, vertices, faces);


	// calculate the average normals for shanding
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

}



vector<vector<point3>> interpolateBezirPatch(const vector<point3> &controlPoints, const int &resolution) {

	
	vector<vector<point3>> patchVerts;

	double stepSize = 1.0 / resolution;
	double u = 0;
	

	vec4 bu, bv;

	while(u < 1.0) {
		vector<point3> curveVerts;
		// blending function b(u)
		bu[0] = pow(1-u, 3);
		bu[1] = 3*u*pow(1-u, 2);
		bu[2] = 3*u*u*(1-u);
		bu[3] = pow(u, 3);

		double v = 0;
		while (v < 1.0) {
			point3 interpolatedPoint;
			// blending function b(v)
			bv[0] = pow(1 - v, 3);
			bv[1] = 3 * v*pow(1 - v, 2);
			bv[2] = 3 * v*v*(1 - v);
			bv[3] = pow(v, 3);


			// interpolate the points using control points 
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					interpolatedPoint.x += bu[i] * bv[j] * controlPoints[4 * i + j].x;
					interpolatedPoint.y += bu[i] * bv[j] * controlPoints[4 * i + j].y;
					interpolatedPoint.z += bu[i] * bv[j] * controlPoints[4 * i + j].z;
				}
			}			

			curveVerts.push_back(interpolatedPoint);
			//interpolatedPoint = point3(0.0, 0.0, 0.0);

			v += stepSize;
			if (v > 1.0) {
				v = 1.0;
			}
		}

		patchVerts.push_back(curveVerts);

		u += stepSize;
		if (u > 1.0) {
			u = 1.0;
		}
		
	}

	return patchVerts;
	
}

void tessellate(const vector<vector<point3>> &patchPoints, vector<point3> &vertices, vector<point3> &faces, vector<vec3> &normals) {

	int uLength = patchPoints.size();
	int vLength = patchPoints[0].size();
	for (int u = 0; u < uLength; u++) {
		for (int v = 0; v < vLength; v++) {
			vertices.push_back(patchPoints[u][v]);
			normals.push_back(vec3(0.0));

			if (u < uLength - 1 && v < vLength - 1) {
				//! smf index starts from 1 instead of 0, so need to +1

				// counter counter clock wise, east triangle
				faces.push_back(point3(u*vLength + v + 1, (u + 1)*vLength + v + 1, (u + 1)*vLength + v + 1 + 1));

				// counter clock wise, north triangle
				faces.push_back(point3(u*vLength + v + 1, (u + 1)*vLength + v + 1 + 1, u *vLength + v + 1 + 1));
			}
			
		}
	}
	
}


void exportSMF(string fileName, const vector<point3> &vertices, const vector<point3> &faces) {
	
	ofstream File;
	File.open(fileName);

	File << "# Exported SMF by Bingyao Huang \n" << endl;


	for (int i = 0; i < vertices.size(); i++) //Write vertices
	{
		File << "v" << "  " << vertices[i].x << " " << vertices[i].y << " " << vertices[i].z << endl;
	}
	
	for (int i = 0; i < faces.size(); i++) //Write vertices
	{
		File << "f" << "  " << faces[i].x << " " << faces[i].y << " " << faces[i].z << endl;
	}
	

	File.close();

}