#version 120
#define GOURAUD 0
#define PHONG 1


#define CYLINDER 2
#define PLANE 3
varying vec4 vColor;
varying vec4 vLightDir[2];
varying vec4 vNormal;
varying vec4 vViewDir;
varying vec3 vTexCoord;
//varying int v_material;
uniform int u_shadingModel;
uniform int u_material;
uniform vec3 u_Color;
uniform int u_texMappingModel;

uniform sampler2D texture; //texture object id from application
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

float mirrorEffect(vec3 normal){
	float mirrorX = mod(asin(normal.x)*pow(normal.z, normal.y), fract(tan(pow(normal.z, normal.x))));
	float mirrorY = mod(cos(normal.y)*pow(normal.z, normal.x), fract(tan(pow(normal.y, normal.x))));

	float mirrorZ = mod(atan(normal.z)*pow(normal.y, normal.x), fract(tan(pow(normal.z, normal.y))));
	return  mirrorX*mirrorY*mirrorZ;
}

float turbulence(vec3 normal){
	float value = 0;
	float frequency = 30.0;
	value = abs(mirrorEffect(normal))/frequency;		
	return value*1000;
}

void main()
{

	vec4 fragColor;

	if(u_shadingModel == PHONG){
		// fragment
		Fragment fragment;
		fragment.normal = vec4(normalize(vNormal.xyz), 0.0);

		// lights
		Light light0, light1;
		light0.direction = vec4(normalize(vLightDir[0]).xyz, 0.0); 

		// light0
		light0.diffuse = vec4(0.8, 0.8, 0.8, 1.0);
		light0.specular = vec4(1.0, 1.0, 1.0, 1.0);
		light0.ambient = vec4(0.8, 0.8, 0.8, 1.0);

		// light1 is in view space
		light1.direction = vec4(normalize(vLightDir[1]).xyz, 0.0); 
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

		// GLSL 120 does not support switch, if you use 140 or above, uncomment this switch and comment the ifs

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
		vec4 color0 = PhongColor(light0, material, fragment);
		vec4 color1 = PhongColor(light1, material, fragment);

		fragColor = clamp(color1 + color0, 0.0, 1.0);

	}
	else{
		fragColor = vColor;
	}


	/******************   Texture sampling *******************/
	vec2 texUV;
	// plane texture mapping, project to X-Y plane
	vec2 planeCoord = vec2(vTexCoord.x, vTexCoord.y);

	//vec2 cylinderCoord = vec2(0.5/3.14*acos(2*3.14*vTexCoord.x) + 0.5/3.14*asin(2*3.14*vTexCoord.z), vTexCoord.y);

	// cylinderal texture mapping, circile is on X-Z plane and y is longitude
	vec2 cylinderCoord = vec2(atan(vTexCoord.z/vTexCoord.x), vTexCoord.y);


	if(u_texMappingModel == CYLINDER){
		texUV = cylinderCoord;
	}
	else if(u_texMappingModel == PLANE){
		texUV = planeCoord;
	}
	vec3 textureColor = texture2D(texture, texUV).rgb;

	gl_FragColor = vec4(fragColor.rgb*textureColor, 1.0);

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

	// add some turbulence on diffuse
	float noise = turbulence(fragment.normal.xyz);
	return diffuse*noise;
}

// Blinn Phong specular
vec4 specularColor(Light light, Material material, Fragment fragment){
	vec4 lightDir = light.direction;
	vec4 viewDir = vec4(normalize(vViewDir.xyz), 0.0);
	vec4 r = 2*(dot(lightDir, fragment.normal))*fragment.normal - lightDir;
	vec4 halfVector = vec4(normalize((lightDir + viewDir).xyz), 0.0);
	vec4 specular = vec4(0.0);

	// if self shadow, specular = 0.0
	if(dot(lightDir, fragment.normal) < 0.0){
		specular = vec4(0.0);
	}
	else{
		// specular = Is * ks * cosPhi^alpha
		float specularShine = pow(max(dot(fragment.normal, halfVector), 0.0), material.shininess); // Blinn Phong specular
				//specularShine = pow(max(dot(r, viewDir), 0.0), material.shininess);  // original Phong specular
		specular = light.specular * material.ks * specularShine;
	}
	return specular;
}


vec4 PhongColor(Light light, Material material, Fragment fragment){
	vec4 ambient = ambientColor(light, material);
	vec4 diffuse = diffuseColor(light, material, fragment);
	vec4 specular = specularColor(light, material, fragment);

	vec4 color = clamp(ambient  + diffuse + specular, 0.0, 1.0);
	return color;
}