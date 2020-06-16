#pragma once

namespace ncl {
	namespace gl {
		template<typename T>
		StorageBufferObj<T>::StorageBufferObj(T t, GLuint id)
			: StorageBufferObj<T>{ std::vector<T>{t} } {
		}

		template<typename T>
		StorageBufferObj<T>::StorageBufferObj(std::vector<T> v, GLuint id)
			:_obj{ v }
			, _size{ sizeOf(v) }
			, _idx{ id }{
			glGenBuffers(1, &_buf);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, _buf);
			glBufferData(GL_SHADER_STORAGE_BUFFER, _size, NULL, GL_DYNAMIC_DRAW);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, _idx, _buf);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		}

		template<typename T>
		StorageBufferObj<T>::StorageBufferObj(int count, GLuint id)
			: _size{ sizeOf(count) }
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
		T* StorageBufferObj<T>::get() {
			return &_obj[0];
		}

		template<typename T>
		T StorageBufferObj<T>::getFromGPU() {
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, _buf);
			T data = *((T*)glMapNamedBuffer(_buf, GL_READ_ONLY));
			glUnmapNamedBuffer(_buf);
			return data;
		}

		template<typename T>
		T StorageBufferObj<T>::getFromGPU(unsigned index) {
			auto offset = sizeOf(index);
			assert(offset < _size);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, _buf);
			T data = *((T*)glMapNamedBuffer(_buf, GL_READ_ONLY) + index);
			glUnmapNamedBuffer(_buf);
			return data;
		}

		template<typename T>
		void StorageBufferObj<T>::sendToGPU(bool update) {
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, _buf);
			if (update && !_obj.empty()) {
				glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, _size, &_obj[0]);
			}
		}

		template<typename T>
		void StorageBufferObj<T>::read(std::function<void(T*)> use) {
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, _buf);
			T* data = (T*)glMapNamedBuffer(_buf, GL_READ_ONLY);
			use(data);
			glUnmapNamedBuffer(_buf);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		}

		template<typename U>
		void transfer(StorageBufferObj<U>& source, StorageBufferObj<U>& dest) {
			dest._obj = std::move(source._obj);
			dest._size = source._size;
			dest._buf = source._buf;
			dest._idx = source._idx;

			source._idx = 0;
			source._buf = 0;
		}
	}
}