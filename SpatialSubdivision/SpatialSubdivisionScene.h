#pragma once

#include <iterator>
#include "../GLUtil/include/ncl/gl/Scene.h"

using namespace std;
using namespace glm;
using namespace ncl;
using namespace gl;

constexpr float SQRT_TWO = 1.4142135623730950488016887242097;

using BoundingBox = geom::bvol::AABB2;
namespace box = geom::bvol::aabb;

class SpatialSubdivisionScene : public Scene {
public:
	SpatialSubdivisionScene() :Scene("Spatial Subdivision") {}

	void init() override {
		setBackGroundColor(WHITE);
		generateSpheres();
		//generateGrid();
		createOrigin();
	}

	void generateSpheres() {
		spheres.resize(numSpheres);

		auto rngR = ncl::rngReal(0.1, 1);
		auto rngX = ncl::rngReal(-0, 4);

		vector<mat4> xforms;
		auto scene = BoundingBox{};
		generate(begin(spheres), end(spheres), [&]() {
			auto c = vec3{ rngX(), rngX(), rngX() };
			auto r = rngR();
			auto s =  vec4{ c, r };
			maxR = std::max(s.w, maxR);
			xforms.push_back(translate(mat4(1), vec3(s)) * scale(mat4(1), vec3(r)));

			auto dh = 0.5 * SQRT_TWO;

			auto x0 = c + vec3(dh, 0, 0) *  r;
			auto x1 = c + vec3(-dh, 0, 0) * r;
			auto y0 = c + vec3(0, dh, 0) *  r;
			auto y1 = c + vec3(0, -dh, 0) * r;
			auto z0 = c + vec3(0, 0, dh) *  r;
			auto z1 = c + vec3(0, 0, -dh) * r;

			auto bounds = BoundingBox{};
			
			bounds = box::Union(bounds, x0);
			bounds = box::Union(bounds, x1);
			bounds = box::Union(bounds, y0);
			bounds = box::Union(bounds, y1);
			bounds = box::Union(bounds, z0);
			bounds = box::Union(bounds, z1);
			scene = box::Union(scene, bounds);

			auto boundingBox = ProvidedMesh{ aabbOutline(bounds) };
			boundingBoxes.push_back(std::move(boundingBox));

			return s;
		});

		auto d = box::diagonal(scene);
		auto l = std::max(d.x, std::max(d.y, d.z));

		sceneBounds = ProvidedMesh{ aabbOutline(scene) };
		grid = new Cylinder{ 0.1, l, 20, 20, vec4{0.35, 0.41, 0.70, 1.0} };
		sphere = Sphere{ 0.5, 20, 20, GREEN, numSpheres, xforms };
	}

	void generateGrid() {
		assert(!spheres.empty() && maxR > 0);
		Mesh mesh;
		auto d = maxR * 2;
		auto halfWidth = grids * maxR;
		for (int j = 0; j < grids; j++) {
			for (int i = 0; i < grids; i++) {
				float dy = i * d;
				float dz = j * d;
				vec3 a = { -halfWidth, -halfWidth + dy, -halfWidth + dz };
				vec3 b = { halfWidth, -halfWidth + dy, halfWidth + dz };
				mesh.positions.push_back(a);
				mesh.positions.push_back(b);
				mesh.colors.push_back(BLACK);
			}
		}

		for (int i = 0; i < grids; i++) {
			for (int j = 0; j < grids; j++) {
				float dx = i * d;
				float dz = j * d;
				vec3 a = { -halfWidth + dx, -halfWidth, -halfWidth + dz };
				vec3 b = { halfWidth + dx, halfWidth, halfWidth + dz };
				mesh.positions.push_back(a);
				mesh.positions.push_back(b);
				mesh.colors.push_back(BLACK);
			}
		}

		mesh.primitiveType = GL_LINES;
	}

	void createOrigin() {
		x = new Vector{ vec3(5, 0, 0), vec3(0), 1.0f, RED };
		y = new Vector{ vec3(0, 5, 0), vec3(0 ), 1.0f, GREEN };
		z = new Vector{ vec3(0, 0, 5), vec3(0), 1.0f, BLUE };
	}

	void display() {
		glLineWidth(5.0f);
		shader("flat")([&] {
			send(activeCamera());
			shade(sphere);
			for (auto& bounds : boundingBoxes) {
				shade(bounds);
			}
			//shade(sceneBounds);
			//shade(grid);
			shade(x);
			shade(y);
			shade(z);
		});
	}

private:
	Sphere sphere;
	vector<vec4> spheres;
	vector<ProvidedMesh> boundingBoxes;
	ProvidedMesh sceneBounds;
	Cylinder* grid;
	Vector* x;
	Vector* y;
	Vector* z;
	const unsigned numSpheres = 20;
	const int grids = 5;
	float maxR = numeric_limits<float>::min();
};