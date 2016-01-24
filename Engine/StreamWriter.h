#ifndef _CEXENGINE_STREAMWRITER_H
#define _CEXENGINE_STREAMWRITER_H

#include "Common.h"
#include "MemoryStream.h"

NAMESPACE_IO

/// <summary>
/// Write integer values to a byte array
/// </summary>
class StreamWriter
{
protected:
	unsigned int _streamPosition;
	std::vector<byte> _streamData;

public:

	/// <summary>
	/// Initialize this class with a byte array
	/// </summary>
	///
	/// <param name="Offset">Length of the array</param>
	StreamWriter(unsigned int Length)
		:
		_streamData(Length),
		_streamPosition(0)
	{
	}

	/// <summary>
	/// Finalize objects
	/// </summary>
	~StreamWriter()
	{
		Destroy();
	}

	/// <summary>
	/// Release all resources associated with the object
	/// </summary>
	void Destroy();

	/// <summary>
	/// Returns the entire array of raw bytes from the stream
	/// </summary>
	std::vector<byte> GetBytes();

	/// <summary>
	/// Returns the base MemoryStream object
	/// </summary>
	MemoryStream GetStream();

	/// <summary>
	/// Write an integer array to the base stream
	/// </summary>
	template <class T>
	void Write(std::vector<T> &Data)
	{
		unsigned int sze = sizeof(T) * Data.size();
		if (_streamPosition + sze > _streamData.size())
			_streamData.resize(_streamPosition + sze);

		memcpy(&_streamData[_streamPosition], &Data[0], sze);
		_streamPosition += sze;
	}

	/// <summary>
	/// Write an integer to the base stream
	/// </summary>
	template <class T>
	void Write(T Data)
	{
		unsigned int sze = sizeof(T);
		if (_streamPosition + sze > _streamData.size())
			_streamData.resize(_streamPosition + sze);

		memcpy(&_streamData[_streamPosition], &Data, sze);
		_streamPosition += sze;
	}
};

NAMESPACE_IOEND
#endif
