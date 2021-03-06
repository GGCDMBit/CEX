﻿#ifndef CEX_ISTREAMCIPHER_H
#define CEX_ISTREAMCIPHER_H

#include "CexDomain.h"
#include "CryptoSymmetricCipherException.h"
#include "IntUtils.h"
#include "ISymmetricKey.h"
#include "ParallelOptions.h"
#include "ParallelUtils.h"
#include "StreamCiphers.h"
#include "SymmetricKeySize.h"

NAMESPACE_STREAM

using Exception::CryptoSymmetricCipherException;
using Utility::IntUtils;
using Key::Symmetric::ISymmetricKey;
using Common::ParallelOptions;
using Enumeration::StreamCiphers;
using Key::Symmetric::SymmetricKeySize;

/// <summary>
/// Stream Cipher virtual interface class
/// </summary>
class IStreamCipher
{
public:

	//~~~Constructor~~~//

	/// <summary>
	/// CTor: Instantiate this class
	/// </summary>
	IStreamCipher() {}

	/// <summary>
	/// Destructor
	/// </summary>
	virtual ~IStreamCipher() {}

	//~~~Properties~~~//

	/// <summary>
	/// Get: Unit block size of internal cipher in bytes.
	/// <para>Block size must be 16 or 32 bytes wide. 
	/// Value set in class constructor.</para>
	/// </summary>
	virtual const size_t BlockSize() = 0;

	/// <summary>
	/// Get: The salt value in the initialization parameters (Tau-Sigma).
	/// <para>This value can only be set with the Info parameter of an ISymmetricKey member, or use the default.
	/// Changing this code will create a unique distribution of the cipher.
	/// For best security, the code should be a random extenion of the key, with rounds increased to 40 or more.
	/// Code must be non-zero, 16 bytes in length, and sufficiently asymmetric.
	/// If the Info parameter of an ISymmetricKey is non-zero, it will overwrite the distribution code.</para>
	/// </summary>
	virtual const std::vector<byte> &DistributionCode() = 0;

	/// <summary>
	/// Get: The stream ciphers type name
	/// </summary>
	virtual const StreamCiphers Enumeral() = 0;

	/// <summary>
	/// Get: Cipher is ready to transform data
	/// </summary>
	virtual const bool IsInitialized() = 0;

	/// <summary>
	/// Get: Processor parallelization availability.
	/// <para>Indicates whether parallel processing is available with this mode.
	/// If parallel capable, input/output data arrays passed to the transform must be ParallelBlockSize in bytes to trigger parallelization.</para>
	/// </summary>
	virtual const bool IsParallel() = 0;

	/// <summary>
	/// Get: Array of allowed cipher input key byte-sizes
	/// </summary>
	virtual const std::vector<SymmetricKeySize> &LegalKeySizes() = 0;

	/// <summary>
	/// Get: Available transformation round assignments
	/// </summary>
	virtual const std::vector<size_t> &LegalRounds() = 0;

	/// <summary>
	/// Get: The stream ciphers class name
	/// </summary>
	virtual const std::string Name() = 0;

	/// <summary>
	/// Get: Parallel block size; the byte-size of the input/output data arrays passed to a transform that trigger parallel processing.
	/// <para>This value can be changed through the ParallelProfile class.<para>
	/// </summary>
	virtual const size_t ParallelBlockSize() = 0;

	/// <summary>
	/// Get/Set: Parallel and SIMD capability flags and sizes 
	/// <para>The maximum number of threads allocated when using multi-threaded processing can be set with the ParallelMaxDegree() property.
	/// The ParallelBlockSize() property is auto-calculated, but can be changed; the value must be evenly divisible by ParallelMinimumSize().
	/// Changes to these values must be made before the <see cref="Initialize(SymmetricKey)"/> function is called.</para>
	/// </summary>
	virtual ParallelOptions &ParallelProfile() = 0;

	/// <summary>
	/// Get: Number of rounds
	/// </summary>
	virtual const size_t Rounds() = 0;

	//~~~Public Functions~~~//

	/// <summary>
	/// Release all resources associated with the object; optional, called by the finalizer
	/// </summary>
	virtual void Destroy() = 0;

	/// <summary>
	/// Initialize the cipher
	/// </summary>
	/// 
	/// <param name="KeyParams">Cipher key container. The LegalKeySizes property contains valid sizes</param>
	virtual void Initialize(ISymmetricKey &KeyParams) = 0;

	/// <summary>
	/// Set the maximum number of threads allocated when using multi-threaded processing.
	/// <para>When set to zero, thread count is set automatically. If set to 1, sets IsParallel() to false and runs in sequential mode. 
	/// Thread count must be an even number, and not exceed the number of processor cores.</para>
	/// </summary>
	///
	/// <param name="Degree">The desired number of threads</param>
	///
	/// <exception cref="Exception::CryptoCipherModeException">Thrown if an invalid degree setting is used</exception>
	virtual void ParallelMaxDegree(size_t Degree) = 0;

	/// <summary>
	/// Encrypt/Decrypt one block of bytes
	/// </summary>
	/// 
	/// <param name="Input">The input array of bytes to transform</param>
	/// <param name="Output">The output array of transformed bytes</param>
	virtual void TransformBlock(const std::vector<byte> &Input, std::vector<byte> &Output) = 0;

	/// <summary>
	/// Encrypt/Decrypt one block of bytes
	/// </summary>
	/// 
	/// <param name="Input">The input array of bytes to transform</param>
	/// <param name="InOffset">Starting offset within the input array</param>
	/// <param name="Output">The output array of transformed bytes</param>
	/// <param name="OutOffset">Starting offset within the output array</param>
	virtual void TransformBlock(const std::vector<byte> &Input, const size_t InOffset, std::vector<byte> &Output, const size_t OutOffset) = 0;

	/// <summary>
	/// Encrypt/Decrypt an array of bytes with offset and length parameters.
	/// <para><see cref="Initialize(SymmetricKey)"/> must be called before this method can be used.</para>
	/// </summary>
	/// 
	/// <param name="Input">The input array of bytes to transform</param>
	/// <param name="InOffset">Starting offset within the input array</param>
	/// <param name="Output">The output array of transformed bytes</param>
	/// <param name="OutOffset">Starting offset within the output array</param>
	/// <param name="Length">Length of data to process</param>
	virtual void Transform(const std::vector<byte> &Input, const size_t InOffset, std::vector<byte> &Output, const size_t OutOffset, const size_t Length) = 0;
};

NAMESPACE_STREAMEND
#endif
