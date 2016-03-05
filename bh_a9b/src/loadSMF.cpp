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
	vector<vec3> texCoord;


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
			fprintf(stdout, "Only read %d elements from current line \n", static_cast<int>(triangleElement.size()));
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
	generateTexCoord(vertices, texCoord);

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
		
		facet.texCoord[0] = texCoord[facet.faces[0]];
		facet.texCoord[1] = texCoord[facet.faces[1]];
		facet.texCoord[2] = texCoord[facet.faces[2]];
		
		temp.set3Points(facet.vertices[0], facet.vertices[2], facet.vertices[1]); // in this case clock wise is normal direction

		facet.triangleNormal = temp.normal;
		facet.centerOfMass = (facet.vertices[0] + facet.vertices[1] + facet.vertices[2]) / 3;
		mesh.push_back(facet);
	}

	if (mesh.size() == 0) {
		cout << "Mesh size is 0 \n";
		return 0;
	}
	return 1;
}


int loadBezir(char* fileName, Mesh &mesh, vector<point3> &controlPoints) {
	fstream file(fileName);
	string buffer;

	//vector<point3> controlPoints;
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
			fprintf(stdout, "Only read %d elements from current line \n", static_cast<int>(coord.size()));
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






vector<vector<point3>> interpolateBezirPatch(const vector<point3> &controlPoints, const int &resolution) {

	
	vector<vector<point3>> patchVerts;

	double stepSize = 1.0 / resolution;
	double u = 0;
	

	vec4 bu, bv;

	while(u <= 1.0) {
		vector<point3> curveVerts;
		// blending function b(u)
		bu[0] = pow(1-u, 3);
		bu[1] = 3*u*pow(1-u, 2);
		bu[2] = 3*u*u*(1-u);
		bu[3] = pow(u, 3);

		double v = 0;
		while (v <= 1.0) {
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


			if (abs(v - 1.0) < 0.001) {
				break;
			}

			v += stepSize;
			if (v > 1.0) {
				v = 1.0;
			}
		}

		patchVerts.push_back(curveVerts);

		if (abs(u - 1.0) < 0.001) {
			break;
		}

		u += stepSize;
		if (u > 1.0) {
			u = 1.0;
		}
		
	}

	return patchVerts;
	
}



void getMax(vector<point3> &vertices, double &x, double &y, double &z)
{
	double maxX = -1000;
	double maxY = -1000;
	double maxZ = -1000;
	
	for (int i = 0; i < vertices.size(); i++)
	{
		if (vertices[i].x > maxX)
		{
			maxX = vertices[i].x;
		}
		if (vertices[i].y > maxY)
		{
			maxY = vertices[i].y;
		}
		if (vertices[i].z > maxZ)
		{
			maxZ = vertices[i].z;
		}
	}

	x = maxX;
	y = maxY;
	z = maxZ;

}

void getMin(vector<point3> &vertices, double &x, double &y, double &z)
{
	double minX = 1000;
	double minY = 1000;
	double minZ = 1000;

	for (int i = 0; i < vertices.size(); i++)
	{
		if (vertices[i].x < minX)
		{
			minX = vertices[i].x;
		}
		if (vertices[i].y < minY)
		{
			minY = vertices[i].y;
		}
		if (vertices[i].z < minZ)
		{
			minZ = vertices[i].z;
		}
	}

	x = minX;
	y = minY;
	z = minZ;

}

void generateTexCoord(vector<point3> &vertices, vector<vec3> &texCoord)
{
	double minX, minY, minZ, maxX, maxY, maxZ;
	getMax(vertices, maxX, maxY, maxZ);
	getMin(vertices, minX, minY, minZ);


	for (int i = 0; i < vertices.size(); i++)
	{
		// normalize to 0~1
		double x = (vertices[i].x - minX) / (maxX - minX);
		double y = (vertices[i].y - minY) / (maxY - minY);
		double z = (vertices[i].z - minZ) / (maxZ - minZ);

		// convert to -1/2/PI ~ 1/2/PI
		double PI = 3.14;
		x = (x - 0.5 / PI);
		z = (z - 0.5 / PI);
		texCoord.push_back(point3(x,y,z));
	}
}



void tessellate(const vector<vector<point3>> &patchPoints, vector<point3> &vertices, vector<point3> &faces, vector<vec3> &normals, vector<vec2> &texCoord) {

	int uLength = patchPoints.size();
	int vLength = patchPoints[0].size();
	for (int u = 0; u < uLength; u++) {
		for (int v = 0; v < vLength; v++) {
			vertices.push_back(patchPoints[u][v]);
			normals.push_back(vec3(0.0));
			texCoord.push_back(vec2(u / float(uLength), v/ float(vLength)));

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

void constructMesh(const vector<point3> &controlPoints, int resolution, Mesh &mesh) {
	vector<vector<point3>> meshVerts;
	vector<point3> vertices;
	vector<vec3>   normals;
	vector<point3> faces;
	vector<vec2> texCoord;

	meshVerts = interpolateBezirPatch(controlPoints, resolution);

	tessellate(meshVerts, vertices, faces, normals, texCoord);


	// calculate the average normals for shanding
	averageNormals(vertices, faces, normals);

	// export to smf file
	// string outputSMF = "..\\src\\resources\\bezierPatch.obj";
	// exportSMF(outputSMF, vertices, faces, normals);


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


		//facet.texCoord[0] = texCoord[facet.faces[0]];
		//facet.texCoord[1] = texCoord[facet.faces[1]];
		//facet.texCoord[2] = texCoord[facet.faces[2]];

		temp.set3Points(facet.vertices[0], facet.vertices[1], facet.vertices[2]); // in this case clock wise is normal direction

		facet.triangleNormal = temp.normal;
		facet.centerOfMass = (facet.vertices[0] + facet.vertices[1] + facet.vertices[2]) / 3;
		mesh.push_back(facet);
	}

}


int loadImage(char* fileName, Image &image) {
	fstream file(fileName);
	string buffer;

	//Image image;
	// if not opened
	if (!file.is_open())
	{
		cout << "Unable to open file" << endl;
		return 0;
	}

	// read file line by line
	int row = 0; 
	int col = 0;
	while (getline(file, buffer, '\n'))
	{

		// read each element in this line
		stringstream currentLine(buffer);
		string currentColumn;
		vector<color3Byte> temp;
		vector<int> pixel;
		int channel = 0;
		while (getline(currentLine, currentColumn, ' '))
		{
			pixel.push_back(stoi(currentColumn)); //! stod converts string to int
			channel++;
			if(channel == 3)
			{
				temp.push_back(color3Byte(pixel[0], pixel[1], pixel[2]));
				col++;
				pixel.clear();
				channel = 0;
			}
		}

		image.push_back(temp);
		row++;

		//if (pixel.size() != 3) {
		//	fprintf(stdout, "Only read %d channels from current pixel \n", static_cast<int>(pixel.size()));
		//	return 0;
		//}
	}

	return 1;
	
}



void exportSMF(string fileName, const vector<point3> &vertices, const vector<point3> &faces, const vector<point3> &normals) {
	
	ofstream File;
	File.open(fileName);

	File << "# Exported SMF by Bingyao Huang \n" << endl;


	for (int i = 0; i < vertices.size(); i++) //Write vertices
	{
		File << "v" << "  " << vertices[i].x << " " << vertices[i].y << " " << vertices[i].z << endl;
	}

	for (int i = 0; i < normals.size(); i++) //Write normals
	{
		File << "n" << "  " << normals[i].x << " " << normals[i].y << " " << normals[i].z << endl;
	}
	
	for (int i = 0; i < faces.size(); i++) //Write faces
	{
		File << "f" << "  " << faces[i].x << " " << faces[i].y << " " << faces[i].z << endl;
	}
	

	File.close();

}