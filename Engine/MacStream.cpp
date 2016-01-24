#include "Common.h"
#include "MacStream.h"

NAMESPACE_PROCESSING

std::vector<byte> MacStream::ComputeMac(CEX::IO::IByteStream* InStream)
{
	if (InStream->Length() - InStream->Position() < 1)
		throw CryptoProcessingException("MacStream:ComputeHash", "The Input stream is too short!");

	_inStream = InStream;
	long dataLen = _inStream->Length() - _inStream->Position();
	CalculateInterval(dataLen);
	_macEngine->Reset();

	return Compute(dataLen);
}

std::vector<byte> MacStream::ComputeMac(const std::vector<byte> &Input, unsigned int InOffset, unsigned int Length)
{
	if (Length - InOffset < 1 || Length - InOffset > Input.size())
		throw CryptoProcessingException("MacStream:ComputeHash", "The Input stream is too short!");

	long dataLen = Length - InOffset;
	CalculateInterval(dataLen);
	_macEngine->Reset();

	return Compute(Input, InOffset, Length);
}

/*** Protected Methods ***/

void MacStream::CalculateInterval(unsigned int Length)
{
	unsigned int interval = Length / 100;

	if (interval < _blockSize)
		_progressInterval = _blockSize;
	else
		_progressInterval = interval - (interval % _blockSize);

	if (_progressInterval == 0)
		_progressInterval = _blockSize;
}

void MacStream::CalculateProgress(unsigned int Length, bool Completed)
{
	if (Completed || Length % _progressInterval == 0)
	{
		double progress = 100.0 * ((double)_progressInterval / Length);
		ProgressPercent((int)progress);
	}
}

std::vector<byte> MacStream::Compute(unsigned int Length)
{
	unsigned int bytesTotal = 0;
	unsigned int bytesRead = 0;
	std::vector<byte> buffer(_blockSize);
	unsigned int maxBlocks = Length / _blockSize;

	for (unsigned int i = 0; i < maxBlocks; i++)
	{
		bytesRead = _inStream->Read(buffer, 0, _blockSize);
		_macEngine->BlockUpdate(buffer, 0, bytesRead);
		bytesTotal += bytesRead;
		CalculateProgress(bytesTotal);
	}

	// last block
	if (bytesTotal < Length)
	{
		buffer.resize(Length - bytesTotal);
		bytesRead = _inStream->Read(buffer, 0, buffer.size());
		_macEngine->BlockUpdate(buffer, 0, buffer.size());
		bytesTotal += buffer.size();
	}

	// get the hash
	std::vector<byte> chkSum(_macEngine->MacSize());
	_macEngine->DoFinal(chkSum, 0);
	CalculateProgress(bytesTotal);

	return chkSum;
}

std::vector<byte> MacStream::Compute(const std::vector<byte> &Input, unsigned int InOffset, unsigned int Length)
{
	const unsigned int alnBlocks = (Length / _blockSize) * _blockSize;
	unsigned int bytesTotal = 0;

	while (bytesTotal != alnBlocks)
	{
		_macEngine->BlockUpdate(Input, InOffset, _blockSize);
		InOffset += _blockSize;
		bytesTotal += _blockSize;
		CalculateProgress(bytesTotal);
	}

	// last block
	if (bytesTotal != Length)
	{
		unsigned int diff = Length - bytesTotal;
		_macEngine->BlockUpdate(Input, InOffset, diff);
		bytesTotal += diff;
	}

	// get the hash
	std::vector<byte> chkSum(_macEngine->MacSize());
	_macEngine->DoFinal(chkSum, 0);
	CalculateProgress(bytesTotal);

	return chkSum;
}

void MacStream::Destroy()
{
	_blockSize = 0;
	_destroyEngine = false;
	_progressInterval = 0;

	if (_destroyEngine)
		delete _macEngine;

	_isDestroyed = true;
}

NAMESPACE_PROCESSINGEND
