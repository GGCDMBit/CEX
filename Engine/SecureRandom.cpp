#include "SecureRandom.h"
#include "CSPPrng.h"
#include "BitConverter.h"
#include "IntUtils.h"

NAMESPACE_PRNG

using CEX::Utility::IntUtils;
using CEX::IO::BitConverter;

// *** Public Methods *** //

/// <summary>
/// Release all resources associated with the object
/// </summary>
void SecureRandom::Destroy()
{
	if (!_isDestroyed)
	{
		_bufferIndex = 0;
		_bufferSize = 0;

		IntUtils::ClearVector(_byteBuffer);

		if (_rngGenerator != 0)
		{
			_rngGenerator->Destroy();
			delete _rngGenerator;
		}
		_isDestroyed = true;
	}
}

// *** Byte *** //

/// <summary>
/// Return an array filled with pseudo random bytes
/// </summary>
/// 
/// <param name="Size">Size of requested byte array</param>
/// 
/// <returns>Random byte array</returns>
std::vector<byte> SecureRandom::GetBytes(unsigned int Size)
{
	std::vector<byte> data(Size);
	GetBytes(data);
	return data;
}

/// <summary>
/// Fill an array with pseudo random bytes
/// </summary>
///
/// <param name="Output">Output array</param>
void SecureRandom::GetBytes(std::vector<byte> &Output)
{
	if (Output.size() == 0)
		throw CryptoRandomException("CTRPrng:GetBytes", "Buffer size must be at least 1 byte!");

	if (_byteBuffer.size() - _bufferIndex < Output.size())
	{
		unsigned int bufSize = _byteBuffer.size() - _bufferIndex;
		// copy remaining bytes
		if (bufSize != 0)
			memcpy(&Output[0], &_byteBuffer[_bufferIndex], bufSize);

		unsigned int rem = Output.size() - bufSize;

		while (rem > 0)
		{
			// fill buffer
			_rngGenerator->GetBytes(_byteBuffer);

			if (rem > _byteBuffer.size())
			{
				memcpy(&Output[bufSize], &_byteBuffer[0], _byteBuffer.size());
				bufSize += _byteBuffer.size();
				rem -= _byteBuffer.size();
			}
			else
			{
				memcpy(&Output[bufSize], &_byteBuffer[0], rem);
				_bufferIndex = rem;
				rem = 0;
			}
		}
	}
	else
	{
		memcpy(&Output[0], &_byteBuffer[_bufferIndex], Output.size());
		_bufferIndex += Output.size();
	}
}

// *** Char *** //

/// <summary>
/// Get a random char
/// </summary>
/// 
/// <returns>Random char</returns>
char SecureRandom::NextChar()
{
	int sze = sizeof(char);
	return BitConverter::ToChar(GetBytes(sze), 0);
}

/// <summary>
/// Get a random unsigned char
/// </summary>
/// 
/// <returns>Random unsigned char</returns>
unsigned char SecureRandom::NextUChar()
{
	int sze = sizeof(unsigned char);
	return BitConverter::ToUChar(GetBytes(sze), 0);
}

// *** Double *** //

/// <summary>
/// Get a random double
/// </summary>
/// 
/// <returns>Random double</returns>
double SecureRandom::NextDouble()
{
	int sze = sizeof(double);
	return BitConverter::ToDouble(GetBytes(sze), 0);
}

// *** Int16 *** //

/// <summary>
/// Get a random non-negative short integer
/// </summary>
/// 
/// <returns>Random Int16</returns>
short SecureRandom::NextInt16()
{
	return BitConverter::ToInt16(GetBytes(2), 0);
}

/// <summary>
/// Get a random non-negative short integer
/// </summary>
/// 
/// <param name="Maximum">Maximum value</param>
/// <returns>Random Int16</returns>
short SecureRandom::NextInt16(short Maximum)
{
	std::vector<byte> rand;
	short num(0);

	do
	{
		rand = GetByteRange(Maximum);
		memcpy(&num, &rand[0], rand.size());
	} 
	while (num > Maximum);

	return num;
}

/// <summary>
/// Get a random non-negative short integer
/// </summary>
/// 
/// <param name="Minimum">Minimum value</param>
/// <param name="Maximum">Maximum value</param>
/// 
/// <returns>Random Int16</returns>
short SecureRandom::NextInt16(short Minimum, short Maximum)
{
	short num = 0;
	while ((num = NextInt16(Maximum)) < Minimum) {}
	return num;
}


// *** UInt16 *** //

/// <summary>
/// Get a random unsigned short integer
/// </summary>
/// 
/// <returns>Random UInt16</returns>
unsigned short SecureRandom::NextUInt16()
{
	return BitConverter::ToUInt16(GetBytes(2), 0);
}

/// <summary>
/// Get a random unsigned short integer
/// </summary>
/// 
/// <param name="Maximum">Maximum value</param>
/// 
/// <returns>Random UInt16</returns>
unsigned short SecureRandom::NextUInt16(unsigned short Maximum)
{
	std::vector<byte> rand;
	unsigned short num(0);

	do
	{
		rand = GetByteRange(Maximum);
		memcpy(&num, &rand[0], rand.size());
	} 
	while (num > Maximum);

	return num;
}

/// <summary>
/// Get a random unsigned short integer
/// </summary>
/// 
/// <param name="Minimum">Minimum value</param>
/// <param name="Maximum">Maximum value</param>
/// 
/// <returns>Random UInt16</returns>
unsigned short SecureRandom::NextUInt16(unsigned short Minimum, unsigned short Maximum)
{
	unsigned short num = 0;
	while ((num = NextUInt16(Maximum)) < Minimum) {}
	return num;
}

// *** Int32 *** //

/// <summary>
/// Get a random non-negative 32bit integer
/// </summary>
/// 
/// <returns>Random Int32</returns>
int SecureRandom::Next()
{
	return BitConverter::ToInt32(GetBytes(4), 0);
}

/// <summary>
/// Get a random non-negative 32bit integer
/// </summary>
/// 
/// <returns>Random Int32</returns>
int SecureRandom::NextInt32()
{
	return BitConverter::ToInt32(GetBytes(4), 0);
}

/// <summary>
/// Get a random non-negative 32bit integer
/// </summary>
/// 
/// <param name="Maximum">Maximum value</param>
/// 
/// <returns>Random Int32</returns>
int SecureRandom::NextInt32(int Maximum)
{
	std::vector<byte> rand;
	int num(0);

	do
	{
		rand = GetByteRange(Maximum);
		memcpy(&num, &rand[0], rand.size());
	} 
	while (num > Maximum);

	return num;
}

/// <summary>
/// Get a random non-negative 32bit integer
/// </summary>
/// 
/// <param name="Minimum">Minimum value</param>
/// <param name="Maximum">Maximum value</param>
/// 
/// <returns>Random Int32</returns>
int SecureRandom::NextInt32(int Minimum, int Maximum)
{
	int num = 0;
	while ((num = NextInt32(Maximum)) < Minimum) {}
	return num;
}

// *** UInt32 *** //

/// <summary>
/// Get a random unsigned 32bit integer
/// </summary>
/// 
/// <returns>Random UInt32</returns>
unsigned int SecureRandom::NextUInt32()
{
	return BitConverter::ToUInt32(GetBytes(4), 0);
}

/// <summary>
/// Get a random unsigned integer
/// </summary>
/// 
/// <param name="Maximum">Maximum value</param>
/// 
/// <returns>Random UInt32</returns>
unsigned int SecureRandom::NextUInt32(unsigned int Maximum)
{
	std::vector<byte> rand;
	unsigned int num(0);

	do
	{
		rand = GetByteRange(Maximum);
		memcpy(&num, &rand[0], rand.size());
	} 
	while (num > Maximum);

	return num;
}

/// <summary>
/// Get a random unsigned integer
/// </summary>
/// 
/// <param name="Minimum">Minimum value</param>
/// <param name="Maximum">Maximum value</param>
/// 
/// <returns>Random UInt32</returns>
unsigned int SecureRandom::NextUInt32(unsigned int Minimum, unsigned int Maximum)
{
	unsigned int num = 0;
	while ((num = NextUInt32(Maximum)) < Minimum) {}
	return num;
}

// *** Int64 *** //

/// <summary>
/// Get a random long integer
/// </summary>
/// 
/// <returns>Random Int64</returns>
long SecureRandom::NextLong()
{
	return BitConverter::ToInt64(GetBytes(8), 0);
}

/// <summary>
/// Get a random long integer
/// </summary>
/// 
/// <returns>Random Int64</returns>
long SecureRandom::NextInt64()
{
	return BitConverter::ToInt64(GetBytes(8), 0);
}

/// <summary>
/// Get a random long integer
/// </summary>
/// 
/// <param name="Maximum">Maximum value</param>
/// 
/// <returns>Random Int64</returns>
long SecureRandom::NextInt64(long Maximum)
{
	std::vector<byte> rand;
	long num(0);

	do
	{
		rand = GetByteRange(Maximum);
		memcpy(&num, &rand[0], rand.size());
	} 
	while (num > Maximum);

	return num;
}

/// <summary>
/// Get a random long integer
/// </summary>
/// 
/// <param name="Minimum">Minimum value</param>
/// <param name="Maximum">Maximum value</param>
/// 
/// <returns>Random Int64</returns>
long SecureRandom::NextInt64(long Minimum, long Maximum)
{
	long num = 0;
	while ((num = NextInt64(Maximum)) < Minimum) {}
	return num;
}

// *** UInt64 *** //

/// <summary>
/// Get a random ulong integer
/// </summary>
/// 
/// <returns>Random UInt64</returns>
ulong SecureRandom::NextUInt64()
{
	return BitConverter::ToUInt64(GetBytes(8), 0);
}

/// <summary>
/// Get a random ulong integer
/// </summary>
/// 
/// <param name="Maximum">Maximum value</param>
/// 
/// <returns>Random UInt64</returns>
ulong SecureRandom::NextUInt64(ulong Maximum)
{
	std::vector<byte> rand;
	ulong num(0);

	do
	{
		rand = GetByteRange(Maximum);
		memcpy(&num, &rand[0], rand.size());
	} 
	while (num > Maximum);

	return num;
}

/// <summary>
/// Get a random ulong integer
/// </summary>
/// 
/// <param name="Minimum">Minimum value</param>
/// <param name="Maximum">Maximum value</param>
/// 
/// <returns>Random UInt64</returns>
ulong SecureRandom::NextUInt64(ulong Minimum, ulong Maximum)
{
	ulong num = 0;
	while ((num = NextUInt64(Maximum)) < Minimum) {}
	return num;
}

/// <summary>
/// Reset the generator instance
/// </summary>
void SecureRandom::Reset()
{
	if (_rngGenerator != 0)
	{
		_rngGenerator->Destroy();
		delete _rngGenerator;
	}
	_rngGenerator = new CEX::Seed::CSPRsg;
	_rngGenerator->GetBytes(_byteBuffer);
	_bufferIndex = 0;
}

// *** Protected Methods *** //

std::vector<byte> SecureRandom::GetByteRange(ulong Maximum)
{
	std::vector<byte> data;

	if (Maximum < 256)
		data = GetBytes(1);
	else if (Maximum < 65536)
		data = GetBytes(2);
	else if (Maximum < 16777216)
		data = GetBytes(3);
	else if (Maximum < 4294967296)
		data = GetBytes(4);
	else if (Maximum < 1099511627776)
		data = GetBytes(5);
	else if (Maximum < 281474976710656)
		data = GetBytes(6);
	else if (Maximum < 72057594037927936)
		data = GetBytes(7);
	else
		data = GetBytes(8);

	return GetBits(data, Maximum);
}

std::vector<byte> SecureRandom::GetBits(std::vector<byte> Data, ulong Maximum)
{
	ulong val = 0;
	memcpy(&val, &Data[0], Data.size());
	int bits = Data.size() * 8;

	while (val > Maximum && bits != 0)
	{
		val >>= 1;
		bits--;
	}

	std::vector<byte> ret(Data.size());
	memcpy(&ret[0], &val, Data.size());

	return ret;
}

NAMESPACE_PRNGEND