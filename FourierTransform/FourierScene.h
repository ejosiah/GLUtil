#pragma once

#include "../GLUtil/include/ncl/gl/Scene.h"
#include "../GLUtil/include/ncl/geom/AABB.h"
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <functional>
#include <random>

using namespace std;
using namespace glm;
using namespace ncl;
using namespace gl;

using AABB = geom::bvol::AABB;
static default_random_engine engine;
static uniform_real_distribution<float> dist(0, 1);
static const vec4 gridColor = { 0.46f, 0.73f, 0.86, 1.0f };
static const float PI = glm::pi<float>();
static const float TWO_PI = glm::two_pi<float>();

class Axis : public Drawable {
public:
	friend class Graph;
	Axis(Scene& scene, int size, int numTerms, int dir = 0, int spacing = 1, bool padLast = false, vec2 tail = vec2(0), bool withHead = false, float lineWidth = 10.0f, vec4 color = WHITE)
		:scene(scene), lineWidth(lineWidth)  {

		float padding = float(size) / numTerms;
		float gap = padding / spacing;
		_extraSpace = padLast ?  2 * gap : 0;
		size += _extraSpace;

		mat2 rotor = dir == 1? mat2(0, 1, -1, 0) : mat2(1);

		Mesh m;
		vec3 origin = vec3(tail * padding, 0);
		_tail = tail;
		m.positions.push_back(origin);
		m.positions.push_back(vec3(size, 0, 0) + origin);
		m.colors.push_back(color);
		m.colors.push_back(color);
		
		int numGrids = numTerms * spacing;
		numGrids = padLast ? numGrids + 2 : numGrids;
		for (int i = 1; i < numGrids; i++) {
			if (i%spacing == 0) {
				auto p0 = vec3{ i * gap, -10, 0 } + origin;
				auto p1 = vec3{ i * gap, 10, 0 } + origin;
				m.positions.push_back(p0);
				m.positions.push_back(p1);
			}
			else {
				auto p0 = vec3{ i * gap, -5, 0 };
				auto p1 = vec3{ i * gap, 5, 0 };
				m.positions.push_back(p0);
				m.positions.push_back(p1);
			}
			m.colors.push_back(color);
			m.colors.push_back(color);
		}

		for (auto& pos : m.positions) {
			pos = vec3(rotor * pos.xy, 0);
			bounds.add(pos);
		}

		m.primitiveType = GL_LINES;
		line = new ProvidedMesh(m);

		head = nullptr;
		if (withHead) {
			m.clear();
			m.positions.push_back(vec3{ size, -5, 0 } + origin);
			m.positions.push_back(vec3{ size + 15, 0, 0 } +origin);
			m.positions.push_back(vec3{ size, 5, 0 } +origin);
			m.colors = vector<vec4>{ 3, color };
			m.primitiveType = GL_TRIANGLES;

			for (auto& pos : m.positions) {
				pos = vec3(rotor * pos.xy, 0);
				bounds.add(pos);
			}
			head = new ProvidedMesh(m);
		}
		font = Font::Arial(20, 0, color);
		_size = size;
		_numTerms = padLast ? numTerms + 0.5 : numTerms;
		_spacing = spacing;
	}

	virtual void draw(Shader& shader) override {
		glLineWidth(lineWidth);
		shade(line);
		if(head) shade(head);
	}

	virtual void add(vec3 p) {

	}

	int _size;
	float _numTerms;
	int _spacing;
	int _extraSpace;
	vec2 _tail;
	AABB bounds;

private:
	ProvidedMesh* line;
	ProvidedMesh* head;
	Font* font;
	Scene& scene;
	float lineWidth;

};

class Border : public Drawable {
public:
	Border(AABB bounds, vec4 color){
		vec3 d = bounds.diagonal();
		Mesh m;

		m.positions.push_back(bounds.min());
		m.positions.push_back(bounds.min() + vec3(d.x, 0, 0));
		m.positions.push_back(bounds.max());
		m.positions.push_back(bounds.max() - vec3(d.x, 0, 0));
		m.colors = vector<vec4>{ 4, color };
		m.primitiveType = GL_LINE_LOOP;

		pMesh = make_unique<ProvidedMesh>(m);

	}

	virtual void draw(Shader& shader) override {
		shade(pMesh.get());
	}
private:
	unique_ptr<ProvidedMesh> pMesh;
};

class Graph : public Drawable {
public:
	Graph(vector<ProvidedMesh*> dataPoints, Axis* x, Axis* y, bool gridOn = false, bool useBorder = false)
		:dataPoints(dataPoints), xAxis(x), yAxis(y), gridOn(gridOn), border(nullptr), width(x->_size), height(y->_size) {
		AABB bounds;
		if (gridOn) {
			Mesh m;
			buildGrid(m, *x, y->_size);
			buildGrid(m, *y, x->_size, mat2(0, 1, -1, 0));
			grid = new ProvidedMesh(m);
			m.clear();
			buildMiniGrid(m, *x, y->_size);
			buildMiniGrid(m, *y, x->_size, mat2(0, 1, -1, 0));
			miniGrids = new ProvidedMesh(m);
		}
		if (useBorder) {
			bounds.add(x->bounds.min());
			bounds.add(x->bounds.max());
			bounds.add(y->bounds.min());
			bounds.add(y->bounds.max());

			vec3 v = bounds.min();
			v = v + glm::sign(v) * 20.0f;
			bounds.add(v);

			v = bounds.max();
			v = v + glm::sign(v) * 20.0f;
			bounds.add(v);

			border = new Border(bounds, iColor(95, 151, 137));
		}
	}

	void buildGrid(Mesh& m, Axis& axis, int length, mat2 rotor = mat2(1)) {
		float padding = float(axis._size) / axis._numTerms;
		int numGrids = axis._numTerms * axis._spacing;
		numGrids = axis._extraSpace > 0 ? numGrids + 2 : numGrids;

		vec3 origin = vec3(axis._tail * padding, 0);

		for (int i = 0; i < (numGrids + 1); i++) {
			
			auto p0 = rotor * (vec2{ i * padding, -(length + padding) * 0.5 }  + origin.xy);
			auto p1 = rotor * (vec2{ i * padding, (length + padding) * 0.5 } +origin.xy);
			m.positions.push_back(vec3(p0, 0));
			m.positions.push_back(vec3(p1, 0));
			vec4 color = i == numGrids / 2 ? WHITE : gridColor;
			m.colors.push_back(color);
			m.colors.push_back(color);
		}
		m.primitiveType = GL_LINES;
	}

	void buildMiniGrid(Mesh& m, Axis& axis, int length, mat2 rotor = mat2(1)) {
		float padding = float(axis._size) / axis._numTerms;
		int numGrids = axis._numTerms * axis._spacing;
		numGrids = axis._extraSpace > 0 ? numGrids + 2 : numGrids;

		vec3 origin = vec3(axis._tail * padding, 0);

		vec3 c = gridColor.xyz;
		vec4 color = vec4(mix(vec3(0), c, 0.5f), 1.0);

		for (int i = 1; i < numGrids + 1; i++) {
			auto p0 = rotor * (vec2{ i * padding - (padding * 0.5), -(length + padding) * 0.5 }  +origin.xy);
			auto p1 = rotor * (vec2{ i * padding - (padding * 0.5), (length + padding) * 0.5 } +origin.xy);
			m.positions.push_back(vec3(p0, 0));
			m.positions.push_back(vec3(p1, 0));
			m.colors.push_back(color);
			m.colors.push_back(color);
		}
		m.primitiveType = GL_LINES;
	}

	virtual void draw(Shader& shader) override {
		if (border) shade(border);
		if (gridOn) {
			shade(grid);
			shade(miniGrids);
		}
		else {
			shade(xAxis);
			shade(yAxis);
		}
		for (auto dp : dataPoints) shade(dp);

	}

	virtual void update(float t) {

	}



	vector<ProvidedMesh*> dataPoints;
	Axis* xAxis;
	Axis* yAxis;
	bool gridOn;
	Border* border;
	ProvidedMesh* grid;
	ProvidedMesh* miniGrids;
	const float height;
	const float width;
	mat4 position;

};


class Sampler {
public:
	vector<float> sample1D(function<float(float)> f, float sampleRate, float numSamples) {
		vector<float> samples;
		float t = 0;
		float dt = 1 / sampleRate;
		for (int i = 0; i < numSamples; i++) {
			samples.push_back(f(t));
			t += dt;
		}
		return samples;
	}

	vector<vec2> sample2D(function<vec2(float)> f, float sampleRate, float numSamples) {
		vector<vec2> samples;
		float t = 0;
		float dt = 1 / sampleRate;
		for (int i = 0; i < numSamples; i++) {
			samples.push_back(f(t));
			t += dt;
		}
		return samples;
	}

	
};


class FourierScene : public Scene {
public:
	FourierScene() :Scene("Fourier Transformation", 1280, 900) {
		bounds = geom::bvol::AABB{ vec3(0), {width(), height(), 0} };
		_requireMouse = false;
		maxSamples = 4000;
		currSample = 0;
		freq = currentFreq = 0.f;
	}

	void init() override {
		setBackGroundColor(BLACK);
		xAxis = new Axis(*this, 1000, 4, 0, 4, true, vec2(0), true);
		yAxis = new Axis(*this, 150, 2, 1, 1, false, vec2(0), true);

		float sampleRate = 1000;
		float numSamples = sampleRate * xAxis->_numTerms;
		float steps = (xAxis->_size)/ numSamples;

		sinWave = sampler.sample1D([&](float t) {
			return g(t);
		}, sampleRate, numSamples);

		Mesh m;
		for (int i = 0; i < numSamples; i++) {
			float y = yAxis->_size * sinWave[i];
			m.positions.push_back(vec3(steps * i, y, 0));
			m.colors.push_back(GREEN);
		}
		m.primitiveType = GL_LINE_STRIP;
		points = new ProvidedMesh(m);

		graph = new Graph({ points }, xAxis, yAxis);
		graph->position = translate(mat4(1), { 50, height() - 50 - graph->height, 0 });

		buildFrequencyGraph();
		buildCycles();
		font = Font::Arial(20, 0, WHITE);
		glEnable(GL_MULTISAMPLE);
		glLineWidth(10.f);
	}

	void buildCycles() {
		Mesh m;
		
		const float TWO_PI = glm::two_pi<float>();

		vector<vec2> samples = sampler.sample2D([&](float t) {
			return vec2{100 * std::cos(TWO_PI * t), 100 * std::sin(TWO_PI * t) };

		}, 100.0f, 100.0f);

		for (auto& sample : samples) {
			m.positions.push_back(vec3(sample, 0));
			m.colors.push_back(WHITE);
		}
		m.primitiveType = GL_POINTS;
		auto circle = new ProvidedMesh(m);



		m.clear();

		cycleFunc = sampler.sample2D([&](float t) {
			return 150.0f * g(t) * e2PIi(freq, t);
		}, 1000, 4000);

		for (auto& sample : cycleFunc) {
			m.positions.push_back(vec3(sample, 0));
			m.colors.push_back(YELLOW);
		}

		m.primitiveType = GL_LINE_STRIP;
		cyclePoints = new ProvidedMesh(m);
		
		cycles = new Graph({ circle, cyclePoints }, new Axis(*this, 400, 4, 0, 1, false, { -2, 0 }), new Axis(*this, 400, 4, 1, 1, false, { -2, 0 }), true);
		
	}

	float nPoints;

	void buildFrequencyGraph() {
		vec4 color = iColor(112, 191, 178);
		//Axis(Scene& scene, int size, int numTerms, int dir = 0, int spacing = 1, bool padLast = false, vec2 tail = vec2(0), bool withHead = false, float lineWidth = 10.0f, vec4 color = WHITE)
		auto xAxis = new Axis(*this, 600, 5, 0, 1, false, vec2(0), true, 10, color);
		auto yAxis = new Axis(*this, 450, 4, 1, 1, false, { -2, 0 }, true, 10, color);
		vector<vec2> samples;
		Mesh m;
		nPoints = xAxis->_numTerms;
		float nSamples = 1000;
		float fStep = nPoints / nSamples;
		float step = 600 / nPoints;

		sampleFreqSpace(nPoints, nSamples, freq);

		color = iColor(222, 122, 101);

		//for (int i = 0; i < sampleRate; i++) {
		//	float f = i * fStep;
		//	if (f > freq) {
		//		m.positions.push_back(vec3(0));
		//		m.colors.push_back(color);
		//		continue;
		//	};
		//	float sum = 0;
		//	
		//	float n = 0.0f;
		//	for (float t = 0; t < nPoints; t += 0.1, n += 1) {
		//		sum += (g(t) * e2PIi(float(f), t)).x;
		//	}
		//	float cm = sum / n;
		//	m.positions.push_back({ f * step, 450 * cm, 0 });
		//	m.colors.push_back(color);
		//}
		for (int i = 0; i < nSamples; i++) {
			float f = i * fStep;
			m.positions.push_back({ f * step, 450 * freqSpaceSamples[i], 0.f });
			m.colors.push_back(color);
		}
		m.primitiveType = GL_LINE_STRIP;
		freqPoints = new ProvidedMesh(m);


		AABB aabb{ vec3(-325, -250, 0), vec3(325, 250, 0)};
		frequency = new Graph({ freqPoints }, xAxis, yAxis, false, true);
	}

	void sampleFreqSpace(int nPoints, float sampleRate, float freq) {
		freqSpaceSamples.clear();
		float fStep = nPoints / sampleRate;
		for (int i = 0; i < sampleRate; i++) {
			float f = i * fStep;
			if (f > freq) {
				freqSpaceSamples.push_back(0);
				continue;
			};
			float sum = 0;

			float n = 0.0f;
			for (float t = 0; t < nPoints; t += 0.1, n += 1) {
				sum += (g(t) * e2PIi(float(f), t)).x;
			}
			float cm = sum / n;
			freqSpaceSamples.push_back(cm);
		}
	}

	void display() override {
		shader("flat")([&] {
			cam.model = translate(mat4(1), {50, height() -50 - graph->height, 0 });
			send(cam);
			shade(graph);

			cam.model = translate(mat4(1), { 300, height() - cycles->height - graph->height, 0 });
			send(cam);
			shade(cycles);

			cam.model = translate(mat4(1), { cycles->width + 200, height() - cycles->height - graph->height, 0 });
			send(cam);
			shade(frequency);
		});
		sbr.clear();
		sbr.str("");
		sbr << setprecision(3) << currentFreq << " Cycles/Second";
		font->render(sbr.str(), 50, height() - cycles->height - graph->height);
	}

	void resized() override {
		cam.projection = ortho(bounds.min().x, bounds.max().x, bounds.min().y, bounds.max().y);
	}

	void update(float dt) override {
		
		static float privFreq = freq;
		static float t = 0;
		float steps = 4.0f / maxSamples;
		
		if (privFreq != freq) {
			t = clamp(t + dt, 0.0f, 1.0f);
			float f = mix(privFreq, freq, t);
			currentFreq = f;
			
			cycleFunc = sampler.sample2D([&](float t) {
				return 150.0f * g(t) * e2PIi(f, t);
			}, 1000, 4000);


			cyclePoints->update2<vec3>(0, [&](vec3* v) {
				for (int i = 0; i < cycleFunc.size(); i++) {
					*(v + i) = vec3(cycleFunc[i], 0);
				}
			});

			float nSamples = 1000;
			float fStep = nPoints / nSamples;
			float step = 600 / nPoints;

			sampleFreqSpace(nPoints, nSamples, f);

			freqPoints->update2<vec3>(0, [&](vec3* v) {
				for (int i = 0; i < nSamples; i++) {
					*(v + i) = vec3({ i * fStep * step, 450 * freqSpaceSamples[i], 0.f });
				}
			});

			if (t >= 1.0f) {
				t = 0;
				privFreq = freq;
			}
		}
		
	}

	void processInput(const Key& key) override {
		if (key.value() == 'w' && key.released()) {
			freq += 0.5;
			if (freq > 5.0f) freq = 5.0f;

			//cyclePoints->update2<vec3>(0, [&](vec3* v) {

			//	for (int i = 0; i < maxSamples; i++) {
			//		*(v + i) = vec3(0);
			//	}
			//});
			//currSample = 0;
			//t = 0;
		}
	}

	float box(float x) {
		return abs(x) < 0.5 ? 1 : 0;
	}

	float delta(float x) {
		return x != 0 ? 0 : 1;
	}

	float g(float t) {
		float _3revs = 3 * glm::two_pi<float>();
		float _2revs = 2 * glm::two_pi<float>();
		float _4revs = 4 * TWO_PI;
		float _5revs = 5 * TWO_PI;
	//	return 0.3 * (cos(_3revs * t) + cos(_2revs * t) + 1.8);
	//	return 0.5 * (cos(_5revs * t) + 1);
	//	return cos(TWO_PI * 3 * t) * exp(-PI * t * t);
	//	return (sin(TWO_PI * 2 * t) + sin(TWO_PI * 3 * t)) * exp(-3.0f * t);
	//	return  sin(_4revs * t) * exp(-0.5 * t);
	//	return cos(6.f * TWO_PI * t) * std::exp(-PI * t * t);
	//	return cos(0.5 * TWO_PI * t) * std::exp(-t);
	//	return exp(-pow((t-2), 2))/sqrt(TWO_PI) * sin(100 * TWO_PI * t) + 0.4;

	//	return 0.5 * cos(TWO_PI * dist(engine) * t) + 0.5;
	//	return box(t);
		return ceil(cos(_5revs * t));
	}

	vec2 e2PIi(float f, float t) {
		float theta = -TWO_PI * f * t;
		return {cos(theta), sin(theta)};
	}

private:
	geom::bvol::AABB bounds;
	Axis* xAxis;
	Axis* yAxis;
	ProvidedMesh* points;
	ProvidedMesh* cyclePoints;
	ProvidedMesh* freqPoints;
	Graph* graph;
	Graph* cycles;
	Graph* frequency;
	Sampler sampler;
	Border* border;
	int maxSamples;
	int currSample;
	float currentFreq;
	vector<float> sinWave;
	vector<float> freqSpaceSamples;
	vector<vec2> cycleFunc;
	float t = 0;
	float freq;
	Font* font;
};