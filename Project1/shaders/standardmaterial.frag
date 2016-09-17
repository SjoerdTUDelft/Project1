#version 400

uniform sampler2D ourTexture1;
uniform float fov;
uniform float B;
uniform mat4 V;
#define M_PI 3.1415926535897932384626433832795


struct PointLight {
	vec3 position;
	float intensity;
};

struct DirectionalLight {
	vec3 direction;
	float intensity;
};

struct SpotLight {
	vec3 position;
	vec3 direction;
	float intensity;
	float outerAngle;
	float innerAngle;
};


#define POINTLIGHTS 4
#define DIRECTIONALLIGHTS 1
#define SPOTLIGHTS 1

uniform	PointLight pointLights[POINTLIGHTS];
uniform DirectionalLight directionalLights[DIRECTIONALLIGHTS];
uniform	SpotLight spotLights[SPOTLIGHTS];

in vec2 texCoord;
in vec3 viewNormals;
in vec3 viewPosition;
in vec3 lightViewPosition;

out vec4 frag_colour;
 
vec3 PBR(vec3 vCamera, vec3 vLight, vec3 vNorm, float roughness) {
	
	vec3 vHalf = normalize(vLight + vCamera);
	float LoN = clamp(dot(vLight, vNorm),0.0,1.0);
	float VoN = clamp(dot(vCamera,vNorm),0.0,1.0);
	float LoH = clamp(dot(vCamera, vHalf),0.0,1.0);
	float NoH = clamp(dot(vNorm,  vHalf),0.0,1.0);
 
	//GGX
	float rough2 = roughness * roughness;
	float alphaSqr = rough2*rough2;
	float den = (NoH*NoH)*(alphaSqr-1.0)+1.0;
	float GGX = alphaSqr/(M_PI * den * den);

	//Schlick
	vec3 FCopper = vec3(0.95,0.64,0.54);
	vec3 FSCopper = vec3(0.98,0.82,0.76);
	vec3 FDiamond = vec3(0.17,0.17,0.17);
	vec3 FAluminium = vec3(1.0,1.0,1.0);
		
	float ind = pow(1.0-LoH,5.0);
	vec3 Schlick = vec3(ind + (1.0-ind));

	//Schlick-Smith
	float k = rough2;
	float Vis_Smith = 0.5/((VoN* (VoN*(1.0-k) + k)) + (LoN*(LoN*(1.0-k) + k)));
	
	return GGX*Schlick*Vis_Smith;
}


void main () {

	//Standard Variables
	float roughness = fov * 0.8 + 0.1;
	vec3 vNorm = normalize(viewNormals);
	vec3 vCamera = normalize(-viewPosition);
	vec3 Diffuse = texture2D(ourTexture1,texCoord).xyz;	
	vec3 endcol;

	
	for(int i = 0; i < POINTLIGHTS; i++) {
		//Direction of Light
		vec3 relative =   (V * vec4(pointLights[i].position ,0.0)).xyz - viewPosition;
		vec3 vLight = normalize(relative);

		//Distance for Intensity of light
		//distance = pow(1.+length(relative)/2.0,-2.0);
		float distance = 1.0/length(relative);
		
		vec3 result = PBR(vCamera,vLight, vNorm, roughness);
		float LoN = clamp(dot(vLight, vNorm),0.0,1.0);

		endcol += vec3(( Diffuse + result)*distance*LoN* pointLights[i].intensity);
	}

	for(int i = 0; i < DIRECTIONALLIGHTS; i++) {
		//Direction of Light
		vec3 relative = (V * vec4(-directionalLights[i].direction ,0.0)).xyz;
		vec3 vLight = normalize(relative);
		
		vec3 result = PBR(vCamera,vLight, vNorm, roughness);
		float LoN = clamp(dot(vLight, vNorm),0.0,1.0);

		endcol += vec3(( Diffuse + result )*LoN*directionalLights[i].intensity);
	}

	for(int i = 0; i < SPOTLIGHTS; i++) {
		//Direction of Light
		vec3 relative = spotLights[i].direction;
		vec3 vLight = normalize(relative);

		vec3 result = PBR(vCamera,vLight, vNorm, roughness);
		float LoN = clamp(dot(vLight, vNorm),0.0,1.0);

		//Intensity Light due to it's angle
		float degree = spotLights[i].outerAngle;
		float outerAngle = cos(degree * M_PI/180);
		degree = spotLights[i].innerAngle;
		float innerAngle = cos(degree * M_PI/180);
		float fallof = dot(vLight,vCamera);

		if( fallof > outerAngle) {
			float radius = innerAngle - outerAngle;
			float distance = (fallof - outerAngle)/radius;
			endcol += vec3(( Diffuse + result )*LoN*distance*spotLights[i].intensity);
		}
	}

	//Compositing
	//vec3 endcol = vec3(( Diffuse + GGX*Schlick*Vis_Smith)*distance*LoN* lightStrength);
	frag_colour = vec4(endcol,1.0);
}