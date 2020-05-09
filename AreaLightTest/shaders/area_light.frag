#version 450 core
#pragma debug(on)
#pragma optimize(off)

#define PI 3.14159265359 
#define INV_PI 0.3183099
#define MAX_SCENE_LIGHTS 10

#define EPSILON 0.000001

const int Power = 0;
const int Intensity = 1; // power/sr
const int Illuminance = 2; // power /area
const int Luminance = 3; // power /(sr * area)

const int Point = 0;
const int Spot = 1;
const int Direct = 2;
const int Sun = 3;
const int AreaSphere = 4;
const int AreaDisk = 5;
const int AreaRectangle = 6;

const int SPHERE_SHAPE = 1000 << 0;
const int DISK_SHAPE = 1000 << 1;
const int RECTANGLE_SHAPE = 1000 << 2;
const int TUBE_SHAPE = 1000 << 3;

struct Stuff{
	sampler2D stuff;
};

struct Sphere{
	float radius;
	vec3 center;
};

struct Disk{
	float radius;
	vec3 center;
};

struct Rectangle{
	float width, height;
};

struct Tube{
	float height;
	float radius;
	vec3 center;
};

struct Light{
	int unit;
	int type;
	int shapeId;
	float value;
	vec3 position;
	vec3 color;
	float radius;
	vec3 normal;
	mat4 localToWorld;
};

in VERTEX{
	smooth vec3 position;
	smooth vec4 color;
	smooth vec3 normal;
	smooth vec3 tangent;
    smooth vec3 bitangent;
	smooth vec2 uv;
} vertex_in;

uniform Light light;
uniform vec3 eyes;
uniform float roughness = 0.5;
uniform vec3 lightNormal;
out vec4 fragColor;
uniform Sphere SphereLights[10];
uniform Disk diskLights[10];
uniform Rectangle rectangleLights[10];
uniform Tube tubeLights[10];

float Area(Sphere s){
	return 4 * s.radius * s.radius * PI;
}

float Area(Disk d){
	return d.radius * d.radius * PI;
}

float Area(Rectangle rect){
	return rect.width * rect.height;
}

float Area(Tube tube){
	float r = tube.radius;
	float h = tube.height;
	return 2 * PI * r * (h + 2 * r);
}

float Saturate(float x){
	return clamp(x, 0.0, 1.0);
}

float illuminanceSphereOrDisk(float cosTheta, float sinSigmaSqr){
	
	float sinTheta = sqrt(1.0f - cosTheta * cosTheta);
	float illuminance = 0.0f;
	if(cosTheta * cosTheta > sinSigmaSqr){
		illuminance = PI * sinSigmaSqr * Saturate(cosTheta);
	}else{
		float x = sqrt(1.0 / sinSigmaSqr - 1.0f);
		float y = -x * (cosTheta / sinTheta);
		float sinThetaSqrtY = sinTheta * sqrt(1.0f - y * y);
		illuminance = ( cosTheta * acos (y) - x * sinThetaSqrtY ) * sinSigmaSqr + atan (sinThetaSqrtY / x);
	}

	return max(illuminance, 0.0);
}

vec3 getSpecularDominantDirArea(vec3 N, vec3 R, float roughness){
	float lerpFactor = (1 - roughness);
	return normalize(mix(N, R, lerpFactor));
}

float DistributionGGX(vec3 N, vec3 H, float a){
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float nom    = a2;
    float denom  = (NdotH2 * (a2 - 1.0) + 1.0);
    denom        = PI * denom * denom;
	
    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float k){
    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return nom / denom;
}
  
float GeometrySmith(vec3 N, vec3 V, vec3 L, float k){
    float NdotV = Saturate(dot(N, V));
    float NdotL = Saturate(dot(N, L));
    float ggx1 = GeometrySchlickGGX(NdotV, k);
    float ggx2 = GeometrySchlickGGX(NdotL, k);
	
    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0){
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness){
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
} 

struct LightPart{
	vec3 diffuse;
	vec3 specular;
};

struct Lighting{
	LightPart direct;
	LightPart indirect;
};

struct Surface{
	vec3 position;
	vec3 normal;
	vec3 color;
	float roughness;
};

float TraceSphere(vec3 p, vec3 d, Sphere s){
	float t = -1;

	vec3 m = p - s.center;
	float b = dot(m, d);
	float c = dot(m, m) - s.radius * s.radius;

	float discr = b*b - c;

	t = -b - sqrt(discr);

	return t;
}

vec3 ClosestPointOnSphere(Sphere s, vec3 p) {
	vec3 d = p - s.center;
	float t = length(d) - s.radius;
	return p + t * d;
}

float TraceTube(vec3 o, vec3 dir, vec3 p, vec3 q, float r){
	float t = -1;

	vec3 d = q - p;
	vec3 m = o - p;
	vec3 n = dir;

	float md = dot(m, d);
	float nd = dot(n, d);
	float dd = dot(d, d);

	float nn = dot(n, n);
	float mn = dot(m, n);
	float a = dd * nn - nd * nd;
	float k = dot(m, m) - r * r;
	float c = dd * k - md * md;
	if(abs(a) < 0){
		if(md < 0 ){
			if(c > 0.0) return -1;
			t = -mn / nn;	// change to sphere test on p side;
		}else if(md > dd) {
			t = (nd - mn) / nn;	// change to sphere test on q side;
		}else{
			t = 0;	// we are inside cylinder, probably not going to happen
		}
		return 1;
	}
	float b = dd * mn - nd * md;
	float discr = b * b - a * c;
	t = (-b - sqrt(discr))/a;

	if(t < 0 || t > 1) return t;	// intersection lies outside segment

	if(md + t * nd < 0){
		// Intersection outside on side p
		if( nd <= 0) return -1; // pointing way from endcap 
		t = -md / nd;	// change to sphere test on p side
		return -1;
	}else if (md + t * nd > dd) {
		if(nd >= 0) return -1;	// point away from endcap
		// Intersection from q side
		t = (dd - md) / nd; // change to sphere test on p side
		return -1;
	}

	return t; 
}

void SphereLight(Light light, Surface surface, vec3 eyes, inout Lighting lighting){
	vec3 worldPos = surface.position;
	vec3 lightPos = (light.localToWorld * vec4(light.position, 1)).xyz;

	vec3 lightDir = lightPos - worldPos;
	Sphere sphere = SphereLights[light.shapeId & ~SPHERE_SHAPE];

	vec3 L = normalize(lightDir);
	vec3 N = normalize(gl_FrontFacing ? surface.normal : -surface.normal);
	vec3 V = normalize(eyes - worldPos);
	vec3 r = -reflect(V, N);
	vec3 H = normalize(L+V);
	float roughness = surface.roughness;

	float NdotL = dot(N, L);
	vec3 color = surface.color;

	float sqrDist = dot(lightDir, lightDir);
	float cosTheta = clamp(NdotL, -0.999, 0.999);
	float sqrLightRadius = pow(sphere.radius, 2);
	float sinSigmaSqr = min(sqrLightRadius / sqrDist, 0.9999f);

	float illuminance = illuminanceSphereOrDisk(cosTheta, sinSigmaSqr);

	

	float intensity = light.value/Area(sphere) * INV_PI;
	vec3 radiance = light.color * intensity * illuminance;


	r = getSpecularDominantDirArea(N, r, roughness);

	float t = TraceSphere(worldPos, r, sphere);
	if(t >= 0 && t <= 1){
		L = r;
	}else{
		vec3 p = worldPos + Saturate(t) * r;
		ClosestPointOnSphere(sphere, p);
	}

	float NDF = DistributionGGX(N, H, roughness);
	float k = (roughness * roughness)/8;
	float G = GeometrySmith(N, V, L, k);
	vec3 F = fresnelSchlick(Saturate(dot(H, V)), vec3(0.04));
	vec3 Ks = F;
	vec3 Kd = 1 - Ks;

	vec3 specular = (NDF * G * F)/(4.0 * Saturate(dot(N, V)) * Saturate(dot(N, L)));
	
	lighting.direct.diffuse += max(vec3(0), color/PI * radiance * Saturate(NdotL) * Kd);
	lighting.direct.specular += max(vec3(0), specular * radiance * Saturate(NdotL));
}

float TracePlane(vec3 o, vec3 d, vec3 po, vec3 pn){
	float t = dot(po, pn) - dot(pn, o);
	return t/dot(pn, d);
}

void DiskLight(Light light, Surface surface, vec3 eyes, inout Lighting lighting){
	vec3 worldPos = surface.position;
	vec3 lightPos = (light.localToWorld * vec4(light.position, 1)).xyz;

	vec3 lightDir = lightPos - worldPos;
	Disk disk = diskLights[light.shapeId & ~DISK_SHAPE];

	vec3 L = normalize(lightDir);
	vec3 N = normalize(gl_FrontFacing ? surface.normal : -surface.normal);
	vec3 V = normalize(eyes - worldPos);
	
	vec3 H = normalize(L+V);
	vec3 Na = normalize(transpose(inverse(mat3(light.localToWorld))) * light.normal);
	float roughness = surface.roughness;

	float sqrDist = dot(lightDir, lightDir);
	float NdotL = dot(N, L);
	float cosTheta = NdotL;
	float sqrLightRadius = pow(disk.radius, 2);
	float sinSigmaSqr = sqrLightRadius / (sqrLightRadius + max(sqrLightRadius, sqrDist));

	float illuminance = illuminanceSphereOrDisk(cosTheta, sinSigmaSqr) * Saturate(dot(Na, -L));
	float intensity = light.value/Area(disk) * INV_PI;
	vec3 radiance = light.color * intensity * illuminance;

	vec3 R = -reflect(V, N);
	R = getSpecularDominantDirArea(N, R, roughness);

	float specularAttenuation = Saturate(abs(dot(Na, R)));

	if(specularAttenuation > 0){
		float t = TracePlane(worldPos, R, lightPos, Na);

		vec3 p = worldPos + R * Saturate(t);

		if(distance(p, lightPos) < disk.radius){
			L = R;
		}
		else{
			Sphere s;
			s.radius = disk.radius;
			s.center = disk.center;
			lightPos = ClosestPointOnSphere(s, p);
			L = normalize(lightPos - worldPos);
		}

	}

	float NDF = DistributionGGX(N, H, roughness);
	float k = (roughness * roughness)/8;
	float G = GeometrySmith(N, V, L, k);
	vec3 F = fresnelSchlick(Saturate(dot(H, V)), vec3(0.04));
	vec3 Ks = F;
	vec3 Kd = 1 - Ks;

	vec3 specular = (NDF * G * F)/(4.0 * Saturate(dot(N, V)) * Saturate(dot(N, L)));
	
	vec3 color = surface.color;
	lighting.direct.diffuse += max(vec3(0), color/PI * radiance * Saturate(NdotL) * Kd);
	lighting.direct.specular += max(vec3(0), specularAttenuation * specular * radiance * Saturate(NdotL));

}

float RightPyramidSolidAngle(float dist, float halfWidth, float halfHeight){
	float a = halfWidth;
	float b = halfHeight;
	float h = dist;

	return 4 * asin(a * b / sqrt((a * a + h * h) * (b * b + h * h)));
}

float RectangleSolidAngle(vec3 worldPos, vec3 p0, vec3 p1, vec3 p2, vec3 p3){
	vec3 v0 = p0 - worldPos;
	vec3 v1 = p1 - worldPos;
	vec3 v2 = p2 - worldPos;
	vec3 v3 = p3 - worldPos;

	vec3 n0 = normalize(cross(v0, v1));
	vec3 n1 = normalize(cross(v1, v2));
	vec3 n2 = normalize(cross(v2, v3));
	vec3 n3 = normalize(cross(v3, v0));

	float g0 = acos(dot(-n0, n1));
	float g1 = acos(dot(-n1, n2));
	float g2 = acos(dot(-n2, n3));
	float g3 = acos(dot(-n3, n0));

	return g0 + g1 + g2 + g3 - 2 * PI;
}

bool TraceTriangle(vec3 a, vec3 b, vec3 c, vec3 o, vec3 dir) {
	vec3 ba = b - a;
	vec3 ca = c - a;
	vec3 pa = o - a;
	vec3 pq = -dir;

	vec3 n = cross(ba, ca);
	float d = dot(pq, n);

	if (d <= 0) return false; // ray is either coplainar with triangle abc or facing opposite it

	float t = dot(pa, n);

	if (t < 0) return false;     // ray invariant t >= 0

	vec3 e = cross(pq, pa);

	float v = dot(e, ca);
	if (v < 0.0f || v > d) return false;

	float w = -dot(e, ba);
	if (w < 0.0f || (v + w) > d) return false;

	return true;
}

bool TraceRectangle(vec3 o, vec3 d, vec3 A, vec3 B, vec3 C, vec3 D) {
	return TraceTriangle(A, B, C, o, d) || TraceTriangle(C, D, A, o, d);
}

vec3 ClosestPointToRect(Rectangle r, vec3 center, vec3 u, vec3 v, vec3 p){

	vec3 d = p - center;

	vec3 q = center;

	float dist = dot(d, u);
	if(dist > r.width * 0.5) dist = r.width * 0.5;
	if(dist < -r.width * 0.5) dist = -r.width * 0.5;

	q += dist * u;

	if(dist > r.height * 0.5) dist = r.height * 0.5;
	if(dist < -r.height * 0.5) dist = -r.height * 0.5;

	q += dist * v;

	return q;
}

vec3 ClosestPointOnSegment(vec3 a, vec3 b, vec3 c){
	vec3 ab = b - a;
	float t = dot(c - a, ab) / dot(ab, ab);
	return a + Saturate(t) * ab;
}

vec3 ClosestPointOnLine(vec3 a, vec3 b, vec3 c){
	vec3 ab = b - a;
	float t = dot(c - a, ab) / dot(ab, ab);
	return a + t * ab;
}


void RectangleLight(Light light, Surface surface, vec3 eyes, inout Lighting lighting){
	vec3 worldPos = surface.position;
	vec3 lightPos = (light.localToWorld * vec4(light.position, 1)).xyz;
	mat3 lightToWorld3 = mat3(transpose(inverse(light.localToWorld)));
	vec3 lightNormal =  normalize(lightToWorld3 * light.normal);
	vec3 lightDir = lightPos - surface.position;
	float roughness = surface.roughness;

	Rectangle rect = rectangleLights[light.shapeId & ~RECTANGLE_SHAPE];

	if(dot((-lightDir), lightNormal) > EPSILON ){
		float lightWidth = rect.width;
		float lightLength = rect.height;

		vec3 lightLeft = lightToWorld3 * vec3(1, 0, 0);
		vec3 lightUp = lightToWorld3 * vec3(0, 0, 1);
		float halfWidth = lightWidth * 0.5;
		float halfHeight = lightLength * 0.5;
		vec3 N = normalize(gl_FrontFacing ? surface.normal : -surface.normal);



		 vec3 p0 = lightPos + lightLeft * -halfWidth + lightUp * halfHeight ;
		 vec3 p1 = lightPos + lightLeft * -halfWidth + lightUp * -halfHeight ;
		 vec3 p2 = lightPos + lightLeft * halfWidth + lightUp * -halfHeight ;
		 vec3 p3 = lightPos + lightLeft * halfWidth + lightUp * halfHeight ;

		float solidAngle = RectangleSolidAngle(worldPos, p0, p1, p2, p3);

		float illuminance = solidAngle * 0.2 * (
							Saturate(dot(normalize(p0 - worldPos), N)) +
							Saturate(dot(normalize(p1 - worldPos), N)) +
							Saturate(dot(normalize(p2 - worldPos), N)) +
							Saturate(dot(normalize(p3 - worldPos), N)) +
							Saturate(dot(normalize(lightDir), N))
							);
		
		vec3 L = normalize(lightDir);
		
		vec3 V = normalize(eyes - surface.position);
		vec3 H = normalize(L + V);

		float specularAttenuation = 1;
		if(illuminance > 0){
			vec3 Na = lightNormal;
			vec3 R = -reflect(V, N);
			R = getSpecularDominantDirArea(N, R, roughness);
			float 
			specularAttenuation = Saturate(abs(dot(Na, R)));

			if(specularAttenuation > 0){
				bool hit = TraceRectangle(worldPos, R, p0, p1, p2, p3);

				if( hit){
					L = R;
				}else{
					float t = TracePlane(worldPos, R, lightPos, Na);
					vec3 p = worldPos + R * Saturate(t);
					lightPos = ClosestPointToRect(rect, lightPos, lightLeft, lightUp,  p);
					L = normalize(lightPos - worldPos);

				}
			}
		}


		float distSqr = dot(lightDir, lightDir);
		float roughness = surface.roughness;
		vec3 radiance = light.color * light.value/Area(rect) * illuminance;

		float NDF = DistributionGGX(N, H, roughness);
		float k = (roughness * roughness)/8;
		float G = GeometrySmith(N, V, L, k);
		vec3 F = fresnelSchlick(Saturate(dot(H, V)), vec3(0.04));
		vec3 Ks = F;
		vec3 Kd = 1 - Ks;
		float NdotL = dot(N, L);
		float NdotV = dot(N, V);

		vec3 specular = (NDF * G * F)/(4.0 * Saturate(dot(N, V)) * Saturate(dot(N, L)) + EPSILON);
	
		vec3 color = surface.color;
		lighting.direct.diffuse = max(vec3(0), color/PI * radiance * Saturate(NdotL) * Kd);
		lighting.direct.specular = max(vec3(0), specularAttenuation * specular * radiance * Saturate(NdotL));
	}else{
		lighting.direct.diffuse += vec3(0);
		lighting.direct.specular += vec3(0);
	}
}

void PointLight(Light light, Surface surface, vec3 eyes, inout Lighting lighting){
	vec3 worldPos = surface.position;
	vec3 lightPos = (light.localToWorld * vec4(light.position, 1)).xyz;

	vec3 lightDir = lightPos - worldPos;

	vec3 L = normalize(lightDir);
	vec3 N = normalize(gl_FrontFacing ? surface.normal : -surface.normal);
	vec3 V = normalize(eyes - worldPos);
	vec3 H = normalize(L+V);
	float roughness = surface.roughness;

	float NdotL = dot(N, L);
	vec3 color = surface.color;

	float sqrDist = dot(lightDir, lightDir);
	float intensity = light.value/( 4 * sqrDist  * PI);
	vec3 radiance = light.color * intensity;


	float NDF = DistributionGGX(N, H, roughness);
	float k = (roughness * roughness)/8;
	float G = GeometrySmith(N, V, L, k);
	vec3 F = fresnelSchlick(Saturate(dot(H, V)), vec3(0.04));
	vec3 Ks = F;
	vec3 Kd = 1 - Ks;

	vec3 specular = (NDF * G * F)/(4.0 * Saturate(dot(N, V)) * Saturate(dot(N, L)));
	
	lighting.direct.diffuse += max(vec3(0), color/PI * radiance * Saturate(NdotL) * Kd);
	lighting.direct.specular += max(vec3(0), specular * radiance * Saturate(NdotL));
}

void TubeLight(Light light, Surface surface, vec3 eyes, inout Lighting lighting){
	mat4 lightToWorld = light.localToWorld;
	mat3 lightToWorld3 = transpose(inverse(mat3(light.localToWorld)));
	vec3 worldPos = surface.position;
	vec3 lightPos = (lightToWorld * vec4(light.position, 1)).xyz;

	vec3 lightDir = lightPos - worldPos;
	Tube tube = tubeLights[light.shapeId & ~TUBE_SHAPE];

	vec3 L = normalize(lightDir);
	vec3 N = normalize(gl_FrontFacing ? surface.normal : -surface.normal);
	vec3 V = normalize(eyes - worldPos);
	vec3 H = normalize(L+V);
	float roughness = surface.roughness;

	float NdotL = dot(N, L);
	vec3 color = surface.color;

	vec3 lightLeft = lightToWorld3 * vec3(1, 0, 0);
	vec3 lightUp = lightToWorld3 * vec3(0, 1, 0);
	vec3 zAxis = lightToWorld3 * vec3(0, 0, 1);
	float lightWidth = tube.height;
	float halfWidth = lightWidth * 0.5;

	vec3 P0 = lightPos - zAxis * halfWidth;
	vec3 P1 = lightPos + zAxis * halfWidth;

	vec3 forward = normalize(ClosestPointOnLine(P0, P1, worldPos) - worldPos);
	vec3 left = zAxis;
	vec3 up = cross(zAxis, forward);

	vec3 p0 = lightPos - left * halfWidth + tube.radius * up;
	vec3 p1 = lightPos - left * halfWidth - tube.radius * up;
	vec3 p2 = lightPos + left * halfWidth - tube.radius * up;
	vec3 p3 = lightPos + left * halfWidth + tube.radius * up;

	float solidAngle = RectangleSolidAngle(worldPos, p0, p1, p2, p3);

	float illuminance = solidAngle * 0.2 * (
						Saturate(dot(normalize(p0 - worldPos), N)) +
						Saturate(dot(normalize(p1 - worldPos), N)) +
						Saturate(dot(normalize(p2 - worldPos), N)) +
						Saturate(dot(normalize(p3 - worldPos), N)) +
						Saturate(dot(normalize(lightDir), N))
							);

	vec3 spherePos = ClosestPointOnSegment(P0, P1, worldPos);
	vec3 sphereLightDir = spherePos - worldPos;
	vec3 sphereL = normalize(sphereLightDir);
	float sqrSphereDist = dot(sphereLightDir, sphereLightDir);
	float sqrLightRadius = pow(tube.radius, 2);

	illuminance += PI * Saturate(dot(sphereL, N)) * (sqrLightRadius/sqrSphereDist);

//	if(illuminance > 0){
//		vec3 R = -reflect(V, N);
//		R = getSpecularDominantDirArea(N, R, roughness);
//		float t = TraceTube(worldPos, R, P0, P1, tube.radius);
//
//		if(t >= 0 && t <= 1){
//			L = R;
//			color = vec3(1, 0, 0);
//		}
//	}


	float sqrDist = dot(lightDir, lightDir);
	float intensity = light.value/Area(tube) * INV_PI;
	vec3 radiance = light.color * intensity * illuminance;


	float NDF = DistributionGGX(N, H, roughness);
	float k = (roughness * roughness)/8;
	float G = GeometrySmith(N, V, L, k);
	vec3 F = fresnelSchlick(Saturate(dot(H, V)), vec3(0.04));
	vec3 Ks = F;
	vec3 Kd = 1 - Ks;

	vec3 specular = (NDF * G * F)/(4.0 * Saturate(dot(N, V)) * Saturate(dot(N, L)));
	
	lighting.direct.diffuse += max(vec3(0), color/PI * radiance * Saturate(NdotL) * Kd);
	lighting.direct.specular += max(vec3(0), specular * radiance * Saturate(NdotL));
}

uniform int numLights;
uniform Light lights[4];

void main(){

	vec3 lightPos = light.position;
	vec3 lightDir = lightPos - vertex_in.position;

	Surface surface;
	surface.position = vertex_in.position;
	surface.normal = vertex_in.normal;
	surface.color = vertex_in.color.xyz;
	surface.roughness = roughness;

	Lighting lighting;
	lighting.direct.diffuse = vec3(0);
	lighting.direct.specular = vec3(0);
	
	for(int i = 0; i < numLights; i++){
		Light light = lights[i];

		if((light.shapeId & RECTANGLE_SHAPE) == RECTANGLE_SHAPE){
			RectangleLight(light, surface, eyes, lighting);
		}else if((light.shapeId & SPHERE_SHAPE) == SPHERE_SHAPE){
			SphereLight(light, surface, eyes, lighting);
		}else if((light.shapeId & DISK_SHAPE) == DISK_SHAPE){
			DiskLight(light, surface, eyes, lighting);
		}else if((light.shapeId & TUBE_SHAPE) == TUBE_SHAPE){
			TubeLight(light, surface, eyes, lighting);
		}else{
			PointLight(light, surface, eyes, lighting);
		}

	}

	vec3 color = lighting.direct.diffuse + lighting.direct.specular;
	color = color / (color + vec3(1.0));
   color = pow(color, vec3(1.0/2.2));  

	fragColor = vec4(color, 1);
}