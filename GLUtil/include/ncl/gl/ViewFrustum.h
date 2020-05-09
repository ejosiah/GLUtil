#pragma once

#include "Shape.h"
#include "Cube.h"

namespace ncl {
	namespace gl {
		class ViewFrustum : public Shape {
		public:
			ViewFrustum() = default;

			ViewFrustum(const ViewFrustum&) = delete;

			ViewFrustum(ViewFrustum&& source) noexcept;

			ViewFrustum& operator=(const ViewFrustum&) = delete;

			ViewFrustum& operator=(ViewFrustum&& source) noexcept; 

			friend void transfer(ViewFrustum& source, ViewFrustum& dest);

			static ViewFrustum perspective(glm::mat4 view, float fov, float a, float near_p, float far_p, glm::vec4 color = WHITE);

			static ViewFrustum* ortho(glm::vec3 pos, glm::vec3 target, glm::vec3 up, float left, float right, float bottom, float top, float near_p, float far_p, glm::vec4 color = WHITE);

		private:
			ViewFrustum(glm::mat4 view, float fov, float a, float near_p, float far_p, glm::vec4 color);

			ViewFrustum(glm::vec3 pos, glm::vec3 target, glm::vec3 up, float left, float right, float bottom, float top, float near_p, float far_p, glm::vec4 color);

			std::vector<Mesh> createOrtho(glm::vec3 pos, glm::vec3 target, glm::vec3 up, float left, float right, float bottom, float top, float near_p, float far_p, glm::vec4 color);

			std::vector<Mesh> createPerspective(glm::mat4 view, float fov, float a, float near_p, float far_p, glm::vec4 color);

			glm::mat4 worldView;

		};

		ViewFrustum::ViewFrustum(ViewFrustum&& source) noexcept 
			: Shape(dynamic_cast<Shape&&>(source)) {
			transfer(source, *this);
		}

		ViewFrustum& ViewFrustum::operator=(ViewFrustum&& source) noexcept {
			Shape::transfer(dynamic_cast<Shape&>(source), dynamic_cast<Shape&>(*this));
			transfer(source, *this);
			return *this;
		}

		void transfer(ViewFrustum& source, ViewFrustum& dest) {
			dest.worldView = source.worldView;
		}

		ViewFrustum ViewFrustum::perspective(glm::mat4 view, float fov, float a, float near_p, float far_p, glm::vec4 color) {
			return ViewFrustum(view, fov, a, near_p, far_p, color);
		}


		ViewFrustum* ViewFrustum::ortho(glm::vec3 pos, glm::vec3 target, glm::vec3 up, float left, float right, float bottom, float top, float near_p, float far_p, glm::vec4 color) {
			return new ViewFrustum(pos, target, up, left, right, bottom, top, near_p,  far_p, color);
		}


		ViewFrustum::ViewFrustum(glm::mat4 view, float fov, float a, float near_p, float far_p, glm::vec4 color) 
			:Shape(createPerspective(view, fov, a, near_p, far_p, color)) {}

		ViewFrustum::ViewFrustum(glm::vec3 pos, glm::vec3 target, glm::vec3 up, float left, float right, float bottom, float top, float near_p, float far_p, glm::vec4 color)
			: Shape(createOrtho(pos, target, up, left, right, bottom, top, near_p, far_p, color)) {}

		std::vector<Mesh> ViewFrustum::createOrtho(glm::vec3 pos, glm::vec3 target, glm::vec3 up, float left, float right, float bottom, float top, float near_p, float far_p, glm::vec4 color) {
			float width = ::abs(right - left);
			float length = ::abs(far_p - near_p);
			float height = ::abs(top - bottom);

			auto cube = Cube{ 1 };
			auto mesh = cube.getMeshes().at(0);
			mesh.colors = std::vector{ mesh.positions.size(), color };

			auto forward = glm::normalize(target - pos);
			auto axis = glm::cross(forward, up);
			auto camUp = glm::cross(axis, forward);

			float angle = glm::acos(dot(up, camUp));

			auto model = glm::mat4{ 1 };
			auto moveInPlace = glm::mat4{ 1 };
			auto rot = glm::rotate(glm::mat4{ 1 }, angle, axis);

		//	moveInPlace *= glm::translate(glm::mat4{ 1 }, { 0, height / 2, 0 });
			moveInPlace *= glm::translate(glm::mat4{ 1 }, pos + glm::vec3{ 0, 0, near_p + 5 });
			moveInPlace *= glm::translate(glm::mat4{ 1 }, { 0, -height / 2, 0 });
			model = glm::scale(glm::mat4{ 1 }, { width, height, length });

			model = moveInPlace * model;
	
			
			mesh.xforms.push_back(model);

			return { mesh };
		}

		std::vector<Mesh> ViewFrustum::createPerspective(glm::mat4 view, float fov, float a, float near_p, float far_p, glm::vec4 color) {
			return {};
		}
	}
}