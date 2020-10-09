#pragma once

#include "../GLUtil/include/ncl/gl/Scene.h"

using namespace ncl;
using namespace gl;

class GPUInfoScene : public Scene {
public:
	GPUInfoScene() :Scene("GPU Info", 1024, 1440)
	{
		_requireMouse = false;
		_hideCursor = false;
	}

	void init() override {
		setForeGroundColor(WHITE);
		setBackGroundColor(BLACK);
		fontColor(WHITE);
		fontSize(20);

		getInfo();
	}

	void getInfo() {
		VendorInfo();
		MemoryInfo();
		ComputeInfo();
	}

	void VendorInfo() {
		int ires;
		glGetIntegerv(GL_MAJOR_VERSION, &ires);
		sbr << "OpenGL version: " << ires << ".";

		glGetIntegerv(GL_MINOR_VERSION, &ires);
		sbr << ires << "\n";
		sbr << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";

		sbr << "\nVendor Info:\n";
		sbr << "\tVendor: " << glGetString(GL_VENDOR) << "\n";
		sbr << "\tRenderer: " << glGetString(GL_RENDERER) << "\n";

		//glGetIntegerv(GL_NUM_EXTENSIONS, &ires);
		//sbr << "\nExtensions (" << ires <<  "):\n";
		//for (int i = 0; i < ires; i++) {
		//	sbr << "\t" << glGetStringi(GL_EXTENSIONS, i) << "\n";
		//}
	}

	void MemoryInfo() {
		int ires;
		sbr << "\nMemory:";

		glGetIntegerv(GL_MAX_IMAGE_UNITS, &ires);
		sbr << "\n\tMax Vertex Image Units: " << ires;

		glGetIntegerv(GL_MAX_VERTEX_IMAGE_UNIFORMS, &ires);
		sbr << "\n\tMax Vertex Image Uniforms: " << ires;

		glGetIntegerv(GL_MAX_TESS_CONTROL_IMAGE_UNIFORMS, &ires);
		sbr << "\n\tMax Tess Control Image Uniforms: " << ires;

		glGetIntegerv(GL_MAX_TESS_EVALUATION_IMAGE_UNIFORMS, &ires);
		sbr << "\n\tMax Tess Evalulation Image Uniforms: " << ires;

		glGetIntegerv(GL_MAX_GEOMETRY_IMAGE_UNIFORMS, &ires);
		sbr << "\n\tMax Geometry Image Uniforms: " << ires;

		glGetIntegerv(GL_MAX_FRAGMENT_IMAGE_UNIFORMS, &ires);
		sbr << "\n\tMax Fragment Image Uniforms: " << ires;

		glGetIntegerv(GL_MAX_COMBINED_IMAGE_UNIFORMS, &ires);
		sbr << "\n\tMax Combined Image Uniforms: " << ires;

		glGetIntegerv(GL_MAX_COMBINED_IMAGE_UNITS_AND_FRAGMENT_OUTPUTS, &ires);
		sbr << "\n\tMax Combined Image/Fragment output: " << ires;
	}

	void ComputeInfo() {
		int res[3];
		sbr << "\nCompute:";

		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, res);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &res[1]);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &res[2]);
		sbr << "\n\t" << "Max Workgroup size: (" << res[0] << ", " << res[1] << ", " << res[2] << ")";

		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, 0, res);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, 1, &res[1]);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, 2, &res[2]);
		sbr << "\n\t" << "Max Workgroup invocations: (" << res[0] << ", " << res[1] << ", " << res[2] << ")";

		
		glGetIntegerv(GL_MAX_COMPUTE_SHARED_MEMORY_SIZE, &res[0]);
		sbr << "\n\t" << "Shared memory size: " << res[0];
	}

	void display() override {
		sFont->render(sbr.str(), 20, 20);
	}

private:
	std::stringstream sbr;
};