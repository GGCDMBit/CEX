#include "Keccak256.h"
#include "Keccak.h"
#include "IntUtils.h"
#include "MemUtils.h"
#include "ParallelUtils.h"

NAMESPACE_DIGEST

const std::string Keccak256::CLASS_NAME("Keccak512");

//~~~Properties~~~//

size_t Keccak256::BlockSize() 
{ 
	return BLOCK_SIZE; 
}

size_t Keccak256::DigestSize() 
{ 
	return DIGEST_SIZE; 
}

const Digests Keccak256::Enumeral() 
{ 
	return Digests::Keccak256; 
}

const bool Keccak256::IsParallel() 
{
	return m_parallelProfile.IsParallel(); 
}

const std::string Keccak256::Name()
{ 
	if (m_parallelProfile.IsParallel())
		return CLASS_NAME + "-P" + Utility::IntUtils::ToString(m_parallelProfile.ParallelMaxDegree());
	else
		return CLASS_NAME;
}

const size_t Keccak256::ParallelBlockSize() 
{ 
	return m_parallelProfile.ParallelBlockSize(); 
}

ParallelOptions &Keccak256::ParallelProfile() 
{ 
	return m_parallelProfile;
}

//~~~Constructor~~~//

Keccak256::Keccak256(bool Parallel)
	:
	m_treeParams(DIGEST_SIZE, static_cast<uint>(BLOCK_SIZE), DEF_PRLDEGREE),
	m_isDestroyed(false),
	m_msgBuffer(Parallel ? DEF_PRLDEGREE * BLOCK_SIZE : BLOCK_SIZE),
	m_msgLength(0),
	m_parallelProfile(BLOCK_SIZE, false, STATE_PRECACHED, false, DEF_PRLDEGREE),
	m_dgtState(Parallel ? DEF_PRLDEGREE : 1)
{
	if (m_parallelProfile.IsParallel())
		m_parallelProfile.IsParallel() = Parallel;

	Reset();
}

Keccak256::Keccak256(KeccakParams &Params)
	:
	m_treeParams(Params),
	m_dgtState(1),
	m_isDestroyed(false),
	m_msgBuffer(BLOCK_SIZE),
	m_msgLength(0),
	m_parallelProfile(BLOCK_SIZE, false, STATE_PRECACHED, false, m_treeParams.FanOut())
{
	if (m_treeParams.FanOut() > 1)
	{
		m_dgtState.resize(m_treeParams.FanOut());
		m_msgBuffer.resize(m_treeParams.FanOut() * BLOCK_SIZE);
	}
	else if (m_parallelProfile.IsParallel())
	{
		m_parallelProfile.IsParallel() = false;
	}

	Reset();
}

Keccak256::~Keccak256()
{
	Destroy();
}

//~~~Public Functions~~~//

void Keccak256::Compute(const std::vector<byte> &Input, std::vector<byte> &Output)
{
	Output.resize(DIGEST_SIZE);
	Update(Input, 0, Input.size());
	Finalize(Output, 0);
}

void Keccak256::Destroy()
{
	if (!m_isDestroyed)
	{
		m_isDestroyed = true;
		m_msgLength = 0;

		for (size_t i = 0; i < m_dgtState.size(); ++i)
			m_dgtState[i].Reset();

		Utility::IntUtils::ClearVector(m_dgtState);
		Utility::IntUtils::ClearVector(m_msgBuffer);
	}
}

size_t Keccak256::Finalize(std::vector<byte> &Output, size_t OutOffset)
{
	CexAssert(Output.size() - OutOffset >= DIGEST_SIZE, "The Output buffer is too short!");

	if (m_parallelProfile.IsParallel())
	{
		// pad buffer with zeros
		if (m_msgLength < m_msgBuffer.size())
			Utility::MemUtils::Clear(m_msgBuffer, m_msgLength, m_msgBuffer.size() - m_msgLength);

		// process buffer
		if (m_msgLength != 0)
		{
			size_t blkCtr = 0;

			while (m_msgLength != 0)
			{
				const size_t MSGRMD = (m_msgLength >= BLOCK_SIZE) ? BLOCK_SIZE : m_msgLength;
				HashFinal(m_msgBuffer, blkCtr * BLOCK_SIZE, MSGRMD, m_dgtState[blkCtr]);
				m_msgLength -= MSGRMD;
				++blkCtr;
			}
		}

		// initialize root state
		Keccak256State rootState;

		// add state blocks as contiguous message input
		for (size_t i = 0; i < m_dgtState.size(); ++i)
		{
			Utility::IntUtils::LeULL256ToBlock(m_dgtState[i].H, 0, m_msgBuffer, i * DIGEST_SIZE);
			m_msgLength += DIGEST_SIZE;
		}

		// compress full blocks
		size_t blkOff = 0;
		if (m_msgLength > BLOCK_SIZE)
		{
			const size_t BLKRMD = m_msgLength - (m_msgLength % BLOCK_SIZE);

			for (size_t i = 0; i < BLKRMD / BLOCK_SIZE; ++i)
				Keccak::Permute(m_msgBuffer, i * BLOCK_SIZE, BLOCK_SIZE, rootState.H);

			m_msgLength -= BLKRMD;
			blkOff = BLKRMD;
		}

		// finalize and store
		HashFinal(m_msgBuffer, blkOff, m_msgLength, rootState);
		Utility::IntUtils::LeULL256ToBlock(rootState.H, 0, Output, OutOffset);
	}
	else
	{
		if (m_msgLength != m_msgBuffer.size())
			Utility::MemUtils::Clear(m_msgBuffer, m_msgLength, m_msgBuffer.size() - m_msgLength);

		HashFinal(m_msgBuffer, 0, m_msgLength, m_dgtState[0]);
		Utility::IntUtils::LeULL256ToBlock(m_dgtState[0].H, 0, Output, OutOffset);
	}

	Reset();

	return DIGEST_SIZE;
}

void Keccak256::ParallelMaxDegree(size_t Degree)
{
	if (Degree == 0)
		throw CryptoDigestException("Skein256:ParallelMaxDegree", "Parallel degree can not be zero!");
	if (Degree > 254)
		throw CryptoDigestException("Skein256:ParallelMaxDegree", "Parallel degree can not exceed 254!");
	if (Degree % 2 != 0)
		throw CryptoDigestException("Skein256:ParallelMaxDegree", "Parallel degree must be an even number!");

	m_parallelProfile.SetMaxDegree(Degree);
	Reset();
}

void Keccak256::Reset()
{
	Utility::MemUtils::Clear(m_msgBuffer, 0, m_msgBuffer.size());
	m_msgLength = 0;

	for (size_t i = 0; i < m_dgtState.size(); ++i)
	{
		m_dgtState[i].Reset();

		if (m_parallelProfile.IsParallel())
		{
			m_treeParams.NodeOffset() = static_cast<uint>(i);
			Keccak::Permute(m_treeParams.ToBytes(), 0, BLOCK_SIZE, m_dgtState[i].H);
		}
	}
}

void Keccak256::Update(byte Input)
{
	std::vector<byte> one(1, Input);
	Update(one, 0, 1);
}

void Keccak256::Update(const std::vector<byte> &Input, size_t InOffset, size_t Length)
{
	CexAssert(Input.size() - InOffset >= Length, "The Output buffer is too short!");

	if (Length == 0)
		return;

	if (m_parallelProfile.IsParallel())
	{
		if (m_msgLength != 0 && Length + m_msgLength >= m_msgBuffer.size())
		{
			// fill buffer
			const size_t RMDLEN = m_msgBuffer.size() - m_msgLength;
			if (RMDLEN != 0)
				Utility::MemUtils::Copy(Input, InOffset, m_msgBuffer, m_msgLength, RMDLEN);

			// empty the message buffer
			Utility::ParallelUtils::ParallelFor(0, m_parallelProfile.ParallelMaxDegree(), [this, &Input, InOffset](size_t i)
			{
				Keccak::Permute(m_msgBuffer, i * BLOCK_SIZE, BLOCK_SIZE, m_dgtState[i].H);
			});

			m_msgLength = 0;
			Length -= RMDLEN;
			InOffset += RMDLEN;
		}

		if (Length >= m_parallelProfile.ParallelBlockSize())
		{
			// calculate working set size
			const size_t PRCLEN = Length - (Length % m_parallelProfile.ParallelBlockSize());

			// process large blocks
			Utility::ParallelUtils::ParallelFor(0, m_parallelProfile.ParallelMaxDegree(), [this, &Input, InOffset, PRCLEN](size_t i)
			{
				ProcessLeaf(Input, InOffset + (i * BLOCK_SIZE), m_dgtState[i], PRCLEN);
			});

			Length -= PRCLEN;
			InOffset += PRCLEN;
		}

		if (Length >= m_parallelProfile.ParallelMinimumSize())
		{
			const size_t PRMLEN = Length - (Length % m_parallelProfile.ParallelMinimumSize());

			Utility::ParallelUtils::ParallelFor(0, m_parallelProfile.ParallelMaxDegree(), [this, &Input, InOffset, PRMLEN](size_t i)
			{
				ProcessLeaf(Input, InOffset + (i * BLOCK_SIZE), m_dgtState[i], PRMLEN);
			});

			Length -= PRMLEN;
			InOffset += PRMLEN;
		}
	}
	else
	{
		if (m_msgLength != 0 && (m_msgLength + Length >= BLOCK_SIZE))
		{
			const size_t RMDLEN = BLOCK_SIZE - m_msgLength;
			if (RMDLEN != 0)
				Utility::MemUtils::Copy(Input, InOffset, m_msgBuffer, m_msgLength, RMDLEN);

			Keccak::Permute(m_msgBuffer, 0, BLOCK_SIZE, m_dgtState[0].H);
			m_msgLength = 0;
			InOffset += RMDLEN;
			Length -= RMDLEN;
		}

		// sequential loop through blocks
		while (Length >= BLOCK_SIZE)
		{
			Keccak::Permute(Input, InOffset, BLOCK_SIZE, m_dgtState[0].H);
			InOffset += BLOCK_SIZE;
			Length -= BLOCK_SIZE;
		}
	}

	// store unaligned bytes
	if (Length != 0)
	{
		Utility::MemUtils::Copy(Input, InOffset, m_msgBuffer, m_msgLength, Length);
		m_msgLength += Length;
	}
}

//~~~Private Functions~~~//

void Keccak256::HashFinal(std::vector<byte> &Input, size_t InOffset, size_t Length, Keccak256State &State)
{
	Input[InOffset + Length] = 1;
	Input[InOffset + BLOCK_SIZE - 1] |= 128;
	Keccak::Permute(Input, InOffset, BLOCK_SIZE, State.H);

	State.H[1] = ~State.H[1];
	State.H[2] = ~State.H[2];
	State.H[8] = ~State.H[8];
	State.H[12] = ~State.H[12];
	State.H[17] = ~State.H[17];
}

void Keccak256::ProcessLeaf(const std::vector<byte> &Input, size_t InOffset, Keccak256State &State, ulong Length)
{
	do
	{
		Keccak::Permute(Input, InOffset, BLOCK_SIZE, State.H);
		InOffset += m_parallelProfile.ParallelMinimumSize();
		Length -= m_parallelProfile.ParallelMinimumSize();
	} 
	while (Length > 0);
}

NAMESPACE_DIGESTEND