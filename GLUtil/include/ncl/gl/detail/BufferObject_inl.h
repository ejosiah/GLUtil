#pragma once

namespace ncl {
	namespace gl {
		template<GLenum Target, typename T>
		BufferObject<Target, T>::BufferObject()
			:_size{ 0 }
			,_idx{ 0 }
			, _buf{ 0 }{
		}

		template<GLenum Target, typename T>
		BufferObject<Target, T>::BufferObject(T t, GLuint id)
			: BufferObject<Target, T>{ std::vector<T>{t} } {
		}

		template<GLenum Target, typename T>
		BufferObject<Target, T>::BufferObject(std::vector<T> v, GLuint id)
			:_obj{ v }
			, _size{ sizeOf(v) }
			, _idx{ id }
		{
			glGenBuffers(1, &_buf);
			glBindBuffer(Target, _buf);
			glBufferData(Target, _size, nullptr, GL_DYNAMIC_DRAW);

			if(!v.empty()){
				glBufferSubData(Target, 0, _size, &_obj[0]);
			}

			glBindBufferBase(Target, _idx, _buf);
			glBindBuffer(Target, 0);
		}

		template<GLenum Target, typename T>
		void BufferObject<Target, T>::allocate(size_t size, GLuint id)
		{
			_size = sizeOf(size);
			_idx = id;
			glGenBuffers(1, &_buf);
			glBindBuffer(Target, _buf);
			glBufferData(Target, _size, NULL, GL_DYNAMIC_DRAW);
			glBindBufferBase(Target, _idx, _buf);
			glBindBuffer(Target, 0);
		}

		template<GLenum Target, typename T>
		void BufferObject<Target, T>::reference(GLuint buffer)
		{
			if (!glIsBuffer(buffer)) throw std::runtime_error{ "Not a buffer object" };
			_owner = false;
			_buf = buffer;
		}

		template<GLenum Target, typename T>
		void BufferObject<Target, T>::copy(GLuint buffer, GLuint index)
		{
			if (!glIsBuffer(buffer)) throw std::runtime_error{ "Not a buffer object" };
			_owner = true;
			auto [size, buf] = CopyBuffer::copy(buffer);
			_size = size;
			_buf = buf;
		}

		template<GLenum Target, typename T>
		BufferObject<Target, T>::BufferObject(Shape& shape, GLuint id) 
			:_size{ shape.size() }
			, _idx{ id }
			, _buf{ shape.bufferFor(0, VAOObject::Position) }
		{
			glBindBufferBase(Target, _idx, _buf);
		}
		
		template<GLenum Target, typename T>
		BufferObject<Target, T>::BufferObject(BufferObject<Target, T>&& source) noexcept 
		{
			transfer(source, *this);
		}

		template<GLenum Target, typename T>
		BufferObject<Target, T>::~BufferObject() 
		{
			if (glIsBuffer(_buf) == GL_TRUE && _owner) {
				glDeleteBuffers(1, &_buf);
			}
		}

		template<GLenum Target, typename T>
		BufferObject<Target, T>& BufferObject<Target, T>::operator=(BufferObject<Target, T>&& source) 
		{
			transfer(source, *this);
			return *this;
		}

		template<GLenum Target, typename T>
		T* BufferObject<Target, T>::get() {
			return &_obj[0];
		}

		template<GLenum Target, typename T>
		T BufferObject<Target, T>::getFromGPU() {
			glBindBuffer(Target, _buf);
			T data = *((T*)glMapNamedBuffer(_buf, GL_READ_ONLY));
			glUnmapNamedBuffer(_buf);
			return data;
		}

		template<GLenum Target, typename T>
		T BufferObject<Target, T>::getFromGPU(unsigned index) {
			auto offset = sizeOf(index);
			assert(offset < _size);
			glBindBuffer(Target, _buf);
			T data = *((T*)glMapNamedBuffer(_buf, GL_READ_ONLY) + index);
			glUnmapNamedBuffer(_buf);
			return data;
		}

		template<GLenum Target, typename T>
		void BufferObject<Target, T>::sendToGPU(bool update) {
			glBindBuffer(Target, _buf);
			if (update && !_obj.empty()) {
				glBufferSubData(Target, 0, _size, &_obj[0]);
			}
		}

		template<GLenum Target, typename T>
		void BufferObject<Target, T>::bind(GLuint index) {
			glBindBufferBase(Target, index, _buf);
		}

		template<GLenum Target, typename T>
		void BufferObject<Target, T>::bind(GLuint index, GLsizeiptr offset, GLsizeiptr size) {
			glBindBufferRange(Target, index, _buf, offset, size);
		}

		// TODO make read only
		template<GLenum Target, typename T>
		void BufferObject<Target, T>::read(std::function<void(T*)> use) const {
			glBindBuffer(Target, _buf);
			T* data = (T*)glMapNamedBuffer(_buf, GL_READ_WRITE);
			use(data);
			glUnmapNamedBuffer(_buf);
			glBindBuffer(Target, 0);
		}

		template<GLenum Target, typename T>
		void BufferObject<Target, T>::update(std::function<void(T*)> use) {
			glBindBuffer(Target, _buf);
			T* data = (T*)glMapNamedBuffer(_buf, GL_READ_WRITE);
			use(data);
			glUnmapNamedBuffer(_buf);
			glBindBuffer(Target, 0);
		}

		template<GLenum Target, typename T>
		void BufferObject<Target, T>::update(T* data) {
			update(0, _size, data);
		}

		template<GLenum Target, typename T>
		void BufferObject<Target, T>::update(GLintptr offset, GLsizeiptr size, T* data) {
			if ((offset + size < 0) || (offset + size > _size)) {
				throw std::out_of_range{ "data is out of range" };
			}
			glBindBuffer(Target, _buf);
			glBufferSubData(Target, offset, size, data);
		}

		template<GLenum Target0, typename U>
		void transfer(BufferObject<Target0, U>& source, BufferObject<Target0, U>& dest) {
			dest._obj = std::move(source._obj);
			dest._size = source._size;
			dest._buf = source._buf;
			dest._idx = source._idx;
			dest._owner = source._owner;

			source._idx = 0;
			source._buf = 0;
			source._owner = false;
		}
	}
}