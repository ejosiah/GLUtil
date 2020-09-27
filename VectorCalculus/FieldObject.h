#pragma once
#include <glm/glm.hpp>
#include <limits>
#include <memory>
#include "../GLUtil/include/ncl/gl/Shape.h"
#include "../GLUtil/include/ncl/gl/textures.h"
#include "../GLUtil/include/ncl/gl/SceneObject.h"
#include "../fsim/fields.h"

using namespace std;
using namespace ncl;
using namespace gl;
using namespace glm;
using namespace fsim;

class ScalaFieldObject : public SceneObject {
public:
	ScalaFieldObject(Scene& scene, const fsim::ScalarField& field, int numPoints = 1000, int width = 2)
		:SceneObject{ &scene }
		, numPoints{ numPoints }
		, width{ width }
		, field{ field }
	{
		
	}

	std::vector<ncl::gl::Mesh> createMesh(const fsim::ScalarField& field, int p, int q, const glm::vec4& color, int w) {
		using namespace ncl::gl;
		using namespace glm;

		Mesh mesh;
		float max = std::numeric_limits<float>::min();
		float min = std::numeric_limits<float>::max();
		for (int j = 0; j < q; j++) {
			for (int i = 0; i < p; i++) {
				float x = w * i / float(p) - w/2;
				float y = w * j / float(q) - w/2;
				vec3 point = { x, y, 0 };
				float z = field.sample(point);
				min = std::min(z, min);
				max = std::max(z, max);
				mesh.positions.push_back({ x, y, z });
				mesh.normals.push_back( glm::normalize( field.gradient(point) ));
				mesh.uvs[0].push_back(glm::vec2(i/float(p), j/float(q)));

			}
		}

		float h = max - min;
		mesh.colors = std::vector<vec4>(mesh.positions.size());
		for (int i = 0; i < mesh.positions.size(); i++) {
			float ratio = (mesh.positions[i].z - min) / h;
			float r = smoothstep(0.6f, 0.8f, ratio);
			float g = smoothstep(0.0f, 0.4f, ratio) - smoothstep(0.8f, 1.0f, ratio);
			float b = 1 - smoothstep(0.4f, 0.6f, ratio);
			mesh.colors[i] = { r, g, b, 1 };
		}

		for (int j = 0; j < q; j++) {
			for (int i = 0; i <= p; i++) {
				mesh.indices.push_back((j + 1) * (p + 1) + i);
				mesh.indices.push_back(j * (p + 1) + i);
			}
		}

		mesh.primitiveType = GL_POINTS;
		return std::vector<Mesh>(1, mesh);
	}

	void init() {
		initField();
		initPatch();

		float ar = float(scene().width()) / scene().height();
	//	cam.model = rotate(cam.model, -glm::half_pi<float>(), { 1.0f, 0, 0 });
		cam.projection = perspective(half_pi<float>() / 2.0f, ar, 0.1f, 1000.0f);
	}

	void initPatch() {
		float n = std::sqrt(numPoints);
		vec4 outer{ n };
		vec2 inner{ n };
		glPatchParameteri(GL_PATCH_VERTICES, 4);
		glPatchParameterfv(GL_PATCH_DEFAULT_OUTER_LEVEL, &outer[0]);
		glPatchParameterfv(GL_PATCH_DEFAULT_INNER_LEVEL, &inner[0]);

		Mesh mesh;
		float halfWidth = width / 2;
		mesh.positions.emplace_back(-halfWidth, 0, halfWidth);
		mesh.positions.emplace_back(halfWidth, 0, halfWidth);
		mesh.positions.emplace_back(halfWidth, 0, -halfWidth);
		mesh.positions.emplace_back(-halfWidth, 0, -halfWidth);
		mesh.primitiveType = GL_PATCHES;

		patch = ProvidedMesh{ mesh };
		quad = ProvidedMesh{ screnSpaceQuad() };
	}

	void initField() {
		float n = 1024;
		GLfloat* heightMapData = new GLfloat[n * n];
		GLfloat* laplacianMapData = new GLfloat[n * n];

		for (int j = 0; j < n; j++) {
			for (int i = 0; i < n; i++) {
				float x = width * i / float(n) - width / 2;
				float y = width * j / float(n) - width / 2;
				vec3 point = { x, y, 0 };
				float val = field.sample(point);

				int idx = (i * n + j);

				auto grad = field.gradient(point);
				float lapVal = field.laplacian(grad);

				heightMapData[idx] = val;
				laplacianMapData[idx] = lapVal;

				minVal = std::min(val, minVal);
				maxVal = std::max(val, maxVal);

				lapMinVal = std::min(lapVal, lapMinVal);
				lapMaxVal = std::max(lapVal, lapMaxVal);
			}
		}

		heightMap = Texture2D( heightMapData, n, n, "heightMapData", 0, GL_R32F, GL_RED, GL_FLOAT );
		laplacianMap = Texture2D(laplacianMapData, n, n, "laplacianMap", 0, GL_R32F, GL_RED, GL_FLOAT );

		delete[] heightMapData;
		delete[] laplacianMapData;
	}


	void render(bool shadowMode = false) override {
		scene().shader("scalar_field")([&] {
			
			cam.view = lookAt(eyes, vec3(0, 1, 0), { 0, 1, 0 });
			cam.model = rotate(mat4(1), glm::radians(angle), { 0, 1, 0 });
			
			glBindTextureUnit(0, heightMap.buffer());
			send(cam);
		//	send(scene().activeCamera());
			send("minVal", minVal);
			send("maxVal", maxVal);
			shade(patch);
		});
	}

	void renderField() {
		scene().shader("scalar_field")([&] {

			cam.view = lookAt(eyes, vec3(0, 1, 0), { 0, 1, 0 });
			cam.model = rotate(mat4(1), glm::radians(angle), { 0, 1, 0 });

			glBindTextureUnit(0, heightMap.buffer());
			send(cam);
			//	send(scene().activeCamera());
			send("minVal", minVal);
			send("maxVal", maxVal);
			shade(patch);
			});
	}

	void renderHeatMap() {
		scene().shader("field")([&] {
			glBindTextureUnit(0, heightMap.buffer());
			send("minVal", minVal);
			send("maxVal", maxVal);
			send("isHeatMap", true);
			shade(quad);
		});
	}

	void renderLaplacian() {
		scene().shader("field")([&] {
			glBindTextureUnit(0, laplacianMap.buffer());
			send("isHeatMap", false);
			shade(quad);
		});
	}

	void update(float t) override {
		angle += speed * t;
	}

	void processInput(const Key& key) override {
		if (key.value() == 'w' && key.pressed()) {
			eyes.z -= dz;
		}
		else if (key.value() == 's' && key.pressed()) {
			eyes.z += dz;
		}
		if (key.value() == 'a' && key.pressed()) {
			eyes.x -= dz;
		}
		else if (key.value() == 'd' && key.pressed()) {
			eyes.x += dz;
		}
		if (key.value() == 'r' && key.pressed()) {
			eyes.y -= dz;
		}
		else if (key.value() == 'e' && key.pressed()) {
			eyes.y += dz;
		}
	}

private:
	int numPoints;
	int width;
	const fsim::ScalarField& field;
//	std::unique_ptr<Texture2D> heightMap;
	Texture2D heightMap;
	Texture2D laplacianMap;
//	std::unique_ptr<Texture2D> laplacianMap;
	ProvidedMesh patch;
	ProvidedMesh quad;
	float minVal = std::numeric_limits<float>::max();
	float maxVal = std::numeric_limits<float>::min();
	float lapMinVal = std::numeric_limits<float>::max();
	float lapMaxVal = std::numeric_limits<float>::min();
	GlmCam cam;
	float angle = 0;
	float speed = 20;
	vec3 eyes{ 0, 3, 10 };
	float dz = 1;
};

class VectorFieldObject : public ncl::gl::Drawable {
public:
	VectorFieldObject() = default;

	VectorFieldObject(const fsim::VectorField& field, int p, int q, int r, float width, int step, const glm::vec4& color) {
		//using namespace glm;
		//float w = width;
		//for (int k = 0; k <= r; k++) {
		//	for (int j = 0; j <= q; j += step) {
		//		for (int i = 0; i <= p; i += step) {
		//			float x = w * i / float(p) - w / 2;
		//			float y = w * j / float(q) - w / 2;
		//			float z = w * k / float(r) - w / 2;
		//			vec3 point = { x, y, z };
		//			vec3 v = field.sample(point);
		//			vectors.push_back(std::unique_ptr<ncl::gl::Vector>{ new ncl::gl::Vector(v, point, 0.3, color, true) });
		//		}
		//	}
		//}
		construct(field, p, q, r, width, step, color);
	}

	void construct(const fsim::VectorField& field, int p, int q, int r, float width, int step, const glm::vec4& color) {
		using namespace ncl::gl;
		using namespace glm;
		using namespace std;
		auto body =  Cylinder(0.01, 0.8, 10, 10, color, 1);
		auto head =  Cone(0.055, 0.2, 10, 10, color, 1);
		Mesh hMesh = head.getMeshes().at(0);
		Mesh bMesh = body.getMeshes().at(0);
		
		
		float w = width;
		for (int k = 0; k <= r; k++) {
			for (int j = 0; j <= q; j += step) {
				for (int i = 0; i <= p; i += step) {
					float x = w * i / float(p) - w / 2;
					float y = w * j / float(q) - w / 2;
					float z = w * k / float(r) - w / 2;
					vec3 point = { x, y, z };
					vec3 v = field.sample(point);
					
					mat4 headXForm;
					mat4 bodyXForm;

					tie(headXForm, bodyXForm) = getXform(v, point);
					hMesh.xforms.push_back(headXForm);
					bMesh.xforms.push_back(bodyXForm);
				}
			}
		}
		unsigned int instances = p * q * r;
		vHeads = new ProvidedMesh{ hMesh, false, instances };
		vTails = new ProvidedMesh{ bMesh, false, instances };
		
	}

	std::tuple<glm::mat4, glm::mat4> getXform(glm::vec3 vector, glm::vec3 origin, float s = 1, bool fixedLength = true) {
		using namespace glm;

		vec3 v1(0, 0, -1);	// primitives face towards -z, so we have to rotate (0,0,-1) to v2
		vec3 v2 = normalize(vector);
		vec3 axis = normalize(cross(v1, v2));
		if (abs(v1) == abs(v2)) {
			axis = vec3(1, 0, 0);
		}

		float angle = degrees(acos(dot(v1, v2)));

		auto rotate = mat4_cast(fromAxisAngle(axis, angle));
		float l = fixedLength ? 1 : length(vector);

		mat4 headXform = scale(mat4(1), { s, s, s }) * translate(mat4(1), origin) * rotate * translate(mat4(1), { 0, 0, -l });
		mat4 bodyXform = scale(mat4(1), { s, s, s }) * translate(mat4(1), origin) * rotate * mat4(1);

		return std::make_tuple(headXform, bodyXform);
	}

	VectorFieldObject(const fsim::ScalarField& field, int p, int q, int r, float width, int step, const glm::vec4& color) {
		using namespace glm;
		float w = width;
		for (int k = 0; k <= r; k++) {
			for (int j = 0; j <= q; j += step) {
				for (int i = 0; i <= p; i += step) {
					float x = w * i / float(p) - w / 2;
					float y = w * j / float(q) - w / 2;
					float z = w * k / float(r) - w / 2;
					vec3 point = { x, 0, y };
					vec3 v = field.gradient(point);
					vectors.push_back(std::unique_ptr<ncl::gl::Vector>{ new ncl::gl::Vector(v, point, 0.1, color, true) });
				}
			}
		}
	}

	virtual void draw(ncl::gl::Shader& shader) override {
		for (auto& v : vectors) {
			v->draw(shader);
		}
	//	vHeads->draw(shader);
	//	vTails->draw(shader);
	}
private:
	std::vector<std::unique_ptr<ncl::gl::Vector>> vectors;
	ncl::gl::ProvidedMesh* vHeads;
	ncl::gl::ProvidedMesh* vTails;

};