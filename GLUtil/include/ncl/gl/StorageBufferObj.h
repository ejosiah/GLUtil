#pragma once

#include <gl/gl_core_4_5.h>

namespace ncl {
	namespace gl {
		
		template<typename T>
		class StorageBufferObj {
		public:
			StorageBufferObj() = default;

			explicit StorageBufferObj(T, GLuint = 0);

			StorageBufferObj(StorageBufferObj<T>&&) noexcept;

			StorageBufferObj(const StorageBufferObj<T>&) = delete;

			~StorageBufferObj();

			StorageBufferObj<T>& operator=(const StorageBufferObj<T>&) = delete;

			StorageBufferObj<T>& operator=(StorageBufferObj<T>&&);

			T& get();

			template<typename U>
			friend void send(const StorageBufferObj<U>&);

			template<typename U>
			friend void transfer(StorageBufferObj<U>&, StorageBufferObj<U>&);

			template<typename U>
			friend GLuint sizeOfObj(U& obj);

			template<typename U>
			friend void* objPtr(U& obj);

		protected:
			void sendToGPU() const;

		private:
			T _obj;
			GLsizeiptr _size;
			GLuint _buf;
			GLuint _idx;

		}; 

		template<typename T>
		StorageBufferObj<T>::StorageBufferObj(T t, GLuint id)
			:_obj{ t }
			, _size{ sizeOfObj(t) }
			, _idx{ id }{
			glGenBuffers(1, &_buf);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, _buf);
			glBufferData(GL_SHADER_STORAGE_BUFFER, _size, NULL, GL_DYNAMIC_DRAW);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, _idx, _buf);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		}

		template<typename T>
		StorageBufferObj<T>::StorageBufferObj(StorageBufferObj<T>&& source) noexcept {
			transfer(source, *this);
		}

		template<typename T>
		StorageBufferObj<T>::~StorageBufferObj() {
			if (glIsBuffer(_buf) == GL_TRUE) {
				glDeleteBuffers(1, &_buf);
			}
		}

		template<typename T>
		StorageBufferObj<T>& StorageBufferObj<T>::operator=(StorageBufferObj<T>&& source) {
			transfer(source, *this);
			return *this;
		}

		template<typename T>
		T& StorageBufferObj<T>::get() {
			return _obj;
		}

		template<typename T>
		void StorageBufferObj<T>::sendToGPU() const{
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, _buf);
			glBufferData(GL_SHADER_STORAGE_BUFFER, _size, objPtr(_obj), GL_DYNAMIC_DRAW);
		}

		template<typename U>
		void transfer(StorageBufferObj<U>& source, StorageBufferObj<U>& dest) {
			dest._obj = source._obj;
			dest._size = source._size;
			dest._buf = source._buf;
			dest._idx = source._idx;

			source._idx = 0;
		}
	}

}