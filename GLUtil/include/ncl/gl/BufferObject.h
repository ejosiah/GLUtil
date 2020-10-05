#pragma once

#include <gl/gl_core_4_5.h>
#include <functional>
#include "../type_wrapper.h"
#include <array>

namespace ncl {
	namespace gl {
		template<GLenum Target, typename T>
		class BufferObject {
		public:
			BufferObject() = default;

			explicit BufferObject(T, GLuint = 0);

			explicit BufferObject(size_t count, GLuint id = 0);

			explicit BufferObject(std::vector<T>, GLuint = 0);

			explicit BufferObject(Shape&, GLuint = 0);

			BufferObject(BufferObject<Target, T>&&) noexcept;

			BufferObject(const BufferObject<Target, T>&) = delete;

			~BufferObject();

			BufferObject<Target, T>& operator=(const BufferObject<Target, T>&) = delete;

			BufferObject<Target, T>& operator=(BufferObject<Target, T>&&);

			T* get();

			T getFromGPU();

			T getFromGPU(unsigned int index);

			inline GLuint buffer() {
				return _buf;
			}

			inline size_t count() {
				return _size / sizeof(T);
			}

			template<GLenum Target0, typename U>
			friend void send(const BufferObject<Target0, U>&);

			template<GLenum Target0, typename U>
			friend void transfer(BufferObject<Target0, U>&, BufferObject<Target0, U>&);

			void sendToGPU(bool update = true);

			static inline GLsizeiptr sizeOf(std::vector<T> v) {
				return sizeof(T) * v.size();
			}

			static inline GLsizeiptr sizeOf(int count) {
				return sizeof(T) * count;
			}
			
			void read(std::function<void(T*)> use);

		private:
			std::vector<T> _obj;
			GLsizeiptr _size;
			GLuint _buf;
			GLuint _idx;

		}; 

		template<typename T>
		using StorageBufferObj = BufferObject<GLenum(GL_SHADER_STORAGE_BUFFER), T>;
		
		using AtomicCounterBuffer = BufferObject<GLenum(GL_ATOMIC_COUNTER_BUFFER), GLuint>;
		
	}
}
#include "detail/BufferObject_inl.h"