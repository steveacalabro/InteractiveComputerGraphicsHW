#version 330
attribute vec3 aPosition;
attribute vec3 aNormal;
attribute vec3 aCenterOfMass;

uniform mat4 u_MVP;
uniform mat4 u_MV;

uniform vec4 u_lightPos;
uniform int u_material;


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
varying int v_material;
varying vec4 vColor;

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
	vertex.normal = normalViewSpace;



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
	light1.position = vec4(0.0, 1.0, -2.0, 1.0);
	light1.diffuse = vec4(0.7, 0.5, 0.6, 1.0);
	light1.specular = vec4(0.7, 0.5, 0.6, 1.0);
	light1.ambient = vec4(0.7, 0.5, 0.6, 1.0);

	Material metal, plastic, gold, material;
	// metal material
	metal.kd = vec4(0.5, 0.5, 0.5, 1.0);
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



	switch (u_material)
	{
		case 0: material = metal;break;
		case 1: material = plastic;break;
		case 2: material = gold;break;

	}

	// Gouraud shading
	vec4 color0 = GouraudColor(light0, material, vertex);
	vec4 color1 = GouraudColor(light1, material, vertex);

	vColor = clamp(color1 + color0, 0.0, 1.0);



	// for Phong shading
	v_material = u_material;
	vLightDir[0] = light0.position - vertex.position;
	vLightDir[1] = light1.position - vertex.position;
	vNormal = vertex.normal;

	vec4 cameraPos = vec4(0.0); // note this is in view space
	vViewDir = cameraPos - vertex.position;


}



vec4 ambientColor(Light light, Material material){

	// ambient = Ia * ka
	vec4 ambient  = light.ambient * material.ka;
	return ambient;
}

vec4 diffuseColor(Light light, Material material, Vertex vertex){

	vec4 lightDir = normalize(light.position - vertex.position);
	float lightDotNormal = dot(lightDir, vertex.normal);

	// diffuse = Id * kd * cos(theta)
	vec4 diffuse = light.diffuse * material.kd * clamp(lightDotNormal, 0.0, 1.0);

	return diffuse;
}

// Blinn Phong specular
vec4 specularColor(Light light, Material material, Vertex vertex){
	vec4 lightDir = normalize(light.position - vertex.position);
	vec4 cameraPos = vec4(0.0); // note this is in view space
	vec4 viewDir = normalize(cameraPos - vertex.position);
	vec4 halfVector = normalize( lightDir + viewDir );
	vec4 specular = vec4(0.0);

	// if self shadow, specular = 0.0
	if(dot(lightDir, vertex.normal) < 0.0){
		specular = vec4(0.0);
	}
	else{
		// specular = Is * ks * cosPhi^alpha
		float specularShine = pow(max(dot(vertex.normal, halfVector), 0.0), material.shininess);
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