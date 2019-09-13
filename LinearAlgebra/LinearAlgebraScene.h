#pragma once

#include "../GLUtil/include/ncl/gl/Scene.h"
#include "../GLUtil/include/ncl/sampling/Sampler.h"
#include <glm/glm.hpp>
#include <algorithm>

using namespace std;
using namespace ncl;
using namespace gl;
using namespace glm;

class LinearAlgebraScene : public Scene {
public:
	LinearAlgebraScene() :Scene("Linear Algebra", Resolution::HD.width, Resolution::HD.height) {
		_requireMouse = false;
	}

	void init() override {
		plane = new Plane(10, 10, 0.5, 0.5, 0);
		n = { 0, 1, 0 };
		n = { -0.0162, - 0.0010, 0.9999 };
		normal = new Vector(n, vec3(0), 1.0, GREEN);
		negNormal = new Vector(-n, vec3(0), 1.0, GREEN);
		float n1 = 1;
		float n2 = 1.57;
		float etaI = n1 / n2;
		float etaO = n2 / n1;

		float cAngle = criticalAngle(n2, n1);

		a = (rotate(mat4(1), cAngle + (pi<float>()/180) * 2, { 0, 0, 1 }) * vec4(n, 1)).xyz;
		float cA = degrees(acos(dot(a, n)));
		a = vec3{ -1, 1, 0 };
		a = normalize(a);
		a = vec3(-0.0107, - 0.1637, 0.9864);
		//b = refract(-a, n, etaI);	// vector in direction of point above surface, flip vectors opposite direction of point
		//b = Reflect(a, n);
		//b = reflect(a, n);	// vector in direction of point, flip vectors opposite direction of point
	//	Refract(a, n, etaI, &b);
		
		//b = refract(a, -n, etaO);	// vectors in direction of point below surface, flip normal and oppsoite direction vectors
		//b = reflect(a, n);
	//	a = -a;
		vec4 color = BLUE;
		
		f = transmit(a, b, n, n1, n2, color, pdf);
		//b = { 0.0332, 0.1027, 0.9942 };
		
		aVec = new Vector(a, vec3(0), 1.0, RED);
		bVec = new Vector(b, vec3(0), 1.0, color);
		light[0].on = true;
		light[0].position = { 0, 3, 3, 0 };
		cam.view = lookAt({ 0, 1.5, 3.0 }, vec3(0), { 0, 1, 0 });
	}

	vec3 Reflect(vec3 i, vec3 n) {
		return -i + 2.0f * dot(i, n) * n;
	}

	inline bool Refract(vec3& wi, vec3& n, float eta,
		vec3* wt) {
		// Compute $\cos \theta_\roman{t}$ using Snell's law
		float cosThetaI = dot(n, wi);
		float sin2ThetaI = std::max(float(0), float(1 - cosThetaI * cosThetaI));
		float sin2ThetaT = eta * eta * sin2ThetaI;

		// Handle total internal reflection for transmission
		if (sin2ThetaT >= 1) return false;
		float cosThetaT = std::sqrt(1 - sin2ThetaT);
		*wt = eta * -wi + (eta * cosThetaI - cosThetaT) * n;
		return true;
	}

	float criticalAngle(float ni, float nt) {
		return asin(nt * sin(half_pi<float>()) * (1.0/ni));
	}

	vec3 faceforward(vec3 i, vec3 n) {
		return dot(i, n) > 0 ? n : -n;
	}

	float transmit(vec3 wo, vec3& wi, vec3 n, float n1, float n2, vec4& color, float& pdf) {
		float woDotn = dot(wo, n);
		bool entering = woDotn > 0;
		float etaI = entering ? n1 : n2;
		float etaT = entering ? n2 : n1;

		float eta = etaI / etaT;
		vec3 nRef = n;
		n = faceforward(wo, n);

		wi = refract(-wo, n, eta);

		bool totalInternalReflection = isnan(wi) == bvec3(true);
		if (totalInternalReflection) {
			wi = reflect(-wo, n);
			color = YELLOW;
			pdf = 0;
			return 0;
		}
		pdf = 1;
		color = CYAN;
		float cos0i = dot(wi, { 0, 1, 0 });
		return 1 - frDielectric(cos0i, etaI, etaT) / abs(cos0i);
	}

	float frDielectric(float cos0i, float ni, float nt) {
		cos0i = clamp(cos0i, -1.0f, 1.0f);

		bool entering = cos0i > 0.0;
		if (!entering) {
			swap(ni, nt);
			cos0i = abs(cos0i);
		}

		float sin0i = sqrt(glm::max(0.0f, 1.0f - cos0i * cos0i));
		float sin0t = ni / (nt * sin0i);

		if (sin0t >= 1.0) return 1.0; // total internal reflection

		float cos0t = sqrt(glm::max(0.0, 1.0 - sin0t * sin0t));

		float rPar = (nt * cos0i - ni * cos0t) / (nt * cos0i + ni * cos0t);
		float rPerp = (ni * cos0i - nt * cos0t) / (ni * cos0i + nt * cos0t);

		return 0.5 * (rPar * rPar + rPerp * rPerp);
	}

	void display() override {
		stringstream ss;
		ss << "pdf: " << pdf << "\nf: " << f;
		renderText(10, 10, ss.str());
		shader("default")([&](Shader& s) {
			send(cam);
			send(light[0]);
			shade(normal);
		//	shade(negNormal);
			shade(aVec);
			shade(bVec);
			shade(plane);
		});
	}

	void update(float dt) override {

	}

	void resized() override {
		cam.projection = perspective(half_pi<float>() / 2.0f, aspectRatio, 0.1f, 100.0f);
	}
private:
	Plane* plane;
	Vector* normal;
	Vector* negNormal;
	Vector* aVec;
	Vector* bVec;
	vec3 a;
	vec3 b;
	vec3 n;
	float f;
	float pdf;
};