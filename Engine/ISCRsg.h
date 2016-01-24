#ifndef _CEXENGINE_ISCPRSG_H
#define _CEXENGINE_ISCPRSG_H

#include "ISeed.h"

NAMESPACE_SEED

/// <summary>
/// ISCRsg: Generates seed material using an ISAAC random number generator.
/// <para>A high speed, cryptographically secure pseudo random provider.</para>
/// </summary>
/// 
/// <example>
/// <description>Example of getting a seed value:</description>
/// <code>
/// ISCRsg gen(Seed);
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
/// <item><description>ISAAC: a fast cryptographic random number generator: <see href="http://www.burtleburtle.net/bob/rand/isaacafa.html"/>.</description></item>
/// <item><description>Rossettacode example implementations: <see href="http://rosettacode.org/wiki/The_ISAAC_Cipher"/>.</description></item>
/// </list>
/// </remarks>
class ISCRsg : public ISeed
{
protected:
	static constexpr int SIZE32 = 4;
	static constexpr int SIZE64 = 8;
	static constexpr int MSIZE = 1 << SIZE64;
	static constexpr int MASK = (MSIZE - 1) << 2;
	static constexpr uint GDNR = 0x9e3779b9; // golden ratio

	int _accululator;
	int _cycCounter;
	bool _isDestroyed;
	int _lstResult;
	int _rndCount;
	std::vector<int> _rndResult;
	uint _rslCounter;
	std::vector<int> _wrkBuffer;

public:
	// *** Properties *** //

	/// <summary>
	/// Get: The seed generators type name
	/// </summary>
	virtual const SeedGenerators Enumeral() { return SeedGenerators::ISCRsg; }

	/// <summary>
	/// Get: Cipher name
	/// </summary>
	virtual const char *Name() { return "ISCRsg"; }

	// *** Constructor *** //

	/// <summary>
	/// Initialize this class using a seed value
	/// </summary>
	///
	/// <param name="Seed">The initial state values; must be between 2 and 256, 32bit values</param>
	///
	/// <exception cref="CEX::Exception::CryptoRandomException">Thrown if an invalid seed size is used</exception>
	ISCRsg(const std::vector<int> &Seed)
		:
		_accululator(0),
		_cycCounter(0),
		_isDestroyed(false),
		_lstResult(0),
		_rndCount(0),
		_rndResult(MSIZE),
		_rslCounter(0),
		_wrkBuffer(MSIZE)
	{
		if (Seed.size() < 1 && Seed.size() > 256)
			throw CryptoRandomException("ISCRsg:CTor", "The seed array length must be between 1 and 256 int32 values!");

		unsigned int len = Seed.size() > MSIZE ? MSIZE : Seed.size();
		memcpy(&_rndResult[0], &Seed[0], len * sizeof(int));
		Initialize(true);
	}

	/// <summary>
	/// Destructor
	/// </summary>
	virtual ~ISCRsg()
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
	/// <param name="Input">The array to fill</param>
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
	/// initializes the generator with new state
	/// </summary>
	/// 
	/// <param name="MixState">Mix with the initial state values</param>
	void Initialize(bool MixState = true);

	/// <summary>
	/// Returns the next pseudo random 32bit integer
	/// </summary>
	virtual int Next();

	/// <summary>
	/// Reinitialize the internal state using existing state as a seed
	/// </summary>
	virtual void Reset();
	
protected:
	void Generate();

	inline void Mix(int &A, int &B, int &C, int &D, int &E, int &F, int &G, int &H)
	{
		A ^= B << 11;
		D += A;
		B += C;
		B ^= (int)((uint)C >> 2);
		E += B;
		C += D;
		C ^= D << 8;
		F += C;
		D += E;
		D ^= (int)((uint)E >> 16);
		G += D;
		E += F;
		E ^= F << 10;
		H += E;
		F += G;
		F ^= (int)((uint)G >> 4);
		A += F;
		G += H;
		G ^= H << 8;
		B += G;
		H += A;
		H ^= (int)((uint)A >> 9);
		C += H;
		A += B;
	}
};

NAMESPACE_SEEDEND
#endif
