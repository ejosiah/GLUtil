#pragma once

#include <gl/gl_core_4_5.h>
#include <functional>
#include <array>
#include "../type_wrapper.h"
#include "CopyBuffer.h"
#include "buffer_iterator.h"
#include <iterator>

namespace ncl {
	namespace gl {
		template<GLenum Target, typename T>
		class BufferObject : public CopyBuffer {
		public:
			BufferObject();

			explicit BufferObject(T, GLuint = 0);

			[[deprecated]]
			explicit BufferObject(size_t, GLuint = 0);

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

			inline GLuint buffer() const {
				return _buf;
			}

			// TODO use iterator
			inline void fill(T val) {
				auto size = count();
				update([size, val](auto ptr) {
					for (int i = 0; i < size; i++) {
						*(ptr + i) = val;
					}
				});
			}

			inline size_t count() const {
				return _size / sizeof(T);
			}

			inline size_t size() const {
				return _size;
			}

			template<GLenum Target0, typename U>
			friend void send(const BufferObject<Target0, U>&);

			template<GLenum Target0, typename U>
			friend void transfer(BufferObject<Target0, U>&, BufferObject<Target0, U>&);

			void sendToGPU(bool update = true);

			void bind(GLuint index = _idx);

			void bind(GLuint index, GLsizeiptr offset, GLsizeiptr size);

			static inline GLsizeiptr sizeOf(std::vector<T> v) {
				return sizeof(T) * v.size();
			}

			static inline GLsizeiptr sizeOf(int count) {
				return sizeof(T) * count;
			}
			
			void read(std::function<void(T*)> use) const;


			void update(std::function<void(T*)> use);

			void update(T* data);

			template<typename InputIterator>
			inline void from(InputIterator first, InputIterator last, GLuint index = 0) {
				allocate(std::distance(first, last), index);
				update([&first, &last](auto ptr) {
					auto itr = first;
					while(itr != last){
						*ptr = *itr;
						++itr;
						++ptr;
					}
				});
			}

			void update(GLintptr offset, GLsizeiptr size, T* data);

			void allocate(size_t size = 1, GLuint index = 0);

			void reference(GLuint buffer);

			void empty();

			void copy(GLuint buffer, GLuint index = 0);

			using const_iterator = buffer_iterator<true, T>;
			using iterator = buffer_iterator<false, T>;
		private:
			std::vector<T> _obj;
			GLsizeiptr _size;
			GLuint _buf;
			GLuint _idx;
			bool _owner = true;

		}; 

		template<typename T>
		using StorageBuffer = BufferObject<GL_SHADER_STORAGE_BUFFER, T>;

		template<typename T>
		using UniformBuffer = BufferObject<GL_UNIFORM_BUFFER, T>;
		
		using AtomicCounterBuffer = BufferObject<GL_ATOMIC_COUNTER_BUFFER, GLuint>;
		
	}
}
#include "detail/BufferObject_inl.h"