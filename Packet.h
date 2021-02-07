#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include <arpa/inet.h>
#include <stdexcept>

#include "zlib.h"
#include "BlockBuffer.h"

#ifndef ntohll
#define ntohll(x) ( ( (uint64_t)(ntohl( (uint32_t)((x << 32) >> 32) )) << 32) | ntohl( ((uint32_t)(x >> 32)) ) )                                        
#define htonll(x) ntohll(x)
#endif

namespace kafkaprotocpp {

struct PacketError : public std::runtime_error
{
	PacketError(const std::string & w) :
	std::runtime_error(w)
	{
	}
};

struct PackError : public PacketError
{
	PackError(const std::string & w) :
	PacketError(w)
	{
	}
};

struct UnpackError : public PacketError
{
	UnpackError(const std::string & w) :
	PacketError(w)
	{
	}
};

struct IncompletePacket : public PacketError
{
    IncompletePacket(const std::string& w) : PacketError(w) {}
};

class PackBuffer
{
private:
	typedef BlockBuffer<def_block_alloc_16k, 65536> BB;
	// use big-block. more BIG? MAX 64K*16k = 1G
	BB bb;
public:
	char * data()
	{
		return bb.data();
	}
	size_t size() const
	{
		return bb.size();
	}

	void resize(size_t n)
	{
		if(bb.resize(n))
			return;
		throw PackError("resize buffer overflow");
	}
	void append(const char * data, size_t size)
	{
		if(bb.append(data, size))
			return;
		throw PackError("append buffer overflow");
	}
	void append(const char * data)
	{
		append(data,:: strlen(data));
	}
	void replace(size_t pos, const char * rep, size_t n)
	{
		if(bb.replace(pos, rep, n))	return;
		throw PackError("replace buffer overflow");
	}
	void reserve(size_t n)
	{
		if(bb.reserve(n)) return;
		throw PackError("reserve buffer overflow");
	}
};

struct Pack {
private:
    Pack(const Pack& o);
    Pack& operator = (const Pack& o);

public:
	// IMPORTANT remember the buffer-size before pack. see data(), size()
	// reserve a space to replace packet header after pack parameter
	// sample below: OffPack. see data(), size()
	Pack(PackBuffer & pb, size_t off = 0) : m_buffer(pb)
	{
		m_offset = pb.size() + off;
		m_buffer.resize(m_offset);
	}

	// access this packet.
	char * data()
	{
		return m_buffer.data() + m_offset;
	}
	const char * data()  const
	{
		return m_buffer.data() + m_offset;
	}
	size_t size() const
	{
		return m_buffer.size() - m_offset;
	}

	Pack & push(const void* s, size_t n)
	{
		m_buffer.append((const char*)s, n); 
        return *this;
	}

	Pack & push(const void* s)
	{
		m_buffer.append((const char*)s); 
        return *this;
	}

	Pack & push_int8(int8_t num)
	{
		return push(&num, 1);
	}

	Pack & push_int16(int16_t num)
	{
		num = htons(num); 
        return push(&num, 2);
	}

	Pack & push_int32(int32_t num)
	{
		num = htonl(num); 
        return push(&num, 4);
	}	

	Pack & push_int64(int64_t num)
	{
		num = htonll(num); 
        return push(&num, 8);
	}

    Pack & push_string(const std::string& str)
    {
        if(str.size() > 0xFFFF) throw PackError("push_string: string too long");
        return push_int16(str.size()).push(str.data(), str.size());
    }

    Pack & push_string(const char* data, size_t len)
    {
        if(len > 0xFFFF) throw PackError("push_string: string too long");
        return push_int16(len).push(data, len);
    }

	Pack & push_bytes(const char* data, size_t size)
	{
        if(size > 0xFFFFFFFF) throw PackError("push_bytes: bytes too long");
        return push_int32(size).push(data, size);
	}

    // replace apis
	size_t replace(size_t pos, const void* data, size_t rplen)
	{
		m_buffer.replace(pos, (const char*)data, rplen);
		return pos + rplen;
	}
	size_t replace_int8(size_t pos, int8_t u8)
	{
		return replace(pos, &u8, 1);
	}
	size_t replace_int16(size_t pos, int16_t u16)
	{
		u16 = htons(u16);
		return replace(pos, &u16, 2);
	}
	size_t replace_int32(size_t pos, int32_t u32)
	{
		u32 = htonl(u32);
		return replace(pos, &u32, 4);
	}

    // The CRC32 functions make the following assumptions:
    // 1 - head is pointing to an int32 crc field when beginCRC32() is called
    // 2 - head is incremented past the crc field, and the next N bytes (to be CRC32'd) are written
    // 3 - when endCRC32() is called, N bytes (this->head - this->crcHead) are CRC32'd, and the result written to the intial crc field (this->crcHead - sizeof(crc))
    // 4 - the functions can't be used concurrently
    void beginCRC32()
    {
        push_int32(0); // will be updated @ endCRC32()
        m_crcHead = m_buffer.size();
    }

    int endCRC32()
    {
        int crcLength = m_buffer.size() - m_crcHead;
        uint32_t initCrc = crc32(0L, Z_NULL, 0);
        uint32_t crc = crc32(initCrc, (unsigned char*)(m_buffer.data() + m_crcHead), crcLength);
        if (crc == initCrc)
        {
            return -1;
        }
        int signedCrc = (int)crc;
        replace_int32(m_crcHead - sizeof(int32_t), signedCrc);
        return signedCrc;
    }

protected:
    PackBuffer & m_buffer;
    size_t m_offset;
    size_t m_crcHead;
};

struct Unpack
{
public:
	Unpack(const char* data, size_t size)
	{
		reset(data, size);
	}
	void reset(const char* data, size_t size) const
	{
		m_data = data;
		m_size = size;
	}

	virtual ~Unpack()
	{
		m_data = NULL;
	}

	operator const char* () const
	{
		return m_data;
	}
	bool operator!() const
	{
		return(NULL == m_data);
	}

	void finish() const
	{
		if(!empty()) throw UnpackError("finish: too much data");
	}

	int8_t pop_int8() const
	{
		if(m_size < 1u)
			throw UnpackError("pop_int8: not enough data");

		int8_t i8 = *((int8_t*)m_data);
		m_data += 1u; m_size -= 1u;
		return i8;
	}

	int16_t pop_int16() const
	{
		if(m_size < 2u)
			throw UnpackError("pop_int16: not enough data");

		int16_t i16 = *((int16_t*)m_data);
		i16 = ntohs(i16);

		m_data += 2u; m_size -= 2u;
		return i16;
	}

	int32_t pop_int32() const
	{
		if(m_size < 4u)
			throw UnpackError("pop_int32: not enough data");
		int32_t i32 = *((int32_t*)m_data);
		i32 = ntohl(i32);
		m_data += 4u; m_size -= 4u;
		return i32;
	}

	int32_t peek_int32() const {
		if (m_size < 4u)
			throw UnpackError("peek_int32: not enough data");
		int32_t i32 = *((int32_t*)m_data);
		i32 = ntohl(i32);
		return i32;
	}
	int64_t pop_int64() const
	{
		if(m_size < 8u)
			throw UnpackError("pop_int64: not enough data");
		int64_t i64 = *((int64_t*)m_data);
		i64 = ntohll(i64);
		m_data += 8u; m_size -= 8u;
		return i64;
	}

    std::string pop_bytes() const
	{
		int size = pop_int32();
        if(size <=0) 
            return "";
		const char* data = pop_fetch_ptr(size);
		return std::string(data, size);
	}

    std::string pop_string() const
    {
        int16_t len = pop_int16();
        if(len <= 0)
            return "";

        const char* data = pop_fetch_ptr(len);
        return std::string(data, len);
    }

	const char * pop_fetch_ptr(size_t k) const
	{
		if(m_size < k)
		{
			//abort();
			throw UnpackError("pop_fetch_ptr: not enough data");
		}

		const char * p = m_data;
		m_data += k; m_size -= k;
		return p;
	}

	bool empty() const
	{
		return m_size == 0;
	}
	const char * data() const
	{
		return m_data;
	}
	size_t size() const
	{
		return m_size;
	}

private:
	mutable const char * m_data;
	mutable size_t m_size;
};

struct Marshallable {
    virtual void marshal(Pack &) const {}
    virtual void unmarshal(const Unpack &) {}
    virtual ~Marshallable()
    {
    }
};

// helper functions

inline Pack & operator << (Pack & p, const Marshallable & m)
{
	m.marshal(p);
	return p;
}

inline const Unpack & operator >> (const Unpack & p, const Marshallable & m)
{
	const_cast<Marshallable &>(m).unmarshal(p);
	return p;
}

inline Pack & operator << (Pack & p, int8_t  i8)
{
	p.push_int8(i8);
	return p;
}

inline Pack & operator << (Pack & p, int16_t  i16)
{
	p.push_int16(i16);
	return p;
}

inline Pack & operator << (Pack & p, int32_t i32)
{
	p.push_int32(i32);
	return p;
}

inline Pack & operator << (Pack & p, int64_t  i64)
{
	p.push_int64(i64);
	return p;
}

inline Pack & operator << (Pack & p, const std::string & str)
{
	p.push_string(str);
	return p;
}

inline const Unpack& operator>>(const Unpack& p, int &i32)
{
    i32 = p.pop_int32();
	return p;
}

inline const Unpack & operator >> (const Unpack & p, int64_t & i64)
{
	i64 =  p.pop_int64();
	return p;
}

inline const Unpack & operator >> (const Unpack & p, std::string & str)
{
	str = p.pop_string();
	return p;
}

inline const Unpack & operator >> (const Unpack & p, int16_t & i16)
{
	i16 =  p.pop_int16();
	return p;
}


inline const Unpack & operator >> (const Unpack & p, int8_t & i8)
{
	i8 =  p.pop_int8();
	return p;
}

// container marshal helper
template < typename ContainerClass >
inline void marshal_container(Pack & p, const ContainerClass & c)
{
	p.push_int32(c.size());
	for(typename ContainerClass::const_iterator i = c.begin(); i != c.end(); ++i)
		p << *i;
}

template <class T>
inline Pack & operator << (Pack& p, const std::vector<T>& vec)
{
    marshal_container(p, vec);
    return p;
}


template < typename OutputIterator >
inline void unmarshal_container(const Unpack & p, OutputIterator i)
{
	for(int32_t count = p.pop_int32(); count > 0; --count)
	{
		typename OutputIterator::container_type::value_type tmp;
		p >> tmp;
		*i = tmp;
		++i;
	}
}

template <class T>
inline const Unpack & operator >> (const Unpack & p, std::vector<T>& vec)
{
    unmarshal_container(p, back_inserter(vec));
    return p;
}

}
