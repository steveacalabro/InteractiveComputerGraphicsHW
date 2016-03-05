#version 120
#define FLAT 2
attribute vec3 aPosition;
attribute vec3 aNormal;
attribute vec3 aCenterOfMass;
attribute vec3 aFlatNormal;
attribute vec3 aTexCoord;

uniform mat4 u_MVP;
uniform mat4 u_MV;

uniform vec4 u_lightPos;
uniform int u_material;
uniform vec3 u_Color;
uniform int u_shadingModel;

// defined in view space
struct Vertex{
	vec4 position;
	vec4 normal;
};


struct Light{
	vec4 position;

	vec4 diffuse;
	vec4 specular;
	vec4 ambient;
};


struct Material{
	// absobtion coefficients
	vec4 kd;
	vec4 ks;
	vec4 ka;
	float shininess;
};


varying vec4 vLightDir[2];
varying vec4 vNormal;
varying vec4 vViewDir;
varying vec4 vColor;
varying vec3 vTexCoord;

vec4 ambientColor(Light light, Material material);
vec4 diffuseColor(Light light, Material material, Vertex vertex);
vec4 specularColor(Light light, Material material, Vertex vertex);
vec4 GouraudColor(Light light, Material material, Vertex vertex);

void main()
{
	gl_Position = u_MVP * vec4(aPosition, 1.0);
	vec4 normalProjectionSpace = u_MVP * vec4(aNormal, 0.0); // remember to transform normal too

	// coord and normal in view space
	vec4 normalViewSpace = u_MV * vec4(aNormal, 0.0); // remember to transform normal too
	vec4 positionViewSpace = u_MV * vec4(aPosition, 1.0);

	Vertex vertex;
	vertex.position = positionViewSpace;
	vertex.normal = vec4(normalize(normalViewSpace).xyz, 0.0);



	Light light0, light1;

	// light0 is in world space
	//light0.position = u_MV*vec4(-0.5, 2.0, 1.0, 1.0); // transforms to view space
	light0.position = u_MV*u_lightPos; // transforms to view space

	//light0.diffuse = vec4(0.2, 0.9, 0.3, 1.0);
	//light0.specular = vec4(0.2, 0.9, 0.3, 1.0);
	//light0.ambient = vec4(0.2, 0.9, 0.3, 1.0);

	light0.diffuse = vec4(0.8, 0.8, 0.8, 1.0);
	light0.specular = vec4(0.8, 0.8, 0.8, 1.0);
	light0.ambient = vec4(0.8, 0.8, 0.8, 1.0);

	// light1 is in view space
	light1.position = vec4(0.0, 1.0, 2.0, 1.0);
	light1.diffuse = vec4(0.7, 0.5, 0.6, 1.0);
	light1.specular = vec4(0.7, 0.5, 0.6, 1.0);
	light1.ambient = vec4(0.7, 0.5, 0.6, 1.0);

	Material metal, plastic, gold, material;
	// metal material
	//metal.kd = vec4(0.5, 0.5, 0.5, 1.0);
	metal.kd = vec4(0.8,0.8,0.8, 1.0);
	metal.ks = vec4(1.0, 1.0, 1.0, 1.0);
	metal.ka = vec4(0.1, 0.1, 0.1, 1.0);
	metal.shininess =  150.0;

	// plastic material
	plastic.kd = vec4(0.6, 0.1, 0.6, 1.0);
	plastic.ks = vec4(0.6, 0.1, 0.6, 1.0);
	plastic.ka = vec4(0.1, 0.1, 0.6, 1.0);
	plastic.shininess = 5.0;

	// plastic material
	gold.kd = vec4(0.8, 0.8, 0.0, 1.0);
	gold.ks = vec4(0.8, 0.8, 0.0, 1.0);
	gold.ka = vec4(0.1, 0.1, 0.1, 1.0);
	gold.shininess = 50.0;


	// GLSL 120 does not support switch
	//switch (u_material)
	//{
	//	case 0: material = metal;break;
	//	case 1: material = plastic;break;
	//	case 2: material = gold;break;

	//}
	if(u_material == 0){
		material = metal;
	}
	else if(u_material == 1){
		material = plastic;
	}
	else{
		material = gold;
	}

	// Gouraud shading
	vec4 color0 = GouraudColor(light0, material, vertex);
	vec4 color1 = GouraudColor(light1, material, vertex);

	vColor = clamp(color1 + color0, 0.0, 1.0);
	//vColor = clamp(color1, 0.0, 1.0);



	// for Phong shading
	//v_material = u_material;
	vLightDir[0] = light0.position - vertex.position;
	vLightDir[1] = light1.position - vertex.position;
	vNormal = vertex.normal;

	vec4 cameraPos = vec4(0.0, 0.0, 0.0, 1.0); // note this is in view space and w = 1.0 because it is a point
	vViewDir = cameraPos - vertex.position;


	// flat shading
	//! somehow flat shading normal is negative to Gouraud or Phong shading
	if(u_shadingModel == FLAT)
	{
		// coord and normal in view space
		normalViewSpace = u_MV * vec4(aFlatNormal, 0.0); // remember to transform normal too
		positionViewSpace = u_MV * vec4(aCenterOfMass, 1.0);

		vertex.position = positionViewSpace;
		vertex.normal = -vec4(normalize(normalViewSpace).xyz, 0.0);
		
		color0 = diffuseColor(light0, material, vertex);
		color1 = diffuseColor(light1, material, vertex);

		vColor = clamp(color1 + color0, 0.0, 1.0);	
		//vColor = clamp(color1, 0.0, 1.0);	
	}

	// texture coord
	vTexCoord = aTexCoord;
}



vec4 ambientColor(Light light, Material material){

	// ambient = Ia * ka
	vec4 ambient  = light.ambient * material.ka;
	return ambient;
}

vec4 diffuseColor(Light light, Material material, Vertex vertex){

	vec4 lightDir = vec4(normalize((light.position - vertex.position).xyz), 0.0);
	float lightDotNormal = dot(lightDir, vertex.normal);

	// diffuse = Id * kd * cos(theta)
	vec4 diffuse = light.diffuse * material.kd * clamp(lightDotNormal, 0.0, 1.0);

	return diffuse;
}

// Blinn Phong specular
vec4 specularColor(Light light, Material material, Vertex vertex){
	vec4 lightDir = vec4(normalize((light.position - vertex.position).xyz), 0.0);
	vec4 cameraPos = vec4(0.0, 0.0, 0.0, 1.0); // note this is in view space and w = 1.0 because it is a point
	vec4 viewDir = vec4(normalize((cameraPos - vertex.position).xyz), 0.0);
	
	vec4 r = 2*(dot(lightDir, vertex.normal))*vertex.normal - lightDir;
	vec4 halfVector = vec4(normalize((lightDir + viewDir).xyz), 0.0);
	
	vec4 specular = vec4(0.0);

	// if self shadow, specular = 0.0
	if(dot(lightDir, vertex.normal) < 0.0){
		specular = vec4(0.0);
	}
	else{
		// specular = Is * ks * cosPhi^alpha
		float specularShine = pow(max(dot(vertex.normal, halfVector), 0.0), material.shininess); // Blinn Phong specular
		specularShine = pow(max(dot(r, viewDir), 0.0), material.shininess); // Original Phong specular
		specular = light.specular * material.ks * specularShine;
	}
	return specular;
}


vec4 GouraudColor(Light light, Material material, Vertex vertex){
	vec4 ambient = ambientColor(light, material);
	vec4 diffuse = diffuseColor(light, material, vertex);
	vec4 specular = specularColor(light, material, vertex);

	vec4 color = clamp(ambient + diffuse + specular, 0.0, 1.0);
	return color;
}