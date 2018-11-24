#pragma once

#include <iostream>
#include <string>
#include "../GLUtil/include/ncl/gl/Scene.h"
#include "../GLUtil/include/ncl/gl/compute.h"
#include "../GLUtil/include/ncl/gl/CopyBuffer.h"
#include "../GLutil/include/ncl/gl/shaders.h"
#include "../GLutil/include/ncl/gl/shader_binding.h"
#include "../GLutil/include/ncl/gl/util.h"

using namespace std;
using namespace ncl;
using namespace gl;
using namespace glm;

const string USERNAME = getEnv("username");

class lightController : public _3DMotionEventHandler {
public:
	lightController(vec4& pos) :pos{ pos } {
		velocity = vec3(5.0f);
	}

	Logger logger = Logger::get("3D Motion Event");

	virtual void onMotion(const _3DMotionEvent& event) override {
		direction = clamp(event.translation, vec3(-1), vec3(1));
	};
	virtual void onNoMotion() override {
		direction = vec3(0);
	};

	virtual void update(float dt) {
		auto v = velocity * direction;
		vec3 newP = pos.xyz + v * dt;
		pos = vec4(newP, pos.w);
	}

private:
	vec4 & pos;
	vec3 direction;
	vec3 velocity;
};

class Cloth : public Compute, public CopyBuffer {
public:
	Cloth(int rows, int cols, int size, vec4& lc, const glm::vec4& color = randomColor(),  mat4 transform = mat4(1)):
		Compute(ivec3(1)), _rows(rows), _cols(cols), _size(size), xform(transform), lightCenter(lc){
		string source = mass_spring_comp_shader;
		auto colPos = source.find("$cols");	
		source = source.replace(colPos, 5, to_string(cols + 1));
		auto rowPos = source.find("$rows");
		source = source.replace(rowPos, 5, to_string(rows + 1));
		_shader = Compute::from(source);
		initShader(rows, cols, size);
	}

	void initShader(int rows, int cols, int size) {
		vec2 patch_size{ float(size) / (cols+1), float(size) / (cols+1) };
		vec3 gravity{ 0.0f, -0.00981f, 0.0f };
	//	vec3 gravity{ 0.0f, -1.666f, 0.0f };
		(*_shader)([&] {
			send("patch_length", patch_size);
			send("gravity", gravity);
			send("m", 1.0f);
		});
	}

	virtual void init() {
	//	pos_buffer[0] = copy(pos);
	//	pos_buffer[1] = copy(pos);

		
		auto positions = vector<vec4>();
		auto indicies = vector<GLuint>();

		createObjects(positions, indicies);
		num_indices = indicies.size();

		glGenBuffers(1, &index_buffer);
		
		for (int i = 0; i < 2; i++) {

			glGenBuffers(1, &pos_buffer[i]);
			glGenBuffers(1, &prev_pos_buffer[i]);
			glBindBuffer(GL_ARRAY_BUFFER, prev_pos_buffer[i]);
			glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(vec4), &positions[0],GL_DYNAMIC_READ);

			glGenVertexArrays(1, &vaoId[i]);

			glBindVertexArray(vaoId[i]);

			glBindBuffer(GL_ARRAY_BUFFER, pos_buffer[i]);
			glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(vec4), &positions[0], GL_DYNAMIC_READ);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_indices * sizeof(GLuint), &indicies[0], GL_STATIC_DRAW);

			glBindVertexArray(0);

		}

		initCoefficients();	

	}

	void createObjects(vector<vec4>& X, vector<GLuint>& indices) {
		size_t i = 0, j = 0, count = 0;
		int l1 = 0, l2 = 0;
		int v = _rows + 1;
		int u = _cols + 1;
		int total_points = u * v;
		int hsize = _size / 2;
		stringstream ss;
		ss << "Total triangles: " << _cols * _rows * 2;
		Logger::get("Cloth").info(ss.str());

		//resize the cloth indices, position, previous position and force vectors
		indices.resize(_cols*_rows * 2 * 3);
		X.resize(total_points);


		//fill in positions
		for (int j = 0;j <= _rows;j++) {
			for (int i = 0;i <= _cols;i++) {
				X[count] = glm::vec4(((float(i) / (u - 1)) * 2 - 1)* hsize, _size + 1, ((float(j) / (v - 1))* _size), 1);
				count++;
			}
		}

		//fill in indices
		GLuint* id = &indices[0];
		for (int i = 0; i < _rows; i++) {
			for (int j = 0; j < _cols; j++) {
				int i0 = i * (_cols + 1) + j;
				int i1 = i0 + 1;
				int i2 = i0 + (_cols + 1);
				int i3 = i2 + 1;
				if ((j + i) % 2) {
					*id++ = i0; *id++ = i2; *id++ = i1;
					*id++ = i1; *id++ = i2; *id++ = i3;
				}
				else {
					*id++ = i0; *id++ = i2; *id++ = i3;
					*id++ = i0; *id++ = i3; *id++ = i1;
				}
			}
		}
	}


	virtual void preCompute() override {
		_shader->sendUniform1f("r", (float(_size)/(_cols+1)) * 1.2);
		_shader->sendUniform4fv("c", 1, &lightCenter.x);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, pos_buffer[front]);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, pos_buffer[back]);

		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, prev_pos_buffer[front]);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, prev_pos_buffer[back]);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, coeffs_buffer);
	}


	virtual void compute() override {
		for (int i = 0; i < numIterations; i++) {
			Compute::compute();
		//	swapBuffers();
		}
	}

	void swapBuffers() {
		std::swap(front, back);
	}

	 void render(Shader& shader) {
		 
		 glBindVertexArray(vaoId[front]);
		 glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, 0);
		 glBindVertexArray(0);
		 
		// plane->draw(shader);
	}


	void initCoefficients() {
		coeffs.vd = -0.05f;
		coeffs.ks[0] = 10.5f;
		coeffs.ks[1] = 0.25f;
		coeffs.ks[2] = 0.25f;
		coeffs.kd[0] = -5.5f * 0.001;
		coeffs.kd[1] = -0.25 * 0.001;
		coeffs.kd[2] = -0.25f * 0.001;
		(*_shader)([&] {
			glGenBuffers(1, &coeffs_buffer);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, coeffs_buffer);
			glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(coeffs), &coeffs.vd, GL_STATIC_READ);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, coeffs_buffer);
			
		});
	}

private:
	int _rows;
	int _cols;
	int _size;
	mat4 xform;
	int numIterations = 20;
	GLuint prev_pos_buffer[2];
	GLuint pos_buffer[2];
	GLuint index_buffer;
	GLuint coeffs_buffer;
	struct Coefficients {
		float vd;
		float ks[3];
		float kd[3];
	} coeffs;
	GLuint front = 0;
	GLuint back = 1;
	GLuint vaoId[2];
	GLuint num_indices;
	Plane* plane;
	vec4& lightCenter;
};


class ClothSimScene : public Scene {
public:

	ClothSimScene() :Scene("Cloth Simulation", 1024, 1024) {
	}

	void init() override {
		auto noOfPoints = 21;
		auto size = 4.f;
		auto patch_size = size / (noOfPoints + 1);
		auto lightRadius = patch_size * 1.2;
		light[0].position = { 0, 2, 2, 0 };
		light[0].transform = true;
		light[0].spotAngle = 180.0f;
		light[0].spotDirection = { 0, -1, -1, 1 };
	//	lightModel.localViewer = true;
		lightModel.colorMaterial = false;
		lightModel.useObjectSpace = true;
		lightObj = new Sphere(lightRadius, 10, 10, WHITE);
		delete _motionEventHandler;
		_motionEventHandler = new lightController(light[0].position);

		plane = new Plane({ {0, 1, 0} , 0 }, 40, 40, 100, 100, 0, WHITE);
		wallPlane = new Plane({ {0, 0, 1}, -2 }, 40, 40, 30, 30, 2, WHITE);
		wallPlane->material().specular = vec4{ 0.1f, 0.1f, 0.1f, 1.0f };
		wallPlane->material().shininess = 0.0f;
		globe = new Hemisphere(20, 50, 50, RED);
		globe->material().ambient = vec4(0);
		globe->material().diffuse = vec4(0.5, 0, 0, 1);
		globe->material().specular = vec4(0.7, 0.6, 0.6, 1.0);
		globe->material().shininess = 0.25f;
		mat4 mat = translate(mat4(1), { 0, 5, 0 });
		cloth = new Cloth(21, 21, 4, light[0].position, BLACK, mat);
		cloth->init();
	//	board = new CheckerBoard_gpu(1024, 1024);
	//	board->compute();
		glPointSize(5.0);
		
		getActiveCameraController().setModelHeight(2);
		activeCamera().lookAt({ 0, 0, 11 }, vec3(0), { 0, 1, 0 });
		setBackGroundColor(GRAY);
		wood_floor = new Texture2D("C:\\Users\\" + USERNAME + "\\OneDrive\\media\\textures\\wood_floor.jpg", 0, "image0", GL_RGBA8, GL_RGBA, glm::ivec2{ GL_REPEAT }, glm::ivec2{ GL_NEAREST });
		wood_floor_nm = new Texture2D("C:\\Users\\" + USERNAME + "\\OneDrive\\media\\textures\\wood_floor_nm.jpg", 1, "image1", GL_RGBA8, GL_RGBA, glm::ivec2{ GL_REPEAT }, glm::ivec2{ GL_NEAREST });
		
		wall = new Texture2D("C:\\Users\\" + USERNAME + "\\OneDrive\\media\\textures\\Stone_Wall\\STW_sq_001_2048x2048.jpg", 2, "image2", GL_RGBA8, GL_RGBA, glm::ivec2{ GL_REPEAT }, glm::ivec2{ GL_NEAREST });
		wall_nmap = new Texture2D("C:\\Users\\" + USERNAME + "\\OneDrive\\media\\textures\\Stone_Wall\\STW_sq_001_2048x2048_normal_map.jpg", 1, "image3", GL_RGBA8, GL_RGBA, glm::ivec2{ GL_REPEAT }, glm::ivec2{ GL_NEAREST });

		cube = new Cube(30);

	}

	void display() override {

		cloth->compute();
		auto cam0 = activeCamera();
		light[0].spotDirection = vec4(activeCamera().getViewDirection(), 1.0);
		light[0].ki = 0;
		light[0].kq = 0;

		shader("flat")([&](Shader& s) {
			send(activeCamera());
			glDisable(GL_CULL_FACE);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			cloth->render(s);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glEnable(GL_CULL_FACE);
		});

		glDisable(GL_CULL_FACE);
		shader("default")([&] {	
			send("activeTextures[0]", true);
			send(light[0]);
			send(activeCamera());
			send(lightModel);

			send("uvMappedToSize", true);
			send(wood_floor);
			send(wood_floor_nm);
			shade(plane);
		});

		shader("default")([&] {
			send("activeTextures[0]", false);
			send("activeTextures[2]", true);
			send(light[0]);
			send(activeCamera());
			send(lightModel);
			send("uvMappedToSize", true);
			send(wall);
			send(wall_nmap);
			shade(wallPlane);
		});

		
		if (light[0].on) {
			shader("flat")([&] {
				mat4 mat = translate(mat4(1), vec3(light[0].position));
				send(activeCamera(), mat);
				shade(lightObj);
			});
		}

	}

	void update(float dt) override {
		dynamic_cast<lightController*>(_motionEventHandler)->update(dt);
	} 

	void resized() override {
		cam.projection = perspective(radians(60.f), aspectRatio, 1.0f, 100.f);
		activeCamera().perspective(60.f, aspectRatio, 1.0f, 100.f);
	}

	void processInput(const Key& key) override {
		if ((key.value() == 'o' || key.value() == 'O') && key.pressed()) lightModel.useObjectSpace = !lightModel.useObjectSpace;
	}

private:
	Plane * plane;
	Plane* wallPlane;
	Texture2D* wall;
	Texture2D* wall_nmap;
	Texture2D* wood_floor;
	Texture2D* wood_floor_nm;
	Cloth* cloth;
	Sphere* lightObj;
	Hemisphere* globe;
	Cube* cube;
	CheckerBoard_gpu* board;
	float yaw = 0;
	float pitch = 10;
	float dist = -11;
};