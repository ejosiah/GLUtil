#pragma once

#include "../GLUtil/include/ncl/gl/Scene.h"
#include "../GLUtil/include/ncl/gl/SkyBox.h"
#include "../GLUtil/include/ncl/ray_tracing/RayGenerator.h"
#include "../GLUtil/include/ncl/ray_tracing/Ray.h"
using namespace std;
using namespace ncl;
using namespace gl;
using namespace glm;

#pragma pack(push, 1)
struct Stack {
	int size = 0;
	int top = 0;
	int data[64];
};
#pragma pack(pop)

class Background : public Compute {
public:
	Background(Scene& scene, StorageBufferObj<std::vector<ray_tracing::Ray>>& rays)
		:Compute(vec3{ scene.width() / 32.0f, scene.height() / 32.0f, 1.0f }
			, vector<Image2D>{ Image2D(scene.width(), scene.height(), GL_RGBA32F, "image", 0) }
	, & scene.shader("background"))
		, rays{ rays }{


		vector<string> skyTextures = vector<string>{
			"right.jpg", "left.jpg",
			"top.jpg", "bottom.jpg",
			"front.jpg", "back.jpg"
		};

		string root = "C:\\Users\\Josiah\\OneDrive\\media\\textures\\skybox\\001\\";
		transform(skyTextures.begin(), skyTextures.end(), skyTextures.begin(), [&root](string path) {
			return root + path;
			});

		skybox = SkyBox::create(skyTextures, 0, scene);
		stack_ssbo = StorageBufferObj<vector<Stack>>{ scene.width() * scene.height(), 2 };
		scene.shader("background")([&] {
			stack_ssbo.sendToGPU(false);
		});

	}

	void preCompute() {
		rays.sendToGPU(false);
		stack_ssbo.sendToGPU(false);
		glBindTextureUnit(1, skybox->buffer);
	}

	void render() {
		skybox->render();
	}

private:
	StorageBufferObj<std::vector<ray_tracing::Ray>>& rays;
	StorageBufferObj<vector<Stack>> stack_ssbo;
	SkyBox* skybox;
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
	}

	void init() override {
		initDefaultCamera();
		camera_ssbo = gl::StorageBufferObj<ray_tracing::Camera>{ ray_tracing::Camera{} };
		rayGenerator = new ray_tracing::RayGenerator{ *this, camera_ssbo };
		background = new Background{ *this, rayGenerator->getRaySSBO() };
		quad = ProvidedMesh{ screnSpaceQuad() };
		setBackGroundColor(BLACK);
	}

	bool once = true;
	void display() override {
		rayGenerator->compute();

		for (int i = 0; i < 50; i++) {
			background->compute();
		}

		shader("screen")([&] {
			background->images().front().renderMode();
			glBindTextureUnit(0, background->images().front().buffer());
			shade(quad);
		});

		//shader("stack_test")([&] {
		//	shade(quad);
		//});
	}

private:
	ray_tracing::RayGenerator* rayGenerator;
	Background* background;
	Logger& logger = Logger::get("ray");
	StorageBufferObj<ray_tracing::Camera> camera_ssbo;
	ProvidedMesh quad;
};

template<>
struct ncl::gl::ObjectReflect<std::vector<Stack>> {

	static GLsizeiptr sizeOfObj(std::vector<Stack>& stacks) {
		auto size = sizeof(Stack);
		return GLsizeiptr(size * stacks.size());
	}

	static void* objPtr(std::vector<Stack>& stacks) {
		return &stacks[0];
	}

	static GLsizeiptr sizeOf(int count) {
		return GLsizeiptr(count * sizeof(Stack));
	}
};