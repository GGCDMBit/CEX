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
// Cipher implementation based on the Rijndael block cipher designed by Joan Daemen and Vincent Rijmen:
// Rijndael <see href="http://csrc.nist.gov/archive/aes/rijndael/Rijndael-ammended.pdf">Specification</see>.
// AES specification <see href="http://csrc.nist.gov/publications/fips/fips197/fips-197.pdf">Fips 197</see>.
// 
// Implementation Details:
// An implementation based on the Rijndael block cipher, 
// using HKDF with a selectable Message Digest for expanded key generation.
// Rijndael HKDF Extended (RHX)
// Written by John Underhill, November 11, 2014
// contact: develop@vtdev.com

#ifndef _CEXENGINE_RHX_H
#define _CEXENGINE_RHX_H

#include "IBlockCipher.h"

NAMESPACE_BLOCK

/// <summary>
/// RHX: A Rijndael Cipher extended with an (optional) HKDF powered Key Schedule.
/// <para>RHX is a Rijndael implementation that uses a standard configuration on key sizes up to 64 bytes (512 bits). 
/// On keys larger than 64 bytes, an HKDF bytes generator is used to expand the user supplied key into a working key integer array.</para>
/// </summary> 
/// 
/// <example>
/// <description>Example using an <c>ICipherMode</c> interface:</description>
/// <code>
/// using (ICipherMode cipher = new CTR(new RHX()))
/// {
///     // initialize for encryption
///     cipher.Initialize(true, new KeyParams(Key, IV));
///     // encrypt a block
///     cipher.Transform(Input, Output);
/// }
/// </code>
/// </example>
/// 
/// <revisionHistory>
/// <revision date="2014/09/18" version="1.5.0.0">Initial release</revision>
/// </revisionHistory>
/// 
/// <seealso cref="CEX::Digest::IDigest">CEX::Digest IDigest Interface</seealso>
/// 
/// <remarks>
/// <description><h4>Implementation Notes:</h4></description>
/// <para>The key schedule in RHX is the defining difference between this and a standard version of Rijndael.
/// if the cipher key input is beyond the standard lengths used in Rijndael (128-512 bits), instead of using an inline function to expand the user supplied key into a larger working array, 
/// RHX uses a hash based pseudo-random generator to create the internal working key array.
/// When using a non-standard key size, the number of diffusion rounds can be set by the user (through the class constructor). RHX can run between 10 and 38 rounds.
/// </para>
/// 
/// <list type="bullet">
/// <item><description>When using a standard cipher key length the rounds calculation is done automatically: 10, 12, 14, and 22, for key sizes 126, 192, 256, and 512 bits.</description></item>
/// <item><description>HKDF Digest <see cref="Digests">engine</see> is definable through the <see cref="RHX(unsigned int, unsigned int, Digests)">Constructor</see> parameter: KeyEngine.</description></item>
/// <item><description>Key Schedule is powered by a Hash based Key Derivation Function using a user definable <see cref="IDigest">Digest</see>.</description></item>
/// <item><description>Minimum key size is (IKm + Salt) (N * Digest State Size) + (Digest Hash Size) in bytes.</description></item>
/// <item><description>Valid block sizes are 16 and 32 byte wide.</description></item>
/// <item><description>Valid Rounds are 10 to 38, default is 22.</description></item>
/// </list>
/// 
/// <description><h4>HKDF Bytes Generator:</h4></description>
/// <para>HKDF is a key derivation function that uses a Digest HMAC (Hash based Message Authentication Code) as its random engine. 
/// This is one of the strongest methods available for generating pseudo-random keying material, and far superior in entropy dispersion to Rijndael, or even Serpents key schedule. 
/// HKDF uses up to three inputs; a nonce value called an information string, an Ikm (Input keying material), and a Salt value. 
/// The HMAC RFC 2104, recommends a key size equal to the digest output, in the case of SHA512, 64 bytes, anything larger gets passed through the hash function to get the required 512 bit key size. 
/// The Salt size is a minimum of the hash functions block size, with SHA-2 512 that is 128 bytes.</para>
/// 
/// <para>When using SHA-2 512, a minimum key size for RHX is 192 bytes, further blocks of salt can be added to the key so long as they align; ikm + (n * blocksize), ex. 192, 320, 448 bytes.. there is no upper maximum. 
/// This means that you can create keys as large as you like so long as it falls on these boundaries, this effectively eliminates brute force as a means of attack on the cipher, even in post-quantum terms.</para> 
/// 
/// <para>The Digest that powers HKDF, can be any one of the Hash Digests implemented in the CEX library; Blake, Keccak, SHA-2, or Skein.
/// The default Digest Engine is SHA-2 512.</para>
/// 
/// <para>The legal key sizes are determined by a combination of the (Hash Size + a Multiplier * the Digest State Size); <math>klen = h + (n * s)</math>, this will vary between Digest implementations. 
/// Correct key sizes can be determined at runtime using the <see cref="LegalKeySizes"/> property.</para>
/// 
/// <para>The number of diffusion rounds processed within the ciphers rounds function can also be defined; adding rounds creates a more diffused cipher output, making the resulting cipher-text more difficult to cryptanalyze. 
/// RHX is capable of processing up to 38 rounds, that is twenty-four rounds more than the fourteen rounds used in an implementation of AES-256. 
/// Valid rounds assignments can be found in the <see cref="LegalRounds"/> static property.</para>
/// 
/// <description><h4>Guiding Publications:</h4></description>
/// <list type="number">
/// <item><description>NIST: <see href="http://csrc.nist.gov/publications/fips/fips197/fips-197.pdf">AES Fips 197</see>.</description></item>
/// <item><description>HMAC: <see href="http://tools.ietf.org/html/rfc2104">RFC 2104</see>.</description></item>
/// <item><description>Fips: <see href="http://csrc.nist.gov/publications/fips/fips198-1/FIPS-198-1_final.pdf">198.1</see>.</description></item>
/// <item><description>HKDF: <see href="http://tools.ietf.org/html/rfc5869">RFC 5869</see>.</description></item>
/// <item><description>NIST: <see href="http://csrc.nist.gov/publications/drafts/800-90/draft-sp800-90b.pdf">SP800-90B</see>.</description></item>
/// </list>
/// 
/// </remarks>
class RHX : public IBlockCipher
{
protected:
	static constexpr unsigned int BLOCK16 = 16;
	static constexpr unsigned int BLOCK32 = 32;
	static constexpr unsigned int LEGAL_KEYS = 14;
	static constexpr unsigned int MAX_ROUNDS = 38;
	static constexpr unsigned int MAX_STDKEY = 64;
	static constexpr unsigned int MIN_ROUNDS = 10;
	static constexpr unsigned int ROUNDS22 = 22;

	unsigned int _blockSize;
	bool _destroyEngine;
	unsigned int _dfnRounds;
	std::vector<uint> _expKey;
	std::vector<byte> _hkdfInfo;
	bool _isDestroyed;
	bool _isEncryption;
	bool _isInitialized;
	unsigned int _ikmSize;
	Digests _kdfEngineType;
	IDigest* _kdfEngine;
	std::vector<unsigned int> _legalKeySizes;
	std::vector<unsigned int> _legalRounds;

public:

	// *** Properties *** //

	/// <summary>
	/// Get: Unit block size of internal cipher in bytes.
	/// <para>Block size must be 16 or 32 bytes wide.
	/// Value set in class constructor.</para>
	/// </summary>
	virtual const unsigned int BlockSize() { return _blockSize; }

	/// <summary>
	/// Get/Set: Sets the Info value in the HKDF initialization parameters.
	/// <para>Must be set before <see cref="Initialize(bool, KeyParams)"/> is called.
	/// Changing this code will create a unique distribution of the cipher.
	/// Code can be either a zero byte array, or a multiple of the HKDF digest engines return size.</para>
	/// </summary>
	///
	/// <exception cref="CryptoSymmetricCipherException">Thrown if an invalid distribution code is used</exception>
	const std::vector<byte> &DistributionCode() { return _hkdfInfo; }

	/// <summary>
	/// Get: The block ciphers type name
	/// </summary>
	virtual const BlockCiphers Enumeral() { return BlockCiphers::RHX; }

	/// <summary>
	/// Get/Set: Specify the size of the HMAC key; extracted from the cipher key.
	/// <para>This property can only be changed before the Initialize function is called.</para>
	/// <para>Default is the digest return size; can only be a multiple of that length.
	/// Maximum size is the digests underlying block size; if the key
	/// is longer than this, the size will default to the block size.</para>
	/// </summary>
	unsigned int &IkmSize() { return _ikmSize; }

	/// <summary>
	/// Get: Initialized for encryption, false for decryption.
	/// <para>Value set in <see cref="Initialize(bool, KeyParams)"/>.</para>
	/// </summary>
	virtual const bool IsEncryption() { return _isEncryption; }

	/// <summary>
	/// Get: Cipher is ready to transform data
	/// </summary>
	virtual const bool IsInitialized() { return _isInitialized; }

	/// <summary>
	/// Get: Available Encryption Key Sizes in bytes
	/// </summary>
	virtual const std::vector<unsigned int> &LegalKeySizes() { return _legalKeySizes; }

	/// <summary>
	/// Get: Available diffusion round assignments
	/// </summary>
	virtual const std::vector<unsigned int> &LegalRounds() { return _legalRounds; }

	/// <summary>
	/// Get: Cipher name
	/// </summary>
	virtual const char* Name() { return "RHX"; }

	/// <summary>
	/// Get: The number of diffusion rounds processed by the transform
	/// </summary>
	virtual const unsigned int Rounds() { return _dfnRounds; }

	// *** Constructor *** //

	/// <summary>
	/// Initialize the class with a Digest instance
	/// </summary>
	///
	/// <param name="KdfEngine">The Key Schedule KDF digest engine; can be any one of the <see cref="CEX::Enumeration::Digests">Digest</see> implementations.</param>
	/// <param name="Rounds">Number of diffusion rounds. The <see cref="LegalRounds"/> property contains available sizes.  Default is 22 rounds.</param>
	/// <param name="BlockSize">Cipher input <see cref="BlockSize"/>. Default is 16 bytes.</param>
	///
	/// <exception cref="CEX::Exception::CryptoSymmetricCipherException">Thrown if an invalid block size or invalid rounds count are used</exception>
	RHX(IDigest *KdfEngine, unsigned int Rounds = ROUNDS22, unsigned int BlockSize = BLOCK16)
		:
		_blockSize(BlockSize),
		_destroyEngine(false),
		_dfnRounds(Rounds),
		_hkdfInfo(0, 0),
		_ikmSize(0),
		_kdfEngine(KdfEngine),
		_isEncryption(false),
		_isInitialized(false),
		_legalKeySizes(LEGAL_KEYS, 0),
		_legalRounds(15, 0)
	{
		if (BlockSize != BLOCK16 && BlockSize != BLOCK32)
			throw CryptoSymmetricCipherException("RHX:CTor", "Invalid block size! Supported block sizes are 16 and 32 bytes.");
		if (Rounds < MIN_ROUNDS || Rounds > MAX_ROUNDS || Rounds % 2 > 0)
			throw CryptoSymmetricCipherException("RHX:CTor", "Invalid rounds size! Sizes supported are even numbers between 10 and 38.");

		std::string info = "information string RHX version 1";
		_hkdfInfo.reserve(info.size());
		for (unsigned int i = 0; i < info.size(); ++i)
			_hkdfInfo.push_back(info[i]);

		_legalRounds = { 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38 };
		_kdfEngineType = KdfEngine->Enumeral();
		// set the hmac key size
		_ikmSize = _ikmSize == 0 ? _kdfEngine->DigestSize() : _ikmSize;
		// add standard key lengths
		_legalKeySizes[0] = 16;
		_legalKeySizes[1] = 24;
		_legalKeySizes[2] = 32;
		_legalKeySizes[3] = 64;

		for (unsigned int i = 4; i < _legalKeySizes.size(); i++)
			_legalKeySizes[i] = (_kdfEngine->BlockSize() * (i - 3)) + _ikmSize;
	}

	/// <summary>
	/// Initialize the class
	/// </summary>
	/// 
	/// <param name="Rounds">Number of diffusion rounds. The <see cref="LegalRounds"/> property contains available sizes.  Default is 22 rounds.</param>
	/// <param name="BlockSize">Cipher input <see cref="BlockSize"/>. Default is 16 bytes.</param>
	/// <param name="KdfEngineType"><para>The Key Schedule KDF digest engine; can be any one of the <see cref="CEX::Enumeration::Digests">Digest</see> 
	/// implementations. The default engine is SHA512</para>.</param>
	/// 
	/// <exception cref="CEX::Exception::CryptoSymmetricCipherException">Thrown if an invalid block size or invalid rounds count are used</exception>
	RHX(unsigned int BlockSize = BLOCK16, unsigned int Rounds = ROUNDS22, Digests KdfEngineType = Digests::SHA512)
		:
		_blockSize(BlockSize),
		_destroyEngine(true),
		_dfnRounds(Rounds),
		_hkdfInfo(0, 0),
		_ikmSize(0),
		_isEncryption(false),
		_isInitialized(false),
		_kdfEngineType(KdfEngineType),
		_legalKeySizes(LEGAL_KEYS, 0),
		_legalRounds(15, 0)
	{
		if (BlockSize != BLOCK16 && BlockSize != BLOCK32)
			throw CryptoSymmetricCipherException("RHX:CTor", "Invalid block size! Supported block sizes are 16 and 32 bytes.");
		if (Rounds < MIN_ROUNDS || Rounds > MAX_ROUNDS || Rounds % 2 > 0)
			throw CryptoSymmetricCipherException("RHX:CTor", "Invalid rounds size! Sizes supported are even numbers between 10 and 38.");

		std::string info = "information string RHX version 1";
		_hkdfInfo.reserve(info.size());
		for (unsigned int i = 0; i < info.size(); ++i)
			_hkdfInfo.push_back(info[i]);

		_legalRounds = { 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38 };
		// set the hmac key size
		_ikmSize = _ikmSize == 0 ? GetIkmSize(KdfEngineType) : _ikmSize;
		// add standard key lengths
		_legalKeySizes[0] = 16;
		_legalKeySizes[1] = 24;
		_legalKeySizes[2] = 32;
		_legalKeySizes[3] = 64;

		int dgtblock = GetSaltSize(KdfEngineType);

		// hkdf extended key sizes
		for (unsigned int i = 4; i < _legalKeySizes.size(); ++i)
			_legalKeySizes[i] = (dgtblock * (i - 3)) + _ikmSize;
	}

	/// <summary>
	/// Finalize objects
	/// </summary>
	virtual ~RHX()
	{
		Destroy();
	}

	// *** Public Methods *** //

	/// <summary>
	/// Decrypt a single block of bytes.
	/// <para><see cref="Initialize(bool, KeyParams)"/> must be called with the Encryption flag set to <c>false</c> before this method can be used.
	/// Input and Output arrays must be at least <see cref="BlockSize"/> in length.</para>
	/// </summary>
	///
	/// <param name="Input">Encrypted bytes</param>
	/// <param name="Output">Decrypted bytes</param>
	virtual void DecryptBlock(const std::vector<byte> &Input, std::vector<byte> &Output);

	/// <summary>
	/// Decrypt a block of bytes with offset parameters.
	/// <para><see cref="Initialize(bool, KeyParams)"/> must be called with the Encryption flag set to <c>false</c> before this method can be used.
	/// Input and Output arrays with Offsets must be at least <see cref="BlockSize"/> in length.</para>
	/// </summary>
	///
	/// <param name="Input">Encrypted bytes</param>
	/// <param name="InOffset">Offset in the Input array</param>
	/// <param name="Output">Decrypted bytes</param>
	/// <param name="OutOffset">Offset in the Output array</param>
	virtual void DecryptBlock(const std::vector<byte> &Input, const unsigned int InOffset, std::vector<byte> &Output, const unsigned int OutOffset);

	/// <summary>
	/// Clear the buffers and reset
	/// </summary>
	virtual void Destroy();

	/// <summary>
	/// Encrypt a block of bytes.
	/// <para><see cref="Initialize(bool, KeyParams)"/> must be called with the Encryption flag set to <c>true</c> before this method can be used.
	/// Input and Output array lengths must be at least <see cref="BlockSize"/> in length.</para>
	/// </summary>
	///
	/// <param name="Input">Input bytes to Transform</param>
	/// <param name="Output">Output product of Transform</param>
	virtual void EncryptBlock(const std::vector<byte> &Input, std::vector<byte> &Output);

	/// <summary>
	/// Encrypt a block of bytes with offset parameters.
	/// <para><see cref="Initialize(bool, KeyParams)"/> must be called with the Encryption flag set to <c>true</c> before this method can be used.
	/// Input and Output arrays with Offsets must be at least <see cref="BlockSize"/> in length.</para>
	/// </summary>
	///
	/// <param name="Input">Input bytes to Transform</param>
	/// <param name="InOffset">Offset in the Input array</param>
	/// <param name="Output">Output product of Transform</param>
	/// <param name="OutOffset">Offset in the Output array</param>
	virtual void EncryptBlock(const std::vector<byte> &Input, const unsigned int InOffset, std::vector<byte> &Output, const unsigned int OutOffset);

	/// <summary>
	/// Initialize the Cipher.
	/// </summary>
	///
	/// <param name="Encryption">Using Encryption or Decryption mode</param>
	/// <param name="KeyParam">Cipher key container. <para>The <see cref="LegalKeySizes"/> property contains valid sizes.</para></param>
	///
	/// <exception cref="CryptoSymmetricCipherException">Thrown if a null or invalid key is used</exception>
	virtual void Initialize(bool Encryption, const KeyParams &KeyParam);

	/// <summary>
	/// Transform a block of bytes.
	/// <para><see cref="Initialize(bool, KeyParams)"/> must be called before this method can be used.
	/// Input and Output array lengths must be at least <see cref="BlockSize"/> in length.</para>
	/// </summary>
	///
	/// <param name="Input">Input bytes to Transform or Decrypt</param>
	/// <param name="Output">Output product of Transform</param>
	virtual void Transform(const std::vector<byte> &Input, std::vector<byte> &Output);

	/// <summary>
	/// Transform a block of bytes with offset parameters.
	/// <para><see cref="Initialize(bool, KeyParams)"/> must be called before this method can be used.
	/// Input and Output arrays with Offsets must be at least <see cref="BlockSize"/> in length.</para>
	/// </summary>
	///
	/// <param name="Input">Input bytes to Transform</param>
	/// <param name="InOffset">Offset in the Input array</param>
	/// <param name="Output">Output product of Transform</param>
	/// <param name="OutOffset">Offset in the Output array</param>
	virtual void Transform(const std::vector<byte> &Input, const unsigned int InOffset, std::vector<byte> &Output, const unsigned int OutOffset);

protected:
	void Decrypt16(const std::vector<byte> &Input, const unsigned int InOffset, std::vector<byte> &Output, const unsigned int OutOffset);
	void Decrypt32(const std::vector<byte> &Input, const unsigned int InOffset, std::vector<byte> &Output, const unsigned int OutOffset);
	void Encrypt16(const std::vector<byte> &Input, const unsigned int InOffset, std::vector<byte> &Output, const unsigned int OutOffset);
	void Encrypt32(const std::vector<byte> &Input, const unsigned int InOffset, std::vector<byte> &Output, const unsigned int OutOffset);
	void ExpandKey(bool Encryption, const std::vector<byte> &Key);
	IDigest* GetDigest(Digests DigestType);
	int GetIkmSize(Digests DigestType);
	int GetSaltSize(Digests DigestType);
	void SecureExpand(const std::vector<byte> &Key);
	void StandardExpand(const std::vector<byte> &Key);
	uint SubByte(uint Rot);
};

NAMESPACE_BLOCKEND
#endif

