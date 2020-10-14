#include "../GLUtil/include/ncl/configured_logger.h"
#include "../GLUtil/include/ncl/gl/GlfwApp.h"
#include <cuda_gl_interop.h>
#include "../GLUtil/include/ncl/gl/Resolution.h"
#include "PhysicsScene.h"

int main() {
	//cudaGLSetGLDevice(0);
	auto scene = new PhysicsScene("Physics Scene");
	return start(scene);
}