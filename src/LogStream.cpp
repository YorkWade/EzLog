#include "LogStream.h"
#include<stdio.h>//snprintf
#include <assert.h>
#include <iostream>
const char digits[] = "9876543210123456789";
const char* zero = digits + 9;
//BOOST_STATIC_ASSERT(sizeof(digits) == 20);

const char digitsHex[] = "0123456789ABCDEF";
//BOOST_STATIC_ASSERT(sizeof digitsHex == 17);

// Efficient Integer to String Conversions, by Matthew Wilson.
template<typename T>
size_t convert(char buf[], T value)
{
	T i = value;
	char* p = buf;

	do
	{
		int lsd = static_cast<int>(i % 10);
		i /= 10;
		*p++ = zero[lsd];
	} while (i != 0);

	if (value < 0)
		{
		*p++ = '-';
		}
	*p = '\0';
	std::reverse(buf, p);

	return p - buf;
}

size_t convertHex(char buf[], uintptr_t value)
{
	uintptr_t i = value;
	char* p = buf;

	do
	{
		int lsd = static_cast<int>(i % 16);
		i /= 16;
		*p++ = digitsHex[lsd];
	} while (i != 0);

	*p = '\0';
	std::reverse(buf, p);

	return p - buf;
}


void LogStream::staticCheck()
{
	/*BOOST_STATIC_ASSERT(kMaxNumericSize - 10 > std::numeric_limits<double>::digits10);
	BOOST_STATIC_ASSERT(kMaxNumericSize - 10 > std::numeric_limits<long double>::digits10);
	BOOST_STATIC_ASSERT(kMaxNumericSize - 10 > std::numeric_limits<long>::digits10);
	BOOST_STATIC_ASSERT(kMaxNumericSize - 10 > std::numeric_limits<long long>::digits10);*/
}

template<typename T>
void LogStream::formatInteger(T v)
{
	if (buffer_.avail() >= kMaxNumericSize)
	{
		size_t len = convert(buffer_.current(), v);
		buffer_.add(len);
	}
}

LogStream& LogStream::operator<<(short v)
{
	*this << static_cast<int>(v);
	return *this;
}

LogStream& LogStream::operator<<(unsigned short v)
{
	*this << static_cast<unsigned int>(v);
	return *this;
}

LogStream& LogStream::operator<<(int v)
{
	formatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(unsigned int v)
{
	formatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(long v)
{
	formatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(unsigned long v)
{
	formatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(long long v)
{
	formatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(unsigned long long v)
{
	formatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(const void* p)
{
	uintptr_t v = reinterpret_cast<uintptr_t>(p);
	if (buffer_.avail() >= kMaxNumericSize)
	{
		char* buf = buffer_.current();
		buf[0] = '0';
		buf[1] = 'x';
		size_t len = convertHex(buf+2, v);
		buffer_.add(len+2);
	}
	return *this;
}

// FIXME: replace this with Grisu3 by Florian Loitsch.
LogStream& LogStream::operator<<(double v)
{
	if (buffer_.avail() >= kMaxNumericSize)
	{
		int len = _snprintf/*snprintf*/(buffer_.current(), kMaxNumericSize, "%.12g", v);
		buffer_.add(len);
	}
	return *this;
}




template<typename T>
Fmt::Fmt(const char* fmt, T val)
{
	/*BOOST_STATIC_ASSERT(boost::is_arithmetic<T>::value == true);*/

	length_ = _snprintf(buf_, sizeof buf_, fmt, val);
	assert(static_cast<size_t>(length_) < sizeof buf_);
}

// Explicit instantiations

template Fmt::Fmt(const char* fmt, char);

template Fmt::Fmt(const char* fmt, short);
template Fmt::Fmt(const char* fmt, unsigned short);
template Fmt::Fmt(const char* fmt, int);
template Fmt::Fmt(const char* fmt, unsigned int);
template Fmt::Fmt(const char* fmt, long);
template Fmt::Fmt(const char* fmt, unsigned long);
template Fmt::Fmt(const char* fmt, long long);
template Fmt::Fmt(const char* fmt, unsigned long long);

template Fmt::Fmt(const char* fmt, float);
template Fmt::Fmt(const char* fmt, double);

Hex::Hex(const char* begin,uint16_t len)
{
	for (uint16_t i = 0;i<len;i++)
	{
		_snprintf(buf_, sizeof buf_, "%02x",begin[i]);
		hex_str_+=buf_;
		hex_str_+=" ";
	}
}
template<typename T>
Hex::Hex( T val)
{
	_snprintf(buf_, sizeof buf_, "0x%x", val);
	hex_str_ = buf_;
}

template Hex::Hex(char);

template Hex::Hex(short);
template Hex::Hex(unsigned short);
template Hex::Hex(int);
template Hex::Hex(unsigned int);
template Hex::Hex(long);
template Hex::Hex(unsigned long);
template Hex::Hex(long long);
template Hex::Hex(unsigned long long);

template Hex::Hex(float);
template Hex::Hex(double);



