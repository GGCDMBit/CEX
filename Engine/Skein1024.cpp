#include "Skein1024.h"
#include "IntUtils.h"

NAMESPACE_DIGEST

using CEX::Utility::IntUtils;

void Skein1024::BlockUpdate(const std::vector<byte> &Input, unsigned int InOffset, unsigned int Length)
{
	if ((unsigned int)(InOffset + Length) > Input.size())
		throw CryptoDigestException("Skein1024:BlockUpdate", "The Input buffer is too short!");

	unsigned int bytesDone = 0;

	// fill input buffer
	while (bytesDone < Length && InOffset < Input.size())
	{
		// do a transform if the input buffer is filled
		if (_bytesFilled == STATE_BYTES)
		{
			// moves the byte input buffer to the UInt64 cipher input
			for (int i = 0; i < STATE_WORDS; i++)
				_cipherInput[i] = IntUtils::BytesToLe64(_inputBuffer, i * 8);

			// process the block
			ProcessBlock(STATE_BYTES);
			// clear first flag, which will be set by Initialize() if this is the first transform
			_ubiParameters.SetIsFirstBlock(false);
			// reset buffer fill count
			_bytesFilled = 0;
		}

		_inputBuffer[_bytesFilled++] = Input[InOffset++];
		bytesDone++;
	}
}

void Skein1024::ComputeHash(const std::vector<byte> &Input, std::vector<byte> &Output)
{
	Output.resize(DIGEST_SIZE);
	BlockUpdate(Input, 0, Input.size());
	DoFinal(Output, 0);
	Reset();
}

void Skein1024::Destroy()
{
	if (!_isDestroyed)
	{
		_isDestroyed = true;
		_bytesFilled = 0;
		_blockCipher.Clear();
		_ubiParameters.Clear();

		IntUtils::ClearVector(_cipherInput);
		IntUtils::ClearVector(_configString);
		IntUtils::ClearVector(_configValue);
		IntUtils::ClearVector(_digestState);
		IntUtils::ClearVector(_inputBuffer);
	}
}

unsigned int Skein1024::DoFinal(std::vector<byte> &Output, const unsigned int OutOffset)
{
	if (Output.size() - OutOffset < DIGEST_SIZE)
		throw CryptoDigestException("Skein1024:DoFinal", "The Output buffer is too short!");

	// pad left over space in input buffer with zeros
	for (unsigned int i = _bytesFilled; i < _inputBuffer.size(); i++)
		_inputBuffer[i] = 0;
	// copy to cipher input buffer
	for (unsigned int i = 0; i < STATE_WORDS; i++)
		_cipherInput[i] = IntUtils::BytesToLe64(_inputBuffer, i * 8);

	// process final message block
	_ubiParameters.SetIsFinalBlock(true);
	ProcessBlock(_bytesFilled);
	// clear cipher input
	std::fill(_cipherInput.begin(), _cipherInput.end(), 0);
	// do output block counter mode output 
	std::vector<byte> hash(STATE_OUTPUT, 0);
	std::vector<ulong> oldState(STATE_WORDS);

	// save old state
	for (unsigned int j = 0; j < _digestState.size(); j++)
		oldState[j] = _digestState[j];

	for (unsigned int i = 0; i < STATE_OUTPUT; i += STATE_BYTES)
	{
		_ubiParameters.StartNewBlockType((UbiType)Out);
		_ubiParameters.SetIsFinalBlock(true);
		ProcessBlock(8);

		// output a chunk of the hash
		unsigned int outputSize = STATE_OUTPUT - i;
		if (outputSize > STATE_BYTES)
			outputSize = STATE_BYTES;

		PutBytes(_digestState, hash, i, outputSize);

		// restore old state
		for (unsigned int j = 0; j < _digestState.size(); j++)
			_digestState[j] = oldState[j];

		// Increment counter
		_cipherInput[0]++;
	}

	memcpy(&Output[OutOffset], &hash[0], hash.size());
	return hash.size();
}

void Skein1024::GenerateConfiguration(std::vector<ulong> InitialState)
{
	Threefish1024 cipher;
	UbiTweak tweak;

	// initialize the tweak value
	tweak.StartNewBlockType((UbiType)Config);
	tweak.SetIsFinalBlock(true);
	tweak.SetBitsProcessed(32);

	cipher.SetKey(InitialState);
	cipher.SetTweak(tweak.GetTweak());
	cipher.Encrypt(_configString, _configValue);

	_configValue[0] ^= _configString[0];
	_configValue[1] ^= _configString[1];
	_configValue[2] ^= _configString[2];
}

void Skein1024::Initialize(SkeinInitializationType InitializationType)
{
	_initializationType = InitializationType;

	switch (InitializationType)
	{
	case SkeinInitializationType::Normal:
	{
		// normal initialization
		Initialize();
		return;
	}
	case SkeinInitializationType::ZeroedState:
	{
		// copy the configuration value to the state
		for (unsigned int i = 0; i < _digestState.size(); i++)
			_digestState[i] = 0;
		break;
	}
	case SkeinInitializationType::ChainedConfig:
	{
		// generate a chained configuration
		GenerateConfiguration(_digestState);
		// continue initialization
		Initialize();
		return;
	}
	case SkeinInitializationType::ChainedState:// keep the state as it is and do nothing
		break;
	}

	// reset bytes filled
	_bytesFilled = 0;
}

void Skein1024::Reset()
{
	Initialize();
}

void Skein1024::SetMaxTreeHeight(const byte Height)
{
	if (Height == 1)
		throw CryptoDigestException("Skein1024:SetMaxTreeHeight", "Tree height must be zero or greater than 1.");

	_configString[2] &= ~((ulong)0xff << 16);
	_configString[2] |= (ulong)Height << 16;
}

void Skein1024::SetSchema(const std::vector<byte> Schema)
{
	if (Schema.size() != 4)
		throw CryptoDigestException("Skein1024:SetSchema", "Schema must be 4 bytes.");

	ulong n = _configString[0];

	// clear the schema bytes
	n &= ~(ulong)0xfffffffful;
	// set schema bytes
	n |= (ulong)Schema[3] << 24;
	n |= (ulong)Schema[2] << 16;
	n |= (ulong)Schema[1] << 8;
	n |= (ulong)Schema[0];

	_configString[0] = n;
}

void Skein1024::SetTreeFanOutSize(const byte Size)
{
	_configString[2] &= ~((ulong)0xff << 8);
	_configString[2] |= (ulong)Size << 8;
}

void Skein1024::SetTreeLeafSize(const byte Size)
{
	_configString[2] &= ~(ulong)0xff;
	_configString[2] |= (ulong)Size;
}

void Skein1024::SetVersion(const unsigned int Version)
{
	if (Version < 0 || Version > 3)
		throw CryptoDigestException("Skein1024:SetVersion", "Version must be between 0 and 3, inclusive.");

	_configString[0] &= ~((ulong)0x03 << 32);
	_configString[0] |= (ulong)Version << 32;
}

void Skein1024::Update(byte Input)
{
	std::vector<byte> one(1, Input);
	BlockUpdate(one, 0, 1);
}

// *** Protected Methods *** //

void Skein1024::GenerateConfiguration()
{
	// default generation function>
	Threefish1024 cipher;
	UbiTweak tweak;

	// initialize the tweak value
	tweak.StartNewBlockType((UbiType)Config);
	tweak.SetIsFinalBlock(true);
	tweak.SetBitsProcessed(32);

	cipher.SetTweak(tweak.GetTweak());
	cipher.Encrypt(_configString, _configValue);

	_configValue[0] ^= _configString[0];
	_configValue[1] ^= _configString[1];
	_configValue[2] ^= _configString[2];
}

void Skein1024::Initialize()
{
	// copy the configuration value to the state
	for (unsigned int i = 0; i < _digestState.size(); i++)
		_digestState[i] = _configValue[i];

	// set up tweak for message block
	_ubiParameters.StartNewBlockType((UbiType)Message);
	// reset bytes filled
	_bytesFilled = 0;
}

void Skein1024::ProcessBlock(int bytes)
{
	// set the key to the current state
	_blockCipher.SetKey(_digestState);
	// update tweak
	long bits = _ubiParameters.GetBitsProcessed() + (long)bytes;
	_ubiParameters.SetBitsProcessed(bits);
	_blockCipher.SetTweak(_ubiParameters.GetTweak());
	// encrypt block
	_blockCipher.Encrypt(_cipherInput, _digestState);

	// feed-forward input with state
	for (unsigned int i = 0; i < _cipherInput.size(); i++)
		_digestState[i] ^= _cipherInput[i];
}

void Skein1024::PutBytes(std::vector<ulong> Input, std::vector<byte> &Output, unsigned int Offset, unsigned int ByteCount)
{
	unsigned int j = 0;
	for (unsigned int i = 0; i < ByteCount; i++)
	{
		Output[Offset + i] = (byte)((Input[i / 8] >> j) & 0xff);
		j = (j + 8) % 64;
	}
}

NAMESPACE_DIGESTEND