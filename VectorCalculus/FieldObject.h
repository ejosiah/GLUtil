#pragma once
#include <glm/glm.hpp>
#include <limits>
#include <memory>
#include <functional>
#include "../GLUtil/include/ncl/gl/Shape.h"
#include "../GLUtil/include/ncl/gl/textures.h"
#include "../GLUtil/include/ncl/gl/SceneObject.h"
#include "../fsim/fields.h"

using namespace std;
using namespace ncl;
using namespace gl;
using namespace glm;
using namespace fsim;

class HeatMap : public Drawable {
public:
	HeatMap() = default;

	HeatMap(std::function<float(vec3)> func, int numSamples, float width, int numSlices = 1, bool isHeat = true)
		: minVal{ std::numeric_limits<float>::max() }
		, maxVal{ std::numeric_limits<float>::min() }
		, isHeat{ isHeat }
		, numSlices{ numSlices}
		, _slice{ 0 }
		, quad{ ProvidedMesh{ screnSpaceQuad() } }
	{
		quad.defautMaterial(false);
		initHeatMap(func, numSamples, width);
	}

	HeatMap(std::unique_ptr<Texture3D> texture, float minVal, float maxVal, int numSlices = 1, bool isHeat = true)
		: minVal{minVal}
		, maxVal{maxVal}
		, isHeat{isHeat}
		, numSlices{ numSlices }
		, _slice{ 0 }
		, quad{ ProvidedMesh{ screnSpaceQuad() } }
		, texture{ std::move(texture) }
	{
		
	}

	void initHeatMap(function<float(vec3)> eval, int numSamples, float width) {
		auto n = numSamples;
		GLfloat* mapData = new GLfloat[n * n * numSlices];
		for (int k = 0; k < numSlices; k++) {
			for (int j = 0; j < n; j++) {
				for (int i = 0; i < n; i++) {
					float x = width * i / float(n) - width / 2;
					float y = width * j / float(n) - width / 2;
					float z = numSlices * k / float(n) - numSlices / 2;

					vec3 sample = { x, y, z };
					float val = eval(sample);

					int idx = k * (n * n) + (i * n + j);

					mapData[idx] = val;


					minVal = std::min(val, minVal);
					maxVal = std::max(val, maxVal);
				}
			}
		}

		TextureConfig config;
		config.internalFmt = GL_RGB32F;
		config.fmt = GL_RED;
		config.type = GL_FLOAT;

		Data data{ mapData, n, n, numSlices };

		texture = make_unique < Texture3D>(data, config);
	}
	void draw(Shader& shader) override {
		glBindTextureUnit(0, texture->buffer());
		shader.sendUniform1f("maxVal", maxVal);
		shader.sendUniform1f("minVal", minVal);
		shader.sendBool("isHeatMap", isHeat);
		shader.sendUniform1i("numSlices", numSlices);
		shader.sendUniform1i("slice", _slice);
		quad.draw(shader);
	}

	int slice() const {
		return _slice;
	}

	void asHeatMap(bool flag) {
		isHeat = flag;
	}

	void slice(int val) {
		_slice = val;
	}

private:
	float minVal;
	float maxVal;
	bool isHeat;
	int numSlices;
	int _slice;
	ProvidedMesh quad;
	std::unique_ptr<Texture3D> texture;
};

class VectorFieldObject : public ncl::gl::Drawable {
public:
	VectorFieldObject() = default;

	VectorFieldObject(std::function<vec3(vec3)> field, int p, int q, int r, vec3 dim, int step, const glm::vec4& color) {
		construct(field, p, q, r, dim, step, color);
	}

	void construct(std::function<vec3(vec3)> field, int p, int q, int r, vec3 dim, int step, const glm::vec4& color) {
		using namespace ncl::gl;
		using namespace glm;
		using namespace std;
		auto body = Cylinder(0.01, 0.8, 10, 10, color, 1);
		auto head = Cone(0.055, 0.2, 10, 10, color, 1);
		Mesh hMesh = head.getMeshes().at(0);
		Mesh bMesh = body.getMeshes().at(0);

		hMesh.xforms.clear();
		bMesh.xforms.clear();

		float w = dim.x;
		float l = dim.y;
		float b = dim.z;
		unsigned int instances = 0;
		for (int k = 0; k <= r; k += step) {
			for (int j = 0; j <= q; j += step) {
				for (int i = 0; i <= p; i += step) {
					float x = w * i / float(p) - w / 2;
					float y = l * j / float(q) - l / 2;
					float z = b * k / float(r) - b / 2;
					vec3 point = { x, y, z };
					vec3 v = field(point);

					auto [headXForm, bodyXForm] = getXform(v, point);
					hMesh.xforms.push_back(headXForm);
					bMesh.xforms.push_back(bodyXForm);
					instances++;
				}
			}
		}

		vHeads = new ProvidedMesh{ hMesh, false, instances };
		vTails = new ProvidedMesh{ bMesh, false, instances };

	}

	std::tuple<glm::mat4, glm::mat4> getXform(glm::vec3 vector, glm::vec3 origin, float s = 0.3, bool fixedLength = true) {
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



	virtual void draw(ncl::gl::Shader& shader) override {
		vHeads->draw(shader);
		vTails->draw(shader);
	}

private:
	std::vector<std::unique_ptr<ncl::gl::Vector>> vectors;
	ncl::gl::ProvidedMesh* vHeads;
	ncl::gl::ProvidedMesh* vTails;

};

class ScalaFieldObject : public SceneObject {
public:
	ScalaFieldObject(Scene& scene, const fsim::ScalarField& field, int numPoints = 1000, int numLayers = 1, float width = 2)
		:SceneObject{ &scene }
		, numPoints{ numPoints }
		, numLayers{ numLayers }
		, width{ width }
		, field{ field }
	{
		
	}

	void init() {
		heatMap = HeatMap{ [&](vec3 v) { return field.sample(v); }, 1024, width, numLayers };
		initField();
		initPatch();
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
		patch.defautMaterial(false);

		quad = ProvidedMesh{ screnSpaceQuad() };
		quad.defautMaterial(false);
	}

	void initField() {
		int n = 1024;
		int size = n * n * numLayers;
		GLfloat* heightMapData = new GLfloat[size];
		GLfloat* laplacianMapData = new GLfloat[size];
		GLfloat* gradiantMapData = new GLfloat[size * 3];

		
		for (int k = 0; k < numLayers; k++) {
			for (int j = 0; j < n; j++) {
				for (int i = 0; i < n; i++) {
					float x = width * i / float(n) - width / 2;
					float y = width * j / float(n) - width / 2;
					float z = numLayers * k / float(n) - numLayers / 2;
					vec3 point = { x, y, z };
					float val = field.sample(point);

					// pos.z * (size.y * size.x) + (pos.y * size.x) + pos.x;

					int idx = k * (n * n) + (i * n + j);

					auto grad = field.gradient(point);
					float lapVal = field.laplacian(grad);

					heightMapData[idx] = val;
					laplacianMapData[idx] = lapVal;

					
					auto gradId = idx * 3;
					gradiantMapData[gradId] = grad.x;
					gradiantMapData[gradId + 1] = grad.y;
					gradiantMapData[gradId + 2] = grad.z;

					minVal = std::min(val, minVal);
					maxVal = std::max(val, maxVal);

					lapMinVal = std::min(lapVal, lapMinVal);
					lapMaxVal = std::max(lapVal, lapMaxVal);
				}
			}
		}

		//float h = (maxVal - minVal);
		//h += h * 0.2;
		//eyes.z = h / (2 * tan(quarter_pi<float>()));

		TextureConfig config;
		config.internalFmt = GL_R32F;
		config.fmt = GL_RED;
		config.type = GL_FLOAT;
		config.name = "heightMap";

		Data data{ heightMapData, n, n, numLayers };

		heightMap = make_unique < Texture3D>( data, config);
		
		data = Data{ laplacianMapData, n , n, numLayers };
		config.name = "laplacianMap";
		laplacianMap = make_unique < Texture3D>(data, config );
	
		data = Data{ gradiantMapData, n, n, numLayers };
		config.internalFmt = GL_RGB32F;
		config.fmt = GL_RGB;
		config.name = "gradiantMap";
		gradiantMap = make_unique<Texture3D>(data, config );

		// TODO make_unique move the pointer so have to delete in ~Data 
		//delete[] heightMapData;
		//delete[] laplacianMapData;
		//delete[] gradiantMapData;
	}

	vec3 pack(vec3 n) {
		return normalize(vec3(n.xzy)) * 0.5f + 0.5f;
	}

	void render(bool shadowMode = false) override {
		renderField();
		renderLaplacian();

		switch (visualization) {
		case Visualization::HeatMap:
			renderHeatMap();
			break;
		case Visualization::Isosurface:
			renderIsosurface();
			break;
		case Visualization::Gradient:
			// TODO
			break;
		}
	}

	void renderField() {
		scene().shader("scalar_field")([&] {
			int h = scene().height();
			int w = scene().width() - std::round(scene().width() * (float(h * 0.5) / scene().width()));
			
			glViewportIndexedf(0, 0, 0, w, h);
			cam.view = lookAt(eyes, vec3(0, 1, 0), { 0, 1, 0 });
		//	cam.model = rotate(mat4(1), glm::radians(angle), { 0, 1, 0 });
			cam.model = rotate(mat4(1), glm::radians(yaw), { 0, 1, 0 });
			cam.model = rotate(cam.model, glm::radians(pitch), { 1, 0, 0 });
			float ar = float(scene().width()) / scene().height();
			cam.projection = perspective(half_pi<float>() / 2, ar, 0.1f, 1000.0f);

			glBindTextureUnit(0, heightMap->buffer());
			glBindTextureUnit(1, gradiantMap->buffer());
			send(cam);
			send("id", 0);
			//	send(scene().activeCamera());
			send("minVal", minVal);
			send("maxVal", maxVal);
			send("slice", slice);
			send("numSlices", numLayers);
			send("numSteps", 20);
			shade(patch);
		});
	}

	void renderHeatMap() {
		scene().shader("field")([&]{
			int h = scene().height() * 0.5;
			int w = std::round(scene().width() * (float(h) / scene().width()));
			int x = scene().width() - w;
			glViewportIndexedf(1, x, 0, w, h);
			send("id", 1);
			heatMap.asHeatMap(isHeat);
			shade(heatMap);
		});
	}

	void renderIsosurface() {
		scene().shader("isosurface")([&] {
			int h = scene().height() * 0.5;
			int w = std::round(scene().width() * (float(h) / scene().width()));
			int x = scene().width() - w;
			glViewportIndexedf(1, x, 0, w, h);

			float d = width * 0.5;
			float y = d/tan(quarter_pi<float>());

			cam.view = lookAt({ 0, 1, 0 }, vec3(0, 0, 0), { 1, 0, 0 });
			cam.model = mat4(1);
		//	cam.projection = perspective(half_pi<float>(), float(w)/float(h), 0.1f, 1000.0f);
			cam.projection = glm::ortho(-d, d, -d, d, 0.1f, 5.0f);

			glBindTextureUnit(0, heightMap->buffer());
			glBindTextureUnit(1, gradiantMap->buffer());
			send(cam);
			send("id", 1);
			send("minVal", minVal);
			send("maxVal", maxVal);
			send("slice", slice);
			send("numSlices", numLayers);
			send("numSteps", 20);
			shade(patch);
			});
	}

	void renderLaplacian() {
		scene().shader("field")([&] {
			int h = scene().height() * 0.5;
			int w = std::round(scene().width() * (float(h) / scene().width()));
			int x = scene().width() - w;
			glViewportIndexedf(2, x, h, w, h);
			glBindTextureUnit(0, laplacianMap->buffer());
			send("id", 2);
			send("isHeatMap", false);
			send("minVal", lapMinVal);
			send("maxVal", lapMaxVal);
			send("slice", slice);
			send("numSlices", numLayers);
			shade(quad);
		});
	}

	void update(float t) override {
		angle += speed * t;
		auto mouse = Mouse::get();
		yaw += mouse.relativePos.x;
		pitch += mouse.relativePos.y;
	}

	void processInput(const Key& key) override {
		if (key.pressed()) {
			switch (key.value()) {
			case '-':
				slice -= 1;
				break;
			case '=':
			case '+':
				slice += 1;
				break;
			case 'h':
				visualization = Visualization::HeatMap;
				isHeat = true;
				break;
			case 'g':
				visualization = Visualization::HeatMap;
				isHeat = false;
				break;
			case 'i':
				visualization = Visualization::Isosurface;
				break;
			}

			slice = glm::clamp(slice, 0, numLayers);
			heatMap.slice(slice);
		}
	}

	enum class Visualization {
		HeatMap, Isosurface, Gradient
	};

private:
	int numPoints;
	int numLayers;
	float width;
	const fsim::ScalarField& field;
	std::unique_ptr<Texture3D> heightMap;
	std::unique_ptr<Texture3D> laplacianMap;
	std::unique_ptr<Texture3D> gradiantMap;
	HeatMap heatMap;
	ProvidedMesh patch;
	ProvidedMesh quad;
	float minVal = std::numeric_limits<float>::max();
	float maxVal = std::numeric_limits<float>::min();
	float lapMinVal = std::numeric_limits<float>::max();
	float lapMaxVal = std::numeric_limits<float>::min();
	GlmCam cam;
	float angle = 0;
	float speed = 20;
	float yaw = 0;
	float pitch = 0;
	vec3 eyes{ 0, 3, 10 };
	float dz = 1;
	int slice = 0;
	bool isHeat = false;
	Visualization visualization = Visualization::HeatMap;
};

class VectorFieldSceneObject : public SceneObject {
public:

	VectorFieldSceneObject(Scene& scene, VectorField& field)
		:SceneObject(&scene)
		, field{ field }
	{
		init();
	}

	void init() override {
		vFieldObj = VectorFieldObject{ [&](auto v) { return field.sample(v); } , 10, 10, 10, vec3(10), 2, RED };
		divergencMap = HeatMap{ [&](vec3 v) { return field.div(v);  }, 1024, 1.5f * two_pi<float>(), 1, false };
	}

	void render(bool flag = false) override {
		renderField();
		renderDivergence();
	}

	void renderField() {
		scene().shader("flat")([&](Shader& s) {
			int h = scene().height();
			int w = scene().width() - std::round(scene().width() * (float(h * 0.5) / scene().width()));

			glViewportIndexedf(0, 0, 0, w, h);

			cam.view = lookAt(eyes, vec3(0, 1, 0), { 0, 1, 0 });
			cam.model = rotate(mat4(1), glm::radians(angle), { 0, 1, 0 });
			
			float ar = float(scene().width()) / scene().height();
			cam.projection = perspective(half_pi<float>() / 2.0f, ar, 0.1f, 1000.0f);
			send(cam);
			vFieldObj.draw(s);
			});
	}

	void renderDivergence() {
		scene().shader("field")([&] {
			int h = scene().height() * 0.5;
			int w = std::round(scene().width() * (float(h) / scene().width()));
			int x = scene().width() - w;
			glViewportIndexedf(1, x, h, w, h);
			send("id", 1);
			shade(divergencMap);
		});
	}

	void update(float t) override {
		angle += speed * t;
	}

	void processInput(const Key& key) override {

	}

private:
	VectorField& field;
	VectorFieldObject vFieldObj;
	HeatMap divergencMap;
	GlmCam cam;
	float angle = 0;
	float speed = 20;
	vec3 eyes{ 0, 0, 8 };
	float dz = 1;
};