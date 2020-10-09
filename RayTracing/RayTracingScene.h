#pragma once

#include <array>
#include "../GLUtil/include/ncl/gl/Scene.h"
#include "../GLUtil/include/ncl/gl/SkyBox.h"
#include "../GLUtil/include/ncl/ray_tracing/RayGenerator.h"
#include "../GLUtil/include/ncl/ray_tracing/model.h"
#include "../GLUtil/include/ncl/ray_tracing/Ray.h"
#include "../GLUtil/include/ncl/geom/aabb2.h"
#include "../GLUtil/include/ncl/geom/bvh2.h"
#include "../GLUtil/include/ncl/data_structure/binary_tree.h"

using namespace std;
using namespace ncl;
using namespace gl;
using namespace glm;
namespace rt = ray_tracing;
namespace box = geom::bvol::aabb;
namespace bvh = geom::bvh;

const unsigned int MILLION = 1000000;
const unsigned int BILLION = 1000000000;
const float DAY_LIGHT_ILLUMINANCE = 64000;
const int MaxSpheres = 10;
const int MAX_BOUNCES = 10;
Logger& logger = Logger::get("ray");

struct {
	Texture triangles;
	Texture normals;
	Texture uvs;
	Texture indices;
	bool hasIndices;
	int matId;
	int numTriangles;
} triangleData; 

#pragma pack(push, 1)
struct Debug {
	vec4 o;
	vec4 d;
	vec4 n;
	vec4 wo;
	vec4 wi;
	float f;
	float cos0;
	float n1;
	float n2;
	int bounce;
	int id;
	int shapeId;
	float t;
};
#pragma pack(pop)

class RayTracer : public Compute {
	using Bounds = geom::bvol::AABB2;

public:
	RayTracer(Scene& scene, StorageBuffer<rt::Ray>& rays)
		:Compute(vec3{ scene.width() / 32.0f, scene.height() / 32.0f, 1.0f }
			, vector<Image2D>{ Image2D(scene.width(), scene.height(), GL_RGBA32F, "image", 0) }
	, & scene.shader("whitted_raytracer"))
		, rays{ rays }
		, scene{ scene }
		, bounds{ bounds }{

		auto& cam = scene.getActiveCameraController();
		worldRadius = (cam.getFar() - cam.getNear()) * 0.5;
		worldCenter = vec3(0);


		vector<string> skyTextures = vector<string>{
			"right.jpg", "left.jpg",
			"top.jpg", "bottom.jpg",
			"front.jpg", "back.jpg"
		};

		string root = "C:\\Users\\" + username +  "\\OneDrive\\media\\textures\\skybox\\005\\";
		transform(skyTextures.begin(), skyTextures.end(), skyTextures.begin(), [&root](string path) {
			return root + path;
		});

		skybox = SkyBox::create(skyTextures, 0, scene);
		auto numRays = scene.width() * scene.height();
		numPixels = numRays;

		checkerboard = new CheckerBoard_gpu(255, 255, WHITE, GRAY, 1, "checker");
		checkerboard->compute();
		glBindTexture(GL_TEXTURE_2D, checkerboard->images().front().buffer());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		checkerboard->images().front().renderMode();

		initObjs();
		buildBVH();
		scene.shader("whitted_raytracer")([&] {
			plane_ssbo.sendToGPU(true);
			light_ssbo.sendToGPU(true);
			sphere_ssbo.sendToGPU(true);
			material_ssbo.sendToGPU(true);
			send("worldCenter", worldCenter);
			send("worldRadius", worldRadius);
			send("numSpheres", numSpheres);
			send("numPlanes", numPlanes);
		});
	}

	void initObjs() {
		auto materials = std::vector<rt::Material>{};
		auto planes = vector<rt::Plane>{};

		rt::Plane plane;
		plane.n = { 0, 1, 0, 1 };
		plane.d = 0;
		plane.id = planes.size();
		plane.matId = -1;
		//plane.matId = materials.size();

		rt::Material m;
		m.ambient = vec4(0);
		m.diffuse = vec4(1, 0, 0, 1);
		m.ior = 1.52;
		m.bsdf[0] = rt::BSDF_DIFFUSE;
		m.nBxDfs += 1;
		m.kr = vec4(0.8, 0.8, 0.8, 1.0);
		m.kt = vec4(0.8, 0.8, 0.8, 1.0);
		m.shine = 50;
		materials.push_back(m);
		planes.push_back(plane);


		//plane.n = { -1, 0, 0, 1 };
		//plane.d = 10;
		//plane.id = planes.size();
		//plane.matId = -1;
		//planes.push_back(plane);

		numPlanes = planes.size();

		plane_ssbo = StorageBuffer<rt::Plane>{ planes, 5 };
		initRayCounter();
		initLights();
		initSpheres(materials);
		initTriangles(materials);
		material_ssbo = StorageBuffer<rt::Material>{ materials, 3 };
		debug_ssbo = StorageBuffer<Debug>{ size_t(scene.width() * scene.height()), 8 };

	}

	void initLights() {
		auto lights = vector<rt::LightSource>{};
		
		rt::LightSource light;
		light.flags = rt::POINT_LIGHT;
	//	light.flags = rt::DISTANT_LIGHT;
		light.I = vec4(vec3(1), 80);
	//	light.I = vec4(vec3(1), 1);
	//	light.lightToWorld = translate(mat4(1), { 0, 10, 10 });
		light.lightToWorld = translate(mat4(1), { 0, 20, 20 });
		light.worldToLight = inverse(light.lightToWorld);
		light.position = light.lightToWorld * vec4(0, 0, 0, 1);
	//	light.position = vec4(0, 1, 0, 1);
		lights.push_back(light);
		
		light_ssbo = StorageBuffer<rt::LightSource>{ lights, 2 };
	}

	void initSpheres(vector< rt::Material>& materials) {
		auto spheres = std::vector<rt::Sphere>{};
		vec2 radius = { 0.3, 2.0 };
		float placementRadius = 20;
		auto seed = 12345678919;
		auto rng = rngReal(0, 1, seed);
		auto rngIOR = rngReal(1.2, 2.417, seed);
		auto rngShine = rngReal(50, 100, seed);

		vector<int> bsdfs = { rt::FRESNEL_SPECULAR, rt::BSDF_DIFFUSE, rt::SPECIULAR_REFLECT };
	//	vector<int> bsdfs = {rt::FRESNEL_SPECULAR };
		auto rngIndex = rngInt(0, bsdfs.size() - 1, seed);

		for (int i = 0; i < MaxSpheres; i++) {

			rt::Material m;
			m.ambient = vec4(0);
			m.diffuse = randomColor();
			m.specular = vec4(1);
			m.shine = rngShine();
			m.kt = randomColor();
			m.kt = vec4(1);
			m.kr = vec4(0.8, 0.8, 0.8, 1.0);
			m.ior = rngIOR();
		//	m.bsdf[0] = rt::FRESNEL_SPECULAR;
			m.bsdf[0] = rt::BSDF_DIFFUSE;
		//	m.bsdf[0] = bsdfs[rngIndex()];
			m.nBxDfs += 1;

			rt::Sphere s;
		//	s.radius = radius.x + rng() * (radius.y - radius.x);
			s.radius = radius.x + rng() * (radius.y - radius.x);
			float r = sqrt(rng());
			vec2 u = vec2(r * cos(rng() * two_pi<float>()), r * sin(rng() * two_pi<float>())) * placementRadius;
			s.center = vec4(0);
			s.objectToWorld = translate(mat4(1), vec3{ u.x, s.radius, u.y });
			s.worldToObject = inverse(s.objectToWorld);
			vec3 worldCenter = vec3(s.objectToWorld * vec4(0, 0, 0, 1));
			auto diagonal = normalize(vec3(1, 1, 1));
			auto x0 = worldCenter + vec3(1, 0, 0) * s.radius;
			auto x1 = worldCenter + vec3(-1, 0, 0) * s.radius;
			auto y0 = worldCenter + vec3(0, 1, 0) * s.radius;
			auto y1 = worldCenter + vec3(0, -1, 0) * s.radius;
			auto z0 = worldCenter + vec3(0, 0, 1) * s.radius;
			auto z1 = worldCenter + vec3(0, 0, -1) * s.radius;

			//s.radius = 5.0;
			////s.objectToWorld = translate(mat4(1), vec3{ 0, s.radius, 0 });
			//s.objectToWorld = translate(mat4(1), vec3{ 0, 0, 0 });
			//s.worldToObject = inverse(s.objectToWorld);

			for (auto other : spheres) {
				float minDist = s.radius + other.radius;
				vec3 aCenter = (s.objectToWorld * vec4(0, 0, 0, 1)).xyz;
				vec3 oCenter = (other.objectToWorld * vec4(0, 0, 0, 1)).xyz;
				vec3 d = aCenter - oCenter;
				if (dot(d, d) < minDist * minDist) goto outter;
			}
			
			bounds = box::Union(bounds, x0);
			bounds = box::Union(bounds, x1);
			bounds = box::Union(bounds, y0);
			bounds = box::Union(bounds, y1);
			bounds = box::Union(bounds, z0);
			bounds = box::Union(bounds, z1);
			s.matId = materials.size();
			s.id = spheres.size();
			materials.push_back(m);
			spheres.push_back(s);

		outter:
			continue;
		}
		numSpheres = spheres.size();
		sphere_ssbo = StorageBuffer<rt::Sphere>{ spheres, 4 };
	}

	void initTriangles(vector< rt::Material>& materials) {
		rt::Material mat;
		mat.diffuse = BLUE;
		mat.ambient = BLUE;
		mat.specular = WHITE;
		mat.ior = 2.417;
		mat.kr = WHITE;
		mat.kt = WHITE;
		mat.shine = 50;
		mat.bsdf[0] = rt::BSDF_DIFFUSE;
		mat.nBxDfs += 1;

		int materialId = materials.size();
		triangleData.matId = materialId;
		
		materials.push_back(mat);

		//initNextIndex();
		auto cube = Teapot{10};
	//	auto cube = Cube{ 5 };
	//	auto model = new Model{ "C:\\Users\\Josiah\\OneDrive\\media\\models\\lte-orb\\lte-orb.obj", true, 5 };
	//	auto model = new Model{ "C:\\Users\\Josiah\\OneDrive\\media\\models\\lte-glass\\lte_glass.obj", true, 20};
	//	auto& cube = *model;

	//	numTriangles = cube.numTriangles();

		initNextIndex();
		triangle_ssbo = StorageBuffer<rt::Triangle>{ 1000000,  6};
		shading_ssbo = StorageBuffer<rt::Shading>{ 1000000,  7};

		auto aabb = cube.aabb();
		auto d = box::diagonal(aabb);
		vec3 min = aabb.min.xyz;
		vec3 max = aabb.max.xyz;
		float dy = abs(min.y);

		mat4 xform = mat4(1);
	//	xform = scale(xform, vec3(3));
		xform = translate(xform, { 0, dy, 0 });
		scene.shader("capture_triangles")([&] {
			glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, nextIndexBuffer);
			glEnable(GL_RASTERIZER_DISCARD);

			send("model", xform);
			send("materialId", materialId);
			triangle_ssbo.sendToGPU(false);
			shading_ssbo.sendToGPU(false);
			shade(cube);
			numTriangles = *(int*)glMapBuffer(GL_ATOMIC_COUNTER_BUFFER, GL_READ_ONLY);
			glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
			glDisable(GL_RASTERIZER_DISCARD);
		});
		
		triangle_ssbo.read([&](rt::Triangle* itr) {
			stringstream ss;
			for (int i = 0; i < numTriangles; i++) {
				auto triangle = *(itr+i);
				bounds = box::Union(bounds, triangle.a);
				bounds = box::Union(bounds, triangle.b);
				bounds = box::Union(bounds, triangle.c);
				//logger.info(ss.str());
				//ss.clear();
				//ss.str("");
				//triangles.push_back(tri);
			}
		});
		//glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, nextIndexBuffer);
		//auto nextIndex = (unsigned*)glMapBuffer(GL_ATOMIC_COUNTER_BUFFER, GL_READ_WRITE);
		//logger.info("nextIndex: " + to_string(*nextIndex));
		//glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
		//glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
	}

	void sendTriangles() {
		glActiveTexture(TEXTURE(triangleData.triangles.unit));
		glBindTexture(GL_TEXTURE_BUFFER, triangleData.triangles.buffer);
		send("triangleData.triangles", triangleData.triangles.unit);


		glActiveTexture(TEXTURE(triangleData.normals.unit));
		glBindTexture(GL_TEXTURE_BUFFER, triangleData.normals.buffer);
		send("triangleData.normals", triangleData.normals.unit);

		if (triangleData.hasIndices) {
			glActiveTexture(TEXTURE(triangleData.indices.unit));
			glBindTexture(GL_TEXTURE_BUFFER, triangleData.indices.buffer);
			send("triangleData.indices", triangleData.indices.unit);
		}

		send("triangleData.hasIndices", triangleData.hasIndices);
		send("triangleData.matId", triangleData.matId);
		send("triangleData.numTriangles", triangleData.numTriangles);
		send("fetchFromTexture", false);

		triangle_ssbo.sendToGPU(false);
		send("numTriangles", numTriangles);
	}

	void buildBVH() {

		bvh::BVH_SSO bvh_buffer;
		bvh::BVH_TRI_INDEX index_buffer;

		triangle_ssbo.read([&](rt::Triangle* ptr) {

			auto getBounds = [](Bounds bounds, rt::Triangle triangle) {
				bounds = box::Union(bounds, triangle.a);
				bounds = box::Union(bounds, triangle.b);
				bounds = box::Union(bounds, triangle.c);
				return bounds;
			};

			bvhRoot = bvh::build(bvhRoot, ptr, numTriangles, getBounds, index_buffer, bvh_buffer, 32);
		});

		stats.height = ds::tree::height(bvhRoot);
		stats.nodes = 0;

		auto bvh_min = ds::tree::min(bvhRoot);
		auto bvh_max = ds::tree::max(bvhRoot);

		Mesh m;
		int total = 0;
		ds::tree::traverse(bvhRoot, [&](geom::bvh::BVHBuildNode* n) {
			//	numNodes++;
			if (n->isLeaf()) {
				stats.nodes++;
				vec4 color = n->leftChild ? CYAN : MAGENTA;
				stats.primsPerNode += n->nPrimitives;
				total++;
			}
			}, ds::tree::TraverseType::IN_ORDER);
		stats.primsPerNode = stats.primsPerNode / total;
		numNodes += ds::tree::size(bvhRoot);

		bvh_ssbo = StorageBuffer<bvh::LinearBVHNode>{ bvh_buffer.nodes, 8 };
		primivite_indices_ssbo = StorageBuffer<int>{ index_buffer.data, 9 };
	}

	void initRayCounter() {
		//unsigned counters[3]{ 0, 0, 0 };
		//glGenBuffers(1, &rayCounterBuffer);
		//glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, rayCounterBuffer);
		//glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(unsigned) * 3, NULL, GL_DYNAMIC_DRAW);
		//glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(unsigned) * 3, counters);
		//glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, rayCounterBuffer);
		//glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
		rayCounterBuffer = AtomicCounterBuffer(size_t(3), 0);
		rayCounterBuffer.read([](auto counter) {
			for (int i = 0; i < 3; i++) *counter++ = 0;
		});
	}

	void initNextIndex() {
		auto startIndex = 0u;
		glGenBuffers(1, &nextIndexBuffer);
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, nextIndexBuffer);
		glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(unsigned), NULL, GL_DYNAMIC_DRAW);
		glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(unsigned), &startIndex);
		glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 1, nextIndexBuffer);
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
	}


	void preCompute() {
		//auto& light = light_ssbo.get()[0];
		//light.lightToWorld = translate(mat4(1), { scene.activeCamera().getPosition() });
		//light.worldToLight = inverse(light.lightToWorld);
		//light.position = light.lightToWorld * vec4(0, 0, 0, 1);


		light_ssbo.sendToGPU(true);
		rays.sendToGPU(false);
		bvh_ssbo.sendToGPU(false);
		primivite_indices_ssbo.sendToGPU(false);
		//triangle_ssbo.sendToGPU(true);
		//send("numTriangles", numTriangles);
		sendTriangles();
		//for (int i = 0; i < numTriangles; i++) {
		//	auto triangle = triangles[i];
		//	send("triangles[" + to_string(i) + "].a", triangle.a);
		//	send("triangles[" + to_string(i) + "].b", triangle.b);
		//	send("triangles[" + to_string(i) + "].c", triangle.c);
		//	send("triangles[" + to_string(i) + "].id", triangle.id);
		//	send("triangles[" + to_string(i) + "].matId", triangle.matId);
		//}

	//	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, rayCounterBuffer);
		rayCounterBuffer.sendToGPU();
		send("bounces", _bounces);
		send("numPlanes", numPlanes);
		send("bounds.min", vec3(bounds.min));
		send("bounds.max", vec3(bounds.max));
		send("numNodes", numNodes);
		send("cullBackFace", true);
		send("currentSample", currentSample);
		glBindTextureUnit(0, skybox->buffer);
		auto& img = checkerboard->images().front();
		glBindTextureUnit(1, img.buffer());
		send("camPos", scene.activeCamera().getPosition());
	}

	bool once = true;
	void postCompute() override {
		currentSample++;
		if (once) {
			once = false;
		////	debug_ssbo.read([&](Debug* ptr) {
		////		for (int i = 0; i < 1; i++) {
		////			auto debug = *(ptr+i);
		////			stringstream ss;
		////			ss << "size: " << sizeof(Debug) << endl;
		////			ss << "Ray debug info:\n";
		////			ss << "\to : " << debug.o.xyz << endl;
		////			ss << "\td : " << debug.d.xyz << endl;
		////			ss << "\tn  : " << debug.n.xyz << endl;
		////			ss << "\two : " << debug.wo << endl;
		////			ss << "\tcos0 : " << debug.cos0 << endl;
		////			ss << "\tf : " << debug.f << endl;
		////			ss << "\tn1 : " << debug.n1 << endl;
		////			ss << "\tn2 : " << debug.n2 << endl;
		////			ss << "\tbounce : " << debug.bounce << endl;
		////			ss << "\tid: " << debug.id << endl;
		////			ss << "\tShapeId: " << debug.shapeId << endl;
		////			logger.info(ss.str());
		////		}
		////	});

			//auto debug = debug_ssbo.getFromGPU(524800);
			//stringstream ss;
			//ss << "\n";
			//ss << "size: " << sizeof(Debug) << endl;
			//ss << "Ray debug info:\n";
			//ss << "\to : " << debug.o.xyz << endl;
			//ss << "\td : " << debug.d << endl;
			//ss << "\tn  : " << debug.n.xyz << endl;
			//ss << "\two : " << debug.wo << endl;
			//ss << "\twi : " << debug.wi << endl;
			//ss << "\tcos0 : " << debug.cos0 << endl;
			//ss << "\tf : " << debug.f << endl;
			//ss << "\tn1 : " << debug.n1 << endl;
			//ss << "\tn2 : " << debug.n2 << endl;
			//ss << "\tbounce : " << debug.bounce << endl;
			//ss << "\tid: " << debug.id << endl;
			//ss << "\tShapeId: " << debug.shapeId << endl;
			//ss << "\tt: " << debug.t << endl;
			//logger.info(ss.str());

		////	rays.read([&](rt::Ray* ptr) {
		////		auto ray = *ptr;
		////		stringstream ss;
		////		ss << "Ray[ ";
		////		ss << "o: " << ray.origin.xyz;
		////		ss << ", d: " << ray.direction.xyz;
		////		ss << ", t: " << ray.tMax;
		////		ss << " ]";
		////		logger.info(ss.str());
		////	});

		//triangle_ssbo.read([&](rt::Triangle* itr) {
		//	stringstream ss;
		//	for (int i = 0; i < numTriangles; i++) {
		//		auto tri = *(itr + i);
		//		ss << "[a : " << tri.a.xyz << ", ";
		//		ss << "b: " << tri.b.xyz << ", ";
		//		ss << "c: " << tri.c.xyz << ", ";
		//		ss << "id: " << tri.id << "], ";
		//		ss << "material: " << tri.matId << "]";
		//		logger.info(ss.str());
		//		ss.clear();
		//		ss.str("");
		//	}
		//	});
		}
	}

	void restartSampling() {
		currentSample = 0;
	}

	void processInput(const Key& key) {
		if (key.released()) {
			switch (key.value()) {
			case '+':
			case '=':
				_bounces += 1;
				break;
			case '-':
				_bounces -= 1;
				break;
			}
			_bounces = glm::clamp(_bounces, 0, MAX_BOUNCES);
		}
	}

	int bounces() {
		return _bounces;
	}

	void render() {
		skybox->render();
	}

	void log(Font& font) {
		if (showRayCount) {
			showRayCount = false;
			//glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, rayCounterBuffer);
			//auto rayCounts = (unsigned*)glMapBuffer(GL_ATOMIC_COUNTER_BUFFER, GL_READ_WRITE);
			//NumRays = rayCounts[0];
			//shadowRays = rayCounts[1];
			//testPerPixel = rayCounts[2];
			//rayCounts[0] = rayCounts[1] = rayCounts[2] = 0;
			////glFlushMappedBufferRange(GL_ATOMIC_COUNTER_BUFFER, 0, 2 * sizeof(unsigned));
			//glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
			//glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
			rayCounterBuffer.read([&](auto rayCounts) {
				NumRays = rayCounts[0];
				shadowRays = rayCounts[1];
				testPerPixel = rayCounts[2];
				rayCounts[0] = rayCounts[1] = rayCounts[2] = 0;
			});
		}

		font.render("Rays: " + toString(NumRays), 10, 100);
		font.render("Shadow Rays: " + toString(shadowRays), 10, 110);
		font.render("Intersection Test: " + toString(testPerPixel), 10, 120);

		font.render("BVH Stats: ", 10, 150);
		font.render("\theight: " + to_string(stats.height), 10, 160);
		font.render("\tNo. of Nodes: " + to_string(numNodes), 10, 170);
		font.render("\tNo. of Leaf Nodes: " + to_string(stats.nodes), 10, 180);
		font.render("\tPrimitives per Node: " + to_string(int(stats.primsPerNode)), 10, 190);
	
	}

	string toString(unsigned int n) {
		float m = std::floor(n / float(MILLION));
		if (m == 0) return to_string(n);
		if (m < 1000) return to_string(int(m)) + " million";
		
		return to_string(m / 1000) + " billion";
	}

	void update(float t) {
		static float et = 0;
		static float elapsedTime = 0;
		elapsedTime += t;
		et += t;
		if (elapsedTime > 1.0f) {
			showRayCount = true;
			elapsedTime = 0.0f;
		}
		auto pos = vec3{ cos(et) * 10, 10, sin(et) * 10 };
	////	auto pos = scene.activeCamera().getPosition();
		auto& light = *light_ssbo.get();
		light.lightToWorld = translate(mat4(1), { pos });
		light.worldToLight = inverse(light.lightToWorld);
		light.position = light.lightToWorld * vec4(0, 0, 0, 1);
	}

private:
	StorageBuffer<rt::Ray>& rays;
	StorageBuffer<rt::Plane> plane_ssbo;
	StorageBuffer<rt::LightSource> light_ssbo;
	StorageBuffer<rt::Sphere> sphere_ssbo;
	StorageBuffer<rt::Material> material_ssbo;
	StorageBuffer<rt::Triangle> triangle_ssbo;
	StorageBuffer<rt::Shading> shading_ssbo;
	StorageBuffer<bvh::LinearBVHNode> bvh_ssbo;
	StorageBuffer<int> primivite_indices_ssbo;
	StorageBuffer<Debug> debug_ssbo;
	CheckerBoard_gpu* checkerboard;
	SkyBox* skybox;
	Scene& scene;
	vec3 worldCenter;
	float worldRadius;
	int numSpheres;
	int numPlanes;
	int _bounces = MAX_BOUNCES;
	unsigned int rayCounters[3] = { 0, 0, 0 };
	int NumRays;
	int shadowRays;
	AtomicCounterBuffer rayCounterBuffer;
	GLuint nextIndexBuffer;
	bool showRayCount = false;
	size_t numTriangles = 0;
	vector<rt::Triangle> triangles;
	bvh::BVHBuildNode* bvhRoot;
	Bounds bounds;
	bvh::BVHStats stats;
	int numNodes = 0;
	int testPerPixel;
	int numPixels;
	float currentSample = 0;
};


class RayTracingScene : public Scene {
public:
	RayTracingScene() :Scene("Ray tracing scene", 1024, 1024) {
		addShader("screen", GL_VERTEX_SHADER, screen_vert_shader);
		addShader("screen", GL_FRAGMENT_SHADER, screen_frag_shader);
		addShader("skybox", GL_VERTEX_SHADER, skybox_vert_shader);
		addShader("skybox", GL_FRAGMENT_SHADER, skybox_frag_shader);
	//	addShader("skybox", GL_GEOMETRY_SHADER, skybox_geom_shader);

		addShader("stack_test", GL_VERTEX_SHADER, screen_vert_shader);
		_debug = true;
	}

	void init() override {
		fontColor(YELLOW);
		initDefaultCamera();
		activeCamera().setPosition({ 0, 0, 15 });
	//	activeCamera().lookAt({ 0, 10, 0 }, vec3(0), { 0, 0, 1 });
		camera_ssbo = gl::StorageBuffer<rt::Camera>{ rt::Camera{} };
		rayGenerator = new rt::RayGenerator{ *this, camera_ssbo };
		raytracer = new RayTracer{ *this, rayGenerator->getRaySSBO() };
		quad = ProvidedMesh{ screnSpaceQuad() };
		teapot = new Teapot;
		setBackGroundColor(BLACK);
	}

	bool once = true;

	void display() override {
		rayGenerator->compute();
		raytracer->compute();
		
//		if (once) {
//			once = false;
//			rayGenerator->getRaySSBO().read([this](rt::Ray* itr) {
//				stringstream ss;
//				auto cam = camera_ssbo.getFromGPU();
//				for (int i = 0; i < 10; i++) {
//;
//					rt::Ray ray = *(itr + i);
//					ss << "Ray[ o: " << vec3(ray.origin);
//					ss << ", d: " << vec3(ray.direction);
//					ss << ", t: " << ray.tMax <<  "]\n";
//					
//					logger.info(ss.str());
//					ss.clear();
//					ss.str("");
//				}
//			});
//		}

		shader("screen")([&] {
			raytracer->images().front().renderMode();
			glBindTextureUnit(0, raytracer->images().front().buffer());
			shade(quad);
		});

		
		//shader("capture_triangles")([&] {
		//	//	glEnable(GL_RASTERIZER_DISCARD);
		//	triangle_ssbo.sendToGPU(false);
		//	shade(teapot);
		//	//shade(quad);
		//	//	glDisable(GL_RASTERIZER_DISCARD);
		//});

		//if (once) {
		//	once = false;
		//	triangle_ssbo.read([&](rt::Triangle* itr) {
		//		stringstream ss;
		//		for (int i = 0; i < 10; i++) {
		//			auto tri = *(itr+i);
		//			ss << "[a : " << tri.a.xyz << ", ";
		//			ss << "b: " << tri.b.xyz << ", ";
		//			ss << "c: " << tri.c.xyz << ", ";
		//			ss << "id: " << tri.id << "]";
		//			logger.info(ss.str());
		//			ss.clear();
		//			ss.str("");
		//		}
		//		});
		//}
		sFont->render("Bounces: " + to_string(raytracer->bounces()), 10, 70);
		raytracer->log(*sFont);
	}

	void update(float t) override {
		raytracer->update(t);
	}

	void processInput(const Key& key) override {
		raytracer->processInput(key);
	}

private:
	rt::RayGenerator* rayGenerator;
	RayTracer* raytracer;
	StorageBuffer<rt::Camera> camera_ssbo;
	ProvidedMesh quad;
	rt::Camera* rCamera;
	Teapot* teapot;
};