#pragma once

#include <utility>
#include <iterator>
#include <gl/gl_core_4_5.h>

namespace ncl {
	namespace gl {

		using VaoId = unsigned int;
		using BufferId = unsigned int;
		using AttributeId = unsigned int;

		template<bool Const, typename T>
		class buffer_iterator {
			friend class buffer_iterator<!Const, T>;
			using node_pointer = std::conditional_t<Const, const T*, T*>;


		public:
			explicit buffer_iterator(T* ptr): _ptr{ptr}{}
			using difference_type = ptrdiff_t;
			using value_type = T;
			using pointer = std::conditional_t<Const, const T*, T>;
			using reference = std::conditional_t<Const, const T&, T&>;
			using iterator_category = std::random_access_iterator_tag;

			reference operator*() const { 
				return *_ptr; 
			}

			auto& operator++() {
				_ptr++;
				return *this;
			}

			auto operator++(int) {
				auto result = *this;
				++*this;
				return result;
			}

			auto& operator--() {
				_ptr--;
				return *this;
			}

			auto operator--(int) {
				auto result = *this;
				--*this;
				return result;
			}

			template<bool R, typename U>
			auto& operator+=(const buffer_iterator<R, U>& rhs) {
				_ptr += rhs._ptr;
				return *this;
			}

			template<bool R, typename U>
			auto& operator-=(const buffer_iterator<R, U>& rhs) {
				_ptr -= rhs._ptr;
				return *this;
			}

			template<bool R, typename U>
			bool operator==(const buffer_iterator<R, U>& rhs) const {
				return _ptr == rhs._ptr;
			}


			template<bool R, typename U>
			bool operator!=(const buffer_iterator<R, U>& rhs) const {
				return _ptr != rhs._ptr;
			}

			template<bool R, typename U>
			friend inline difference_type operator-(const buffer_iterator<R, U>& lhs, const buffer_iterator<R, U>& rhs) {
				return  lhs._ptr - rhs._ptr;
			}

			template<bool R, typename U>
			friend inline difference_type operator+(const buffer_iterator<R, U>& lhs, const buffer_iterator<R, U>& rhs) {
				return lhs._ptr + rhs._ptr;
			}


		private:
			node_pointer _ptr;
		};

		template<typename T>
		class GlBuffer {
		public:
			GlBuffer(VaoId vaoId, BufferId bufferId) :
				_vaoId{ vaoId }, _bufferId{ bufferId }{
				glBindVertexArray(vaoId);
				_ptr = (T*)glMapNamedBuffer(_bufferId, GL_READ_ONLY);
				glBindBuffer(GL_ARRAY_BUFFER, bufferId);	// TODO make generic
				glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &_size);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				_size /= sizeof(T);
				_begin = _ptr;
				_end = _begin + _size;
			}

			GlBuffer(const GlBuffer& source) = delete;

			GlBuffer(GlBuffer&& source) noexcept {
				if (this == &source) return;
				this->_ptr = source._ptr;
				this->_begin = source._begin;
				this->_end = source._end;
				this->_vaoId = source._vaoId;
				this->_bufferId = source._bufferId;

				source._ptr = nullptr;
				source._begin = source._end;
				source._vaoId = 0;
				source._bufferId = 0;
			}

			~GlBuffer() {
				glUnmapNamedBuffer(_bufferId);
				glBindVertexArray(0);
			}

			using const_iterator = buffer_iterator<true, T>;
			using iterator = buffer_iterator<false, T>;

			GlBuffer& operator=(const GlBuffer& source) = delete;

			iterator begin() {
				return iterator(_begin);
			}

			iterator end() {
				return iterator(_end);
			}

			const_iterator const_begin() {
				return const_iterator(_begin);
			}

			const_iterator const_end() {
				return const_iterator(_end);
			}

			int size() const {
				return _size;
			}

		private:
			T* _ptr;
			T* _begin;
			T* _end;
			int _size;
			VaoId _vaoId;
			BufferId _bufferId;
		};

	}
}