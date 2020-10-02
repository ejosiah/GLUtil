#pragma once

#include <memory>
#include <vector>
#include "../gl/Drawable.h"
#include "../gl/SceneObject.h"
#include "../gl/Scene.h"
#include "../gl/Shader.h"
#include "../gl/Timer.h"
#include <glm/glm.hpp>

namespace ncl {
	namespace animation {

		struct Frame {
			int id;
			std::unique_ptr<gl::Drawable> content;
			long int duration;
		};

		class Animation : public gl::SceneObject {
		public:
			Animation(gl::Scene& scene, bool loop = false)
				:gl::SceneObject(&scene)
				, _loop(loop)
			{}

			void update(float dt) override {
				auto timer = Timer::get();
				long int size = _frames.size();

				if (timer.millisSinceStart() > _duration) {
					_currentFrame = &_frames.at(_index);
					_duration += timer.millisSinceStart() + _currentFrame->duration;

					if (_loop) {
						_index %= size;
					}
					_index = _index < size ? (_index + 1) : size - 1;
				}
			}

			void draw(gl::Shader& shader) override {
				_currentFrame->content->draw(shader);
			}

			bool stopped() const {
				return !_loop && _index == _frames.size() - 1;
			}

		private:
			bool _loop;
			std::vector<Frame> _frames;
			int _index;
			Frame* _currentFrame;
			long int _duration;
		};
	}
}