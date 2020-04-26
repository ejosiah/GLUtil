#pragma once

LightingM::LightingM(Scene* scene, string name) :SceneObject{ (scene) }, _name{ name }{

}

void LightingM::init()  {
	model = new Model("C:\\Users\\Josiah\\OneDrive\\media\\models\\lte-orb\\lte-orb.obj", true, 1);
	font = _scene().font;
}


PbrScene& LightingM::_scene() {
	return dynamic_cast<PbrScene&>(SceneObject::scene());
}




BSDF::BSDF(Scene* scene) :LightingM(scene, "BSDF") {
	scene->addShader("spec_brdf", GL_FRAGMENT_SHADER, identity_frag_shader);
	bitfield = NORMAL_DISTRIBUTION_FUNC;
}

void BSDF::init() {
	LightingM::init();
	material = pbr::ScalarMaterial{ vec3{0, 0, 1}, 0.5f, 0.5f };
}

void BSDF::render() {
	_scene().shader("spec_brdf")([&]() {
		send("bitfield", bitfield);
		send("alpha", material.roughness);
		send("metalness", material.metalness);
		send("lightPos", _scene().light[0].position);
		send("viewPos", vec3(_scene().sCoord));
		send(_scene().cam);
		shade(model);
		});
	displayText();
}

void BSDF::displayText() {
	ss.str("");
	ss.clear();

	ss << "Normal distribution function: 0\t";
	ss << "Geomertry function: 1\t";
	ss << "Frenel: 2\n";

	if ((bitfield & NORMAL_DISTRIBUTION_FUNC) == NORMAL_DISTRIBUTION_FUNC) {
		ss << "Selection: Normal distribution function\n";
	}
	if ((bitfield & GEOMETRY_FUNC) == GEOMETRY_FUNC) {
		ss << "Selection: Geometry function\n";
	}
	if ((bitfield & FRENEL) == FRENEL) {
		ss << "Selection: Frenel\n";
	}
	if ((bitfield & ALL) == ALL) {
		ss << "Selection: Normal distribution function + Geometry function + Frenel\n";
	}

	ss << "Albedo: " << material.albedo;
	ss << "\nRoughness: " << setw(2) << material.roughness;
	ss << "\nMetalness: " << material.metalness;
	font->render(ss.str(), 20, 20);

	ss.str("");
	ss.clear();

	ss << "ViewPos: " << vec3(_scene().sCoord) << "\n";
	ss << "RightPos: " << _scene().light[0].position.xyz;
	font->render(ss.str(), _scene()._width - 300, 20);
}

void BSDF::update(float t) {

}

void BSDF::processInput(const Key& key) {
	float prevStep = step;
	if (key.pressed()) {
		switch (key.value()) {
		case '0':
			bitfield = NORMAL_DISTRIBUTION_FUNC;
			break;
		case '1':
			bitfield = GEOMETRY_FUNC;
			break;
		case '2':
			bitfield = FRENEL;
			break;
		case '3':
			bitfield = ALL;
			break;
		case 'r':
			material.roughness += 0.05;
			break;
		case 'R':
			material.roughness -= 0.05;
			break;
		case 'm':
			material.metalness += 0.05;
			break;
		case 'M':
			material.metalness -= 0.05;
			break;
		case 'a':
			step += 0.01;
			break;
		case 'A':
			step -= 0.01;
			break;
		}
		material.roughness = glm::clamp(material.roughness, 0.1f, 0.9f);
		material.metalness = glm::clamp(material.metalness, 0.0f, 1.0f);
		if (step != prevStep) {
			step = glm::clamp(step, 0.0f, 1.0f);
			material.albedo = color();
		}
	}
}