#pragma once

#include <gl.hpp>


enum : uint32_t {
	SizeBits = 2,
	SizeShift = 0,
	SizeMask = ~((~0U >> SizeBits) << SizeBits) << SizeShift,
	Size1 = 0 << SizeShift,
	Size2 = 1 << SizeShift,
	Size3 = 2 << SizeShift,
	Size4 = 3 << SizeShift,
#define AttribSize( A ) (((A & SizeMask) >> SizeShift) + 1)

	TypeBits = 8,
	TypeShift = SizeShift + SizeBits,
	TypeMask = ~((~0U >> TypeBits) << TypeBits) << TypeShift,
#define AttribType( A ) (((A & TypeMask) >> TypeShift) | 0x1400)
	TypeFloat = (GL_FLOAT & 0xff) << TypeShift,
	TypeUnsignedByte = (GL_UNSIGNED_BYTE & 0xff) << TypeShift,

	BytesBits = 4,
	BytesShift = TypeShift + TypeBits,
	BytesMask = ~((~0U >> BytesBits) << BytesBits) << BytesShift,
#define AttribBytes( A ) (((A & BytesMask) >> BytesShift) + 1)

	Bytes1 = 0 << BytesShift,
	Bytes2 = 1 << BytesShift,
	Bytes3 = 2 << BytesShift,
	Bytes4 = 3 << BytesShift,
	Bytes8 = 7 << BytesShift,
	Bytes12 = 11 << BytesShift,
	Bytes16 = 15 << BytesShift,

#define AttribNormalized( A ) ((AttribType( A ) == GL_FLOAT) ? GL_FALSE : GL_TRUE)
//----------------------
	Attrib1f = Size1 | TypeFloat | Bytes4,
	Attrib2f = Size2 | TypeFloat | Bytes8,
	Attrib3f = Size3 | TypeFloat | Bytes12,
	Attrib4f = Size4 | TypeFloat | Bytes16,

	Attrib1ub = Size1 | TypeUnsignedByte | Bytes1,
	Attrib2ub = Size2 | TypeUnsignedByte | Bytes2,
	Attrib3ub = Size3 | TypeUnsignedByte | Bytes3,
	Attrib4ub = Size4 | TypeUnsignedByte | Bytes4,
};
static_assert(AttribBytes(Attrib3f) == 12,"Attrib3f has proper bytes field");
static_assert(AttribSize(Attrib2f) == 2,"Attrib2f has proper size field");

static_assert(AttribNormalized(Attrib4ub) == GL_TRUE, "Byte is normalized");
static_assert(AttribNormalized(Attrib1f) == GL_FALSE, "Float is not normalized");

class GLAttribStoreBase {
public:
	GLAttribStoreBase() : buffer(0), count(0) {
	}
	~GLAttribStoreBase() {
		clear();
	}
	template< class X >
	void internal_set(const X *_data, uint32_t _count) {
		count = _count;
		if (count == 0) return;

		if (buffer == 0) {
			glGenBuffers(1, &buffer);
		}
		GLint old_binding = 0;
		glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &old_binding);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(X) * _count, _data, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, old_binding);
	}
	void clear() {
		if (buffer) {
			glDeleteBuffers(1, &buffer);
			buffer = 0;
			count = 0;
		}
	}
public:
	GLuint buffer;
	GLsizei count;
};


template< int A1 >
class GLAttribStore1 : public GLAttribStoreBase {
public:
	template< class X >
	void set(const X *_data, uint32_t _count) {
		static_assert(sizeof(X) == AttribBytes(A1), "Class is sum of attribs");
		internal_set(_data, _count);
	}
};

template< int A1, int A2 >
class GLAttribStore2 : public GLAttribStoreBase {
public:
	template< class X >
	void set(const X *_data, uint32_t _count) {
		static_assert(sizeof(X) == AttribBytes(A1) + AttribBytes(A2), "Class is sum of attribs");
		internal_set(_data, _count);
	}
};

template< int A1, int A2, int A3 >
class GLAttribStore3 : public GLAttribStoreBase {
public:
	template< class X >
	void set(const X *_data, uint32_t _count) {
		static_assert(sizeof(X) == AttribBytes(A1) + AttribBytes(A2) + AttribBytes(A3), "Class is sum of attribs");
		internal_set(_data, _count);
	}
};



class BindAttribStore1 {
public:
	GLuint t1;
	GLint old_binding;
	template< int A1 >
	BindAttribStore1(GLuint _t1, GLAttribStore1< A1 > *store) : t1(_t1) {
		assert(store);
		glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &old_binding);
		glBindBuffer(GL_ARRAY_BUFFER, store->buffer);
		glEnableVertexAttribArray(t1);
		glVertexAttribPointer(t1, AttribSize(A1), AttribType(A1), AttribNormalized(A1), AttribBytes(A1), reinterpret_cast< const GLbyte * >(NULL) + 0);
	}
	~BindAttribStore1() {
		glDisableVertexAttribArray(t1);
		glBindBuffer(GL_ARRAY_BUFFER, old_binding);
	}
};


class BindAttribStore2 {
public:
	GLuint t1, t2;
	GLint old_binding;
	template< int A1, int A2 >
	BindAttribStore2(GLuint _t1, GLuint _t2, GLAttribStore2< A1, A2 > *store) : t1(_t1), t2(_t2) {
		assert(store);
		glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &old_binding);
		glBindBuffer(GL_ARRAY_BUFFER, store->buffer);
		glVertexAttribPointer(t1, AttribSize(A1), AttribType(A1), AttribNormalized(A1), AttribBytes(A1) + AttribBytes(A2), reinterpret_cast< const GLbyte * >(NULL) + 0);
		glVertexAttribPointer(t2, AttribSize(A2), AttribType(A2), AttribNormalized(A2), AttribBytes(A1) + AttribBytes(A2), reinterpret_cast< const GLbyte * >(NULL) + AttribBytes(A1));
		glEnableVertexAttribArray(t1);
		glEnableVertexAttribArray(t2);
	}
	~BindAttribStore2() {
		glDisableVertexAttribArray(t1);
		glDisableVertexAttribArray(t2);
		glBindBuffer(GL_ARRAY_BUFFER, old_binding);
	}
};

class BindAttribStore3 {
public:
	GLuint t1, t2, t3;
	GLint old_binding;
	template< int A1, int A2, int A3 >
	BindAttribStore3(GLuint _t1, GLuint _t2, GLuint _t3, GLAttribStore3< A1, A2, A3 > *store) : t1(_t1), t2(_t2), t3(_t3) {
		assert(store);
		glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &old_binding);
		glBindBuffer(GL_ARRAY_BUFFER, store->buffer);
		glVertexAttribPointer(t1, AttribSize(A1), AttribType(A1), AttribNormalized(A1), AttribBytes(A1) + AttribBytes(A2) + AttribBytes(A3), reinterpret_cast< const GLbyte * >(NULL) + 0);
		glVertexAttribPointer(t2, AttribSize(A2), AttribType(A2), AttribNormalized(A2), AttribBytes(A1) + AttribBytes(A2) + AttribBytes(A3), reinterpret_cast< const GLbyte * >(NULL) + AttribBytes(A1));
		glVertexAttribPointer(t3, AttribSize(A3), AttribType(A3), AttribNormalized(A3), AttribBytes(A1) + AttribBytes(A2) + AttribBytes(A3), reinterpret_cast< const GLbyte * >(NULL) + AttribBytes(A1) + AttribBytes(A2));
		glEnableVertexAttribArray(t1);
		glEnableVertexAttribArray(t2);
		glEnableVertexAttribArray(t3);
	}
	~BindAttribStore3() {
		glDisableVertexAttribArray(t1);
		glDisableVertexAttribArray(t2);
		glDisableVertexAttribArray(t3);
		glBindBuffer(GL_ARRAY_BUFFER, old_binding);
	}
};

//---------------------------------
// convenience classes for attribs:

template< typename A1, typename A2 >
class Attrib2 {
public:
	Attrib2(A1 const &_a1, A2 const &_a2) : a1(_a1), a2(_a2) {
		static_assert(sizeof(Attrib2< A1, A2 >) == sizeof(A1) + sizeof(A2), "Attrib2 is packed");
	}
	A1 a1;
	A2 a2;
};

template< typename A1, typename A2, typename A3 >
class Attrib3 {
public:
	Attrib3(A1 const &_a1, A2 const &_a2, A3 const &_a3) : a1(_a1), a2(_a2), a3(_a3) {
		static_assert(sizeof(Attrib3< A1, A2, A3 >) == sizeof(A1) + sizeof(A2) + sizeof(A3), "Attrib3 is packed");
	}
	A1 a1;
	A2 a2;
	A3 a3;
};
