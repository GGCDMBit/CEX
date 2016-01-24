﻿// The MIT License (MIT)
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

#ifndef _CEXENGINE_ICIPHERMODE_H
#define _CEXENGINE_ICIPHERMODE_H

#include "Common.h"
#include "CipherModes.h"
#include "CryptoCipherModeException.h"
#include "IBlockCipher.h"
#include "KeyParams.h"

NAMESPACE_MODE

using CEX::Common::KeyParams;
using CEX::Enumeration::CipherModes;
using CEX::Exception::CryptoCipherModeException;

/// <summary>
/// Cipher Mode Interface
/// </summary>
class ICipherMode
{
public:
	// *** Constructor *** //

	/// <summary>
	/// CTor: Initialize this class
	/// </summary>
	ICipherMode() {}

	/// <summary>
	/// Destructor
	/// </summary>
	virtual ~ICipherMode() {}

	// *** Properties *** //

	/// <summary>
	/// Get: Unit block size of internal cipher
	/// </summary>
	virtual const unsigned int BlockSize() = 0;

	/// <summary>
	/// Get: Underlying Cipher
	/// </summary>
	virtual IBlockCipher* Engine() = 0;

	/// <summary>
	/// Get: The cipher modes type name
	/// </summary>
	virtual const CipherModes Enumeral() = 0;

	/// <summary>
	/// Get: Initialized for encryption, false for decryption
	/// </summary>
	virtual const bool IsEncryption() = 0;

	/// <summary>
	/// Get: Cipher is ready to transform data
	/// </summary>
	virtual const bool IsInitialized() = 0;

	/// <summary>
	/// Get/Set: Automatic processor parallelization
	/// </summary>
	virtual bool &IsParallel() = 0;

	/// <summary>
	/// Get: The current state of the initialization Vector
	/// </summary>
	virtual const std::vector<byte> IV() = 0;

	/// <summary>
	/// Get: List of available legal key sizes
	/// </summary>
	virtual const std::vector<unsigned int> &LegalKeySizes() = 0;

	/// <summary>
	/// Get: Cipher name
	/// </summary>
	virtual const char* Name() = 0;

	/// <summary>
	/// Get/Set: Parallel block size. Must be a multiple of <see cref="ParallelMinimumSize"/>.
	/// </summary>
	///
	/// <exception cref="CryptoCipherModeException">Thrown if a parallel block size is not evenly divisible by ParallelMinimumSize, or  block size is less than ParallelMinimumSize or more than ParallelMaximumSize values</exception>
	virtual unsigned int &ParallelBlockSize() = 0;

	/// <summary>
	/// Get: Maximum input size with parallel processing
	/// </summary>
	virtual const unsigned int ParallelMaximumSize() = 0;

	/// <summary>
	/// Get: The smallest parallel block size. Parallel blocks must be a multiple of this size.
	/// </summary>
	virtual const unsigned int ParallelMinimumSize() = 0;

	/// <remarks>
	/// Get: Processor count
	/// </remarks>
	virtual const unsigned int ProcessorCount() = 0;

	// *** Public Methods *** //

	/// <summary>
	/// Release all resources associated with the object
	/// </summary>
	virtual void Destroy() = 0;

	/// <summary>
	/// Initialize the Cipher
	/// </summary>
	///
	/// <param name="Encryption">Cipher is used. for encryption, false to decrypt</param>
	/// <param name="KeyParam">The KeyParams containing key and vector</param>
	virtual void Initialize(bool Encryption, const KeyParams &KeyParam) = 0;

	/// <summary>
	/// <para>Transform a block of bytes. Parallel capable function if Output array length is at least equal to <see cref="ParallelMinimumSize"/>.
	/// Initialize() must be called before this method can be used.</para>
	/// </summary>
	///
	/// <param name="Input">Input bytes to Transform</param>
	/// <param name="Output">Output product of Transform</param>
	virtual void Transform(const std::vector<byte> &Input, std::vector<byte> &Output) = 0;

	/// <summary>
	/// <para>Transform a block of bytes with offset parameters.  Parallel capable function if Output array length is at least equal to <see cref="ParallelMinimumSize"/>.
	/// Initialize() must be called before this method can be used.</para>
	/// </summary>
	///
	/// <param name="Input">Input bytes to Transform</param>
	/// <param name="InOffset">Offset in the Input array</param>
	/// <param name="Output">Output product of Transform</param>
	/// <param name="OutOffset">Offset in the Output array</param>
	virtual void Transform(const std::vector<byte> &Input, const unsigned int InOffset, std::vector<byte> &Output, const unsigned int OutOffset) = 0;
};

NAMESPACE_MODEEND
#endif
