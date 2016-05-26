// The MIT License (MIT)
// 
// Copyright (c) 2016 vtdev.com
// This file is part of the CEX Cryptographic library.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// 
// Principal Algorithms:
// Portions of this cipher based on the ChaCha stream cipher designed by Daniel J. Bernstein:
// ChaCha20 <a href="http://cr.yp.to/chacha/chacha-20080128.pdf">Specification</a>.
// 
// Implementation Details:
// ChaCha20+
// An implementation based on the ChaCha stream cipher,
// using an extended key size, and higher variable rounds assignment.
// Valid Key sizes are 128 and 256 (16 and 32 bytes).
// Valid rounds are 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28 and 30.
// Written by John Underhill, October 21, 2014
// contact: develop@vtdev.com

#ifndef _CEXENGINE_CHACHA_H
#define _CEXENGINE_CHACHA_H

#include "IStreamCipher.h"

NAMESPACE_STREAM

/// <summary>
/// ChaCha+: A parallelized ChaCha stream cipher implementation
/// </summary>
/// 
/// <example>
/// <description>Encrypt an array with ChaCha:</description>
/// <code>
/// KeyParams kp(Key, Iv);
/// ChaCha cipher(20);
/// // linear encrypt
/// cipher.Initialize(kp);
/// cipher.IsParallel() = false;
/// cipher.Transform(Input, Output);
/// </code>
/// </example>
/// 
/// <remarks>
/// <description>Implementation Notes:</description>
/// <list type="bullet">
/// <item><description>Valid Key sizes are 128 and 256 (16 and 32 bytes).</description></item>
/// <item><description>Block size is 64 bytes wide.</description></item>
/// <item><description>Valid rounds are 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28 and 30.</description></item>
/// </list>
/// 
/// <description>Guiding Publications:</description>
/// <list type="number">
/// <item><description>ChaCha20 <a href="http://cr.yp.to/chacha/chacha-20080128.pdf">Specification</a>.</description></item>
/// <item><description>Salsa20 <a href="http://cr.yp.to/snuffle/design.pdf">Design</a>.</description></item>
/// <item><description>Salsa20 <a href="http://cr.yp.to/snuffle/security.pdf">Security</a>.</description></item>
/// </list>
/// </remarks>
class ChaCha : public IStreamCipher
{
private:
	static constexpr size_t ROUNDS20 = 20;
	static constexpr size_t MAX_ROUNDS = 30;
	static constexpr size_t MIN_ROUNDS = 8;
	static constexpr size_t STATE_SIZE = 16;
	static constexpr size_t VECTOR_SIZE = 8;
	static constexpr size_t BLOCK_SIZE = 64;
	static constexpr size_t PARALLEL_CHUNK = 1024;
	static constexpr size_t MAXALLOC_MB100 = 100000000;
	static constexpr size_t PARALLEL_DEFBLOCK = 64000;
	static constexpr const char *SIGMA = "expand 32-byte k";
	static constexpr const char *TAU = "expand 16-byte k";

	std::vector<uint> _ctrVector;
	bool _isDestroyed;
	std::vector<byte> _dstCode;
	bool _isInitialized;
	bool _isParallel;
	std::vector<size_t> _legalKeySizes;
	std::vector<size_t> _legalRounds;
	size_t _parallelBlockSize;
	size_t _processorCount;
	size_t _rndCount;
	std::vector<std::vector<uint>> _threadVectors;
	std::vector<uint> _wrkState;

public:

	// *** Properties *** //

	/// <summary>
	/// Get: Unit block size of internal cipher in bytes.
	/// <para>Block size is 64 bytes wide.</para>
	/// </summary>
	virtual const size_t BlockSize() { return BLOCK_SIZE; }

	/// <summary>
	/// Get the current counter value
	/// </summary>
	virtual ulong Counter() { return ((ulong)_ctrVector[1] << 32) | (_ctrVector[0] & 0xffffffffL); }

	/// <summary>
	/// Get/Set: Sets the Nonce value in the initialization parameters (Tau-Sigma).
	/// <para>Must be set before <see cref="Initialize(KeyParams)"/> is called.
	/// Changing this code will create a unique distribution of the cipher.
	/// Code must be 16 bytes in length and sufficiently asymmetric (no more than 2 repeating characters, at a distance of 2 intervals).</para>
	/// </summary>
	virtual std::vector<byte> &DistributionCode() { return _dstCode; }

	/// <summary>
	/// Get: The stream ciphers type name
	/// </summary>
	virtual const CEX::Enumeration::StreamCiphers Enumeral() { return CEX::Enumeration::StreamCiphers::ChaCha; }

	/// <summary>
	/// Get: Cipher is ready to transform data
	/// </summary>
	virtual const bool IsInitialized() { return _isInitialized; }

	/// <summary>
	/// Get: Available Encryption Key Sizes in bytes
	/// </summary>
	virtual const std::vector<size_t>&LegalKeySizes() { return _legalKeySizes; }

	/// <summary>
	/// Get: Available diffusion round assignments
	/// </summary>
	virtual const std::vector<size_t> &LegalRounds() { return _legalRounds; }

	/// <summary>
	/// Get/Set: Automatic processor parallelization
	/// </summary>
	virtual bool &IsParallel() { return _isParallel; }

	/// <summary>
	/// Get/Set: Parallel block size. Must be a multiple of <see cref="ParallelMinimumSize"/>.
	/// </summary>
	virtual size_t &ParallelBlockSize() { return _parallelBlockSize; }

	/// <summary>
	/// Get: Maximum input size with parallel processing
	/// </summary>
	virtual const size_t ParallelMaximumSize() { return MAXALLOC_MB100; }

	/// <summary>
	/// Get: The smallest parallel block size. 
	/// <para>Parallel blocks must be a multiple of this size.</para>
	/// </summary>
	virtual const size_t ParallelMinimumSize() { return _processorCount * (STATE_SIZE * 4); }

	/// <remarks>
	/// Get: Processor count
	/// </remarks>
	virtual const size_t ProcessorCount() { return GetProcessorCount(); }

	/// <summary>
	/// Get: Cipher name
	/// </summary>
	virtual const char *Name() { return "ChaCha"; }

	/// <summary>
	/// Get: Number of rounds
	/// </summary>
	virtual const size_t Rounds() { return _rndCount; }

	/// <summary>
	/// Get: Initialization vector size
	/// </summary>
	virtual const size_t VectorSize() { return VECTOR_SIZE; }

	// *** Constructor *** //

	/// <summary>
	/// Initialize the class
	/// </summary>
	///
	/// <param name="Rounds">Number of diffusion rounds. The <see cref="LegalRounds"/> property contains available sizes. Default is 20 rounds.</param>
	///
	/// <exception cref="CEX::Exception::CryptoSymmetricCipherException">Thrown if an invalid rounds count is chosen</exception>
	explicit ChaCha(size_t Rounds = ROUNDS20)
		:
		_ctrVector(2, 0),
		_isDestroyed(false),
		_isInitialized(false),
		_isParallel(false),
		_parallelBlockSize(PARALLEL_DEFBLOCK),
		_rndCount(Rounds),
		_wrkState(14, 0)
	{
		if (Rounds == 0 || (Rounds & 1) != 0)
			throw CryptoSymmetricCipherException("Salsa20:Ctor", "Rounds must be a positive even number!");
		if (Rounds < MIN_ROUNDS || Rounds > MAX_ROUNDS)
			throw CryptoSymmetricCipherException("Salsa20:Ctor", "Rounds must be between 8 and 30!");

		_legalKeySizes = { 16, 32 };
		_legalRounds = { 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30 };

		SetScope();
	}

	/// <summary>
	/// Finalize objects
	/// </summary>
	virtual ~ChaCha()
	{
		Destroy();
	}

	// *** Public Methods *** //

	/// <summary>
	/// Destroy of this class
	/// </summary>
	virtual void Destroy();

	/// <summary>
	/// Initialize the Cipher
	/// </summary>
	/// 
	/// <param name="KeyParam">Cipher key container. 
	/// <para>Uses the Key and IV fields of KeyParam. 
	/// The <see cref="LegalKeySizes"/> property contains valid Key sizes. 
	/// IV must be 8 bytes in size.</para>
	/// </param>
	virtual void Initialize(const CEX::Common::KeyParams &KeyParam);

	/// <summary>
	/// Reset the primary internal counter
	/// </summary>
	virtual void Reset();

	/// <summary>
	/// Encrypt/Decrypt an array of bytes.
	/// <para><see cref="Initialize(KeyParams)"/> must be called before this method can be used.</para>
	/// </summary>
	/// 
	/// <param name="Input">Input bytes, plain text for encryption, cipher text for decryption</param>
	/// <param name="Output">Output bytes, array of at least equal size of input that receives processed bytes</param>
	virtual void Transform(const std::vector<byte> &Input, std::vector<byte> &Output);

	/// <summary>
	/// Encrypt/Decrypt an array of bytes with offset parameters.
	/// <para><see cref="Initialize(KeyParams)"/> must be called before this method can be used.</para>
	/// </summary>
	/// 
	/// <param name="Input">Input bytes to Transform</param>
	/// <param name="InOffset">Offset in the Input array</param>
	/// <param name="Output">Output product of Transform</param>
	/// <param name="OutOffset">Offset in the Output array</param>
	virtual void Transform(const std::vector<byte> &Input, const size_t InOffset, std::vector<byte> &Output, const size_t OutOffset);

	/// <summary>
	/// Encrypt/Decrypt an array of bytes with offset and length parameters.
	/// <para><see cref="Initialize(KeyParams)"/> must be called before this method can be used.</para>
	/// </summary>
	/// 
	/// <param name="Input">Input bytes to Transform</param>
	/// <param name="InOffset">Offset in the Input array</param>
	/// <param name="Output">Output product of Transform</param>
	/// <param name="OutOffset">Offset in the Output array</param>
	/// <param name="Length">Number of bytes to process</param>
	virtual void Transform(const std::vector<byte> &Input, const size_t InOffset, std::vector<byte> &Output, const size_t OutOffset, const size_t Length);

private:
	void ChaChaCore(std::vector<byte> &Output, const size_t OutOffset, std::vector<uint> &Counter);
	void Generate(const size_t Size, std::vector<uint> &Counter, std::vector<byte> &Output, const size_t OutOffset);
	uint GetProcessorCount();
	void Increase(const std::vector<uint> &Counter, const size_t Size, std::vector<uint> &Buffer);
	void Increment(std::vector<uint> &Counter);
	void ProcessBlock(const std::vector<byte> &Input, const size_t InOffset, std::vector<byte> &Output, const size_t OutOffset, const size_t Length);
	void SetKey(const std::vector<byte> &Key, const std::vector<byte> &Iv);
	void SetScope();
};

NAMESPACE_STREAMEND
#endif
