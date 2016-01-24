#ifndef _CEXENGINE_XSPPRSG_H
#define _CEXENGINE_XSPPRSG_H

#include "ISeed.h"

NAMESPACE_SEED

/// <summary>
/// XSPRsg: Generates seed material using an XorShift+ generator.
/// <para>This generator is not generally considered a cryptographic quality generator. 
/// This generator is suitable as a quality high-speed number generator, but not to be used directly for tasks that require secrecy, ex. key generation.</para>
/// </summary>
/// 
/// <example>
/// <description>Example of getting a seed value:</description>
/// <code>
/// XSPRsg gen(Seed);
/// gen.GetSeed(Output);
/// </code>
/// </example>
/// 
/// <revisionHistory>
/// <revision date="2015/06/09" version="1.0.0.0">Initial release</revision>
/// </revisionHistory>
/// 
/// <remarks>
/// <description><h4>Guiding Publications:</h4></description>
/// <list type="number">
/// <item><description>Further scramblings of Marsaglia�s xorshift generators <see href="http://vigna.di.unimi.it/ftp/papers/xorshiftplus.pdf"/>.</description></item>
/// <item><description>Xorshift+ generators and the PRNG shootout: <see href="http://xorshift.di.unimi.it/"/>.</description></item>
/// </list>
/// </remarks>
class XSPRsg : public ISeed
{
protected:
	static constexpr int SIZE32 = 4;
	static constexpr int SIZE64 = 8;
	static constexpr ulong Z1 = 0x9E3779B97F4A7C15;
	static constexpr ulong Z2 = 0xBF58476D1CE4E5B9;
	static constexpr ulong Z3 = 0x94D049BB133111EB;
	static constexpr ulong Z4 = 1181783497276652981;

	bool _isDestroyed;
	bool _isShift1024;
	unsigned int _stateOffset;
	std::vector<ulong> _stateSeed;
	std::vector<ulong> _wrkBuffer;
	std::vector<ulong> JMP128;
	std::vector<ulong> JMP1024;

public:
	// *** Properties *** //

	/// <summary>
	/// Get: The seed generators type name
	/// </summary>
	virtual const SeedGenerators Enumeral() { return SeedGenerators::XSPRsg; }

	/// <summary>
	/// Get: Cipher name
	/// </summary>
	virtual const char *Name() { return "XSPRsg"; }

	// *** Constructor *** //

	/// <summary>
	/// Initialize this class.
	/// <para>Initializing with 2 ulongs invokes the 128 bit function, initializing with 16 ulongs
	/// invokes the 1024 bit function.</para>
	/// </summary>
	///
	/// <param name="Seed">The initial state values; can be either 2, or 16, 64bit values</param>
	///
	/// <exception cref="CEX::Exception::CryptoRandomException">Thrown if an invalid seed size is used</exception>
	XSPRsg(const std::vector<ulong> &Seed)
		:
		_isDestroyed(false),
		_isShift1024(false),
		_stateOffset(0),
		_stateSeed(Seed.size()),
		_wrkBuffer(Seed.size())
	{
		if (Seed.size() != 2 && Seed.size() != 16)
			throw CryptoRandomException("XSPRsg:CTor", "The seed array length must be either 2 or 16 long values!");

		for (unsigned int i = 0; i < Seed.size(); ++i)
		{
			if (Seed[i] == 0)
				throw CryptoRandomException("XSPRsg:CTor", "Seed values can not be zero!");
		}

		memcpy(&_stateSeed[0], &Seed[0], Seed.size() * sizeof(ulong));
		_isShift1024 = (Seed.size() == 16);

		if (!_isShift1024)
			JMP128 = { 0x8a5cd789635d2dffULL, 0x121fd2155c472f96ULL };
		else
			JMP1024 = { 0x84242f96eca9c41dULL,
				0xa3c65b8776f96855ULL, 0x5b34a39f070b5837ULL, 0x4489affce4f31a1eULL,
				0x2ffeeb0a48316f40ULL, 0xdc2d9891fe68c022ULL, 0x3659132bb12fea70ULL,
				0xaac17d8efa43cab8ULL, 0xc4cb815590989b13ULL, 0x5ee975283d71c93bULL,
				0x691548c86c1bd540ULL, 0x7910c41d10a1e6a5ULL, 0x0b5fc64563b3e2a8ULL,
				0x047f7684e9fc949dULL, 0xb99181f2d8f685caULL, 0x284600e3f30e38c3ULL
			};

		Reset();
	}

	/// <summary>
	/// Destructor
	/// </summary>
	virtual ~XSPRsg()
	{
		Destroy();
	}

	// *** Public Methods *** //

	/// <summary>
	/// Release all resources associated with the object
	/// </summary>
	virtual void Destroy();

	/// <summary>
	/// Fill the buffer with random bytes
	/// </summary>
	///
	/// <param name="Output">The array to fill</param>
	virtual void GetBytes(std::vector<byte> &Output);

	/// <summary>
	/// Get a pseudo random seed byte array
	/// </summary>
	/// 
	/// <param name="Size">The size of the expected seed returned</param>
	/// 
	/// <returns>A pseudo random seed</returns>
	virtual std::vector<byte> GetBytes(int Size);

	/// <summary>
	/// Increment the state by 64 blocks; used with the 128 and 1024 implementations
	/// </summary>
	void Jump();

	/// <summary>
	/// Returns the next pseudo random 32bit integer
	/// </summary>
	/// 
	/// <returns>A pseudo random 32bit integer</returns>
	virtual int Next();

	/// <summary>
	/// Reset the internal state
	/// </summary>
	virtual void Reset();

	/// <summary>
	/// Implementation of java's Splittable function
	/// </summary>
	/// 
	/// <param name="X">Input integer</param>
	/// 
	/// <returns>A processed long integer</returns>
	ulong Split(ulong X);

protected:
	void Jump128();
	void Jump1024();
	void Generate(std::vector<byte> &Output, unsigned int Size);
	ulong Shift128();
	ulong Shift1024();
};

NAMESPACE_SEEDEND
#endif
