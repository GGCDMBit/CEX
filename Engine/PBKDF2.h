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

#ifndef _CEXENGINE_PBKDF2_H
#define _CEXENGINE_PBKDF2_H

#include "IGenerator.h"
#include "IDigest.h"
#include "HMAC.h"

NAMESPACE_GENERATOR

using CEX::Mac::HMAC;
using CEX::Digest::IDigest;

/// <summary>
/// PBKDF2 V2: An implementation of an Hash based Key Derivation Function.
/// <para>PBKDF2 Version 2, as outlined in RFC 2898</para>
/// </summary> 
/// 
/// <example>
/// <description>Generate an array of pseudo random bytes:</description>
/// <code>
/// PBKDF2 rnd(new SHA512(), 10000);
/// // initialize
/// rnd.Initialize(Salt, Ikm, [Nonce]);
/// // generate bytes
/// rnd.Generate(Output, [Offset], [Size]);
/// </code>
/// </example>
/// 
/// <revisionHistory>
/// <revision date="2015/11/20" version="1.0.0.0">Initial C++ Library implemention</revision>
/// </revisionHistory>
/// 
/// <seealso cref="CEX::Mac::HMAC">CEX::Mac HMAC</seealso>
/// <seealso cref="CEX::Digest::IDigest">CEX::Digest IDigest Interface</seealso>
/// <seealso cref="CEX::Enumeration::Digests">CEX::Enumeration Digests Enumeration</seealso>
/// 
/// <remarks>
/// <description><h4>Implementation Notes:</h4></description>
/// <list type="bullet">
/// <item><description>Can be initialized with a <see cref="CEX::Enumeration::Digests">Digest</see> or a <see cref="CEX::Enumeration::Macs">Mac</see>.</description></item>
/// <item><description>Salt size should be multiple of Digest block size.</description></item>
/// <item><description>Ikm size should be Digest hash return size.</description></item>
/// <item><description>Nonce and Ikm are optional, (but recommended).</description></item>
/// </list>
/// 
/// <description><h4>Guiding Publications:</h4></description>
/// <list type="number">
/// <item><description>RFC 2898: <see href="http://tools.ietf.org/html/rfc2898">Specification</see>.</description></item>
/// </list>
/// </remarks>
class PBKDF2 : public IGenerator
{
protected:
	static constexpr unsigned int PKCS_ITERATIONS = 1000;

	unsigned int _blockSize;
	HMAC* _digestMac;
	unsigned int _hashSize;
	bool _isDestroyed;
	bool _isInitialized;
	std::vector<byte> _macKey;
	IDigest* _msgDigest;
	unsigned int _prcIterations;
	std::vector<byte> _macSalt;

public:

	// *** Properties *** //

	/// <summary>
	/// Get: The generators type name
	/// </summary>
	virtual const Generators Enumeral() { return Generators::PBKDF2; }

	/// <summary>
	/// Get: Generator is ready to produce data
	/// </summary>
	virtual const bool IsInitialized() { return _isInitialized; }

	/// <summary>
	/// Get: The current state of the initialization Vector
	/// </summary>
	virtual const std::vector<byte> IV() { return _macKey; }

	/// <summary>
	/// <para>Minimum initialization key size in bytes; 
	/// combined sizes of Salt, Ikm, and Nonce must be at least this size.</para>
	/// </summary>
	virtual unsigned int KeySize() { return _blockSize; }

	/// <summary>
	/// Get: Cipher name
	/// </summary>
	virtual const char *Name() { return "PBKDF2"; }

	// *** Constructor *** //

	/// <summary>
	/// Creates a PBKDF2 Bytes Generator based on the given hash function
	/// </summary>
	/// 
	/// <param name="Digest">The digest used</param>
	/// <param name="Iterations">The number of cycles used to produce output</param>
	/// 
	/// <exception cref="CEX::Exception::CryptoGeneratorException">Thrown if a null Digest or Iterations count is used</exception>
	PBKDF2(IDigest* Digest, unsigned int Iterations = PKCS_ITERATIONS)
		:
		_blockSize(Digest->BlockSize()),
		_hashSize(Digest->DigestSize()),
		_isDestroyed(false),
		_isInitialized(false),
		_macKey(0),
		_msgDigest(Digest),
		_prcIterations(Iterations),
		_macSalt(0)
	{
		if (_prcIterations == 0)
			throw CryptoGeneratorException("PBKDF2:CTor", "Iterations count can not be zero!");

		_digestMac = new HMAC(_msgDigest);
	}

	/// <summary>
	/// Creates a PBKDF2 Bytes Generator based on the given hash function
	/// </summary>
	/// 
	/// <param name="Digest">The digest used</param>
	/// <param name="Iterations">The number of cycles used to produce output</param>
	/// 
	/// <exception cref="CEX::Exception::CryptoGeneratorException">Thrown if a null Digest or invalid Iterations count is used</exception>
	PBKDF2(HMAC* Hmac, unsigned int Iterations = PKCS_ITERATIONS)
		:
		_blockSize(Hmac->BlockSize()),
		_digestMac(Hmac),
		_hashSize(Hmac->MacSize()),
		_isDestroyed(false),
		_isInitialized(false),
		_macKey(0),
		_macSalt(0),
		_prcIterations(Iterations)
	{
		if (_prcIterations == 0)
			throw CryptoGeneratorException("PBKDF2:CTor", "Iterations count can not be zero!");
		if (!_digestMac->IsInitialized())
			throw CryptoGeneratorException("PBKDF2:CTor", "The HMAC has not been initialized!");
	}

	/// <summary>
	/// Finalize objects
	/// </summary>
	virtual ~PBKDF2()
	{
		Destroy();
	}

	// *** Public Methods *** //

	/// <summary>
	/// Release all resources associated with the object
	/// </summary>
	virtual void Destroy();

	/// <summary>
	/// Generate a block of pseudo random bytes
	/// </summary>
	/// 
	/// <param name="Output">Output array filled with random bytes</param>
	/// 
	/// <returns>Number of bytes generated</returns>
	virtual unsigned int Generate(std::vector<byte> &Output);

	/// <summary>
	/// Generate pseudo random bytes
	/// </summary>
	/// 
	/// <param name="Output">Output array filled with random bytes</param>
	/// <param name="OutOffset">The starting position within Output array</param>
	/// <param name="Size">Number of bytes to generate</param>
	/// 
	/// <returns>Number of bytes generated</returns>
	///
	/// <exception cref="CEX::Exception::CryptoGeneratorException">Thrown if the output buffer is too small</exception>
	virtual unsigned int Generate(std::vector<byte> &Output, unsigned int OutOffset, unsigned int Size);

	/// <summary>
	/// Initialize the generator
	/// </summary>
	/// 
	/// <param name="Salt">Salt value; minimum size is 2* the digests output size</param>
	/// 
	/// <exception cref="CEX::Exception::CryptoGeneratorException">Thrown if the Salt is too small</exception>
	virtual void Initialize(const std::vector<byte> &Salt);

	/// <summary>
	/// Initialize the generator
	/// </summary>
	/// 
	/// <param name="Salt">Salt value</param>
	/// <param name="Ikm">Key material</param>
	virtual void Initialize(const std::vector<byte> &Salt, const std::vector<byte> &Ikm);

	/// <summary>
	/// Initialize the generator
	/// </summary>
	/// 
	/// <param name="Salt">Salt value</param>
	/// <param name="Ikm">Key material</param>
	/// <param name="Nonce">Nonce value</param>
	virtual void Initialize(const std::vector<byte> &Salt, const std::vector<byte> &Ikm, const std::vector<byte> &Nonce);

	/// <summary>
	/// Update the Salt material
	/// </summary>
	/// 
	/// <param name="Salt">Pseudo random seed material</param>
	/// 
	/// <exception cref="CEX::Exception::CryptoGeneratorException">Thrown if the Salt value is too small</exception>
	virtual void Update(const std::vector<byte> &Salt);

protected:
	unsigned int GenerateKey(std::vector<byte> &Output, unsigned int OutOffset, unsigned int Size);
	void IntToOctet(std::vector<byte> &Output, unsigned int Counter);
	void Process(std::vector<byte> Input, std::vector<byte> &Output, unsigned int OutOffset);
};

NAMESPACE_GENERATOREND
#endif
