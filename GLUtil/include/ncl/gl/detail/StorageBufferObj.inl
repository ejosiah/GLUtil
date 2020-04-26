#pragma once

namespace ncl {
	namespace gl {
		template<typename T>
		StorageBufferObj<T>::StorageBufferObj(T t, GLuint id)
			:_obj{ t }
			, _size{ ObjectReflect<T>::sizeOfObj(t) }
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
		void StorageBufferObj<T>::sendToGPU() {
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, _buf);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, _size, ObjectReflect<T>::objPtr(_obj));
		}

		template<typename U>
		void transfer(StorageBufferObj<U>& source, StorageBufferObj<U>& dest) {
			dest._obj = source._obj;
			dest._size = source._size;
			dest._buf = source._buf;
			dest._idx = source._idx;

			source._idx = 0;
			source._buf = 0;
		}
	}
}