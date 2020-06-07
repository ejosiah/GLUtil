#pragma once

namespace ncl {
	namespace gl {
		template<typename T>
		StorageBufferObj<T>::StorageBufferObj(T* t, int count, bool map, GLuint id)
			:_obj{ t }
			, _size{ sizeOf<T>(count) }
			, _idx{ id }
			, _mapped{ map }{
			glGenBuffers(1, &_buf);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, _buf);

			if (_mapped) {
				glBufferStorage(GL_SHADER_STORAGE_BUFFER, _size, nullptr, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT);
			}
			else {
				glBufferData(GL_SHADER_STORAGE_BUFFER, _size, nullptr, GL_DYNAMIC_DRAW);
			}

			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, _idx, _buf);

			if (_mapped) {
				_obj = (T*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, _size,
					GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT);
			}

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		}

		template<typename T>
		StorageBufferObj<T>::StorageBufferObj(int count, bool map, GLuint id)
			: _obj{ nullptr }
			, _size{ sizeOf<T>(count) }
			, _idx{ id }
			, _mapped{ map }{
			glGenBuffers(1, &_buf);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, _buf);
			if (_mapped) {
				glBufferStorage(GL_SHADER_STORAGE_BUFFER, _size, nullptr, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT);
			}
			else {
				glBufferData(GL_SHADER_STORAGE_BUFFER, _size, nullptr, GL_DYNAMIC_DRAW);
			}
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, _idx, _buf);
			

			if (_mapped) {
				_obj = (T*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, _size,
					GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT);
			}
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
			return *_obj;
		}

		template<typename T>
		T StorageBufferObj<T>::getFromGPU() {
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, _buf);
			T* data = (T*)glMapNamedBuffer(_buf, GL_READ_ONLY);
			glUnmapNamedBuffer(_buf);
			return *data;
		}

		template<typename T>
		void StorageBufferObj<T>::sendToGPU(bool update) {
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, _buf);
			if (update) {
				glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, _size, _obj);
			}
		}

		template<typename U>
		void transfer(StorageBufferObj<U>& source, StorageBufferObj<U>& dest) {
			dest._obj = source._obj;
			dest._size = source._size;
			dest._buf = source._buf;
			dest._idx = source._idx;
			dest._mapped = source._mapped;

			source._idx = 0;
			source._buf = 0;
		}
	}
}