#pragma once
#include "../GLUtil/include/ncl/gl/Shape.h"
#include <glm/glm.hpp>
#include <limits>
#include <memory>
#include "../fsim/fields.h"

class ScalaFieldObject : public ncl::gl::Shape {
public:
	ScalaFieldObject(const fsim::ScalarField& field, int p, int q, const glm::vec4& color, int width = 2):
		ncl::gl::Shape(createMesh(field, p, q, color, width), 1){

	}

protected:
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
			float g = smoothstep(0.0f, 0.4f, ratio) - smoothstep(0.8f, 1.0f, ratio);;
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