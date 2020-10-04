#pragma once

#include <map>
#include <memory>
#include <vector>
#include <memory>
#include <regex>
#include <functional>
#include <filesystem>
#include <glm/glm.hpp>
#include "../gl/Timer.h"
#include "../gl/Scene.h"
#include "../gl/Shader.h"
#include "../gl/Drawable.h"
#include "../gl/SceneObject.h"
#include "../gl/Model.h"
#include "Image2D.h"

namespace fs = std::filesystem;

namespace ncl {
	namespace animation {

		struct Frame {
			int id = 0;
			gl::Drawable* content = nullptr;
			long int duration = 0;

			~Frame() {
//				delete content;
			}
		};

		class Animation : public gl::SceneObject {
		public:
			Animation(gl::Scene& scene, std::vector <Frame> frames, bool loop = true)
				:gl::SceneObject(&scene)
				, _loop(loop)
				, _frames(std::move(frames))
			{}

			void update(float dt) override {
				auto timer = Timer::get();
				long int size = _frames.size();

				if (timer.millisSinceStart() > _duration) {
					_currentFrame = &_frames.at(_index);
					_duration = timer.millisSinceStart() + _currentFrame->duration;

					_index++;
					if (_index >= size) {
						if (_loop) {
							_duration = 0;
							_index = 0;
						}else{
							_index--;
						}
					}

				}
			}

			void draw(gl::Shader& shader) override {
				_currentFrame->content->draw(shader);
			}

			bool stopped() const {
				return !_loop && _index == _frames.size() - 1;
			}

		private:
			bool _loop = true;
			std::vector<Frame> _frames;
			int _index = 0;
			Frame* _currentFrame = nullptr;
			long int _duration = 0;
		};

		using FrameFactory = std::function<gl::Drawable*(fs::path)>;

		class AnimationBuilder {
		public:
			inline std::unique_ptr<Animation> build(gl::Scene& scene, fs::path path, int fps = 60) {
				auto itr = fs::directory_iterator(path);

				std::vector<fs::path> files;
				for (auto entry : itr) {
					if (entry.is_regular_file()) {
						files.push_back(entry.path());
					}
				}

				std::vector<Frame> frames(files.size());
				const std::regex DIGIT("\(\\d+\)");
				std::smatch match;
				long int duration = (1.0 / fps) * 1000;

				for (auto p : files) {
					auto filename = p.filename().string();
					auto ext = p.filename().extension().string();
					auto i = filename.find(ext);

					auto name = filename.substr(0, i);
					if (std::regex_search(name, match, DIGIT)) {
						auto id = match[1];
						auto index = std::stoi(id);
						frames[index] = {
							index,
							factory(p),
							duration
						};
					}
					
				}

				return std::make_unique<Animation>(scene, frames);
			}
			
			static inline AnimationBuilder ObjAnimationBuilder() {
				return AnimationBuilder([](fs::path path) {
					return new gl::Model(path.string());
				});
			}

			static inline AnimationBuilder Image2DAnimationBuilder() {
				return AnimationBuilder([](fs::path path) {
					return new Image2D{ path.string() };
				});
			}

		private:
			AnimationBuilder(FrameFactory factory)
				:factory{ factory }
			{}


			FrameFactory factory;
		};
	}
}
