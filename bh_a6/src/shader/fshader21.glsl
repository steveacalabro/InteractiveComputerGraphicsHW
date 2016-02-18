#version 130
#define GOURAUD 0
#define PHONG 1
varying vec4 vColor;
varying vec4 vLightDir[2];
varying vec4 vNormal;
varying vec4 vViewDir;
//varying int v_material;
uniform int u_shadingModel;
uniform int u_material;
// defined in view space

struct Fragment{
	vec4 normal;
};

struct Light{
	vec4 direction;
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

vec4 ambientColor(Light light, Material material);
vec4 diffuseColor(Light light, Material material);
vec4 specularColor(Light light, Material material);
vec4 PhongColor(Light light, Material material, Fragment fragment);
void main()
{

	vec4 fragColor;

	if(u_shadingModel == PHONG){
		// fragment
		Fragment fragment;
		fragment.normal = normalize(vNormal);

		// lights
		Light light0, light1;
		light0.direction = normalize(vLightDir[0]); 

		light0.diffuse = vec4(0.8, 0.8, 0.8, 1.0);
		light0.specular = vec4(0.8, 0.8, 0.8, 1.0);
		light0.ambient = vec4(0.8, 0.8, 0.8, 1.0);

		// light1 is in view space
		light1.direction = vLightDir[1]; 
		light1.diffuse = vec4(0.7, 0.5, 0.6, 1.0);
		light1.specular = vec4(0.7, 0.5, 0.6, 1.0);
		light1.ambient = vec4(0.7, 0.5, 0.6, 1.0);

		Material metal, plastic, gold, material;
		// metal material
		metal.kd = vec4(0.5, 0.5, 0.5, 1.0);
		metal.ks = vec4(1.0, 1.0, 1.0, 1.0);
		metal.ka = vec4(0.1, 0.1, 0.1, 1.0);
		metal.shininess =  300.0;

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
		vec4 color0 = PhongColor(light0, material, fragment);
		vec4 color1 = PhongColor(light1, material, fragment);

		fragColor = clamp(color1 + color0, 0.0, 1.0);
	}
	else{
		fragColor = vColor;
	}

	gl_FragColor = fragColor;

}



vec4 ambientColor(Light light, Material material){

	// ambient = Ia * ka
	vec4 ambient  = light.ambient * material.ka;
	return ambient;
}

vec4 diffuseColor(Light light, Material material, Fragment fragment){

	vec4 lightDir = light.direction;
	float lightDotNormal = dot(lightDir, fragment.normal);

	// diffuse = Id * kd * cos(theta)
	vec4 diffuse = light.diffuse * material.kd * clamp(lightDotNormal, 0.0, 1.0);

	return diffuse;
}

// Blinn Phong specular
vec4 specularColor(Light light, Material material, Fragment fragment){
	vec4 lightDir = light.direction;
	vec4 viewDir = normalize(vViewDir);
	vec4 halfVector = normalize( lightDir + viewDir );
	vec4 specular = vec4(0.0);

	// if self shadow, specular = 0.0
	if(dot(lightDir, fragment.normal) < 0.0){
		specular = vec4(0.0);
	}
	else{
		// specular = Is * ks * cosPhi^alpha
		float specularShine = pow(max(dot(fragment.normal, halfVector), 0.0), material.shininess);
		specular = light.specular * material.ks * specularShine;
	}
	return specular;
}


vec4 PhongColor(Light light, Material material, Fragment fragment){
	vec4 ambient = ambientColor(light, material);
	vec4 diffuse = diffuseColor(light, material, fragment);
	vec4 specular = specularColor(light, material, fragment);

	vec4 color = clamp(ambient + diffuse + specular, 0.0, 1.0);
	return color;
}