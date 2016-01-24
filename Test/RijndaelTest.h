﻿#ifndef _CEXTEST_RIJNDAELTEST_H
#define _CEXTEST_RIJNDAELTEST_H

#include "ITest.h"
#include "KeyParams.h"
#include "RHX.h"

namespace Test
{
	using CEX::Cipher::Symmetric::Block::RHX; 
	using CEX::Common::KeyParams;

    /// <summary>
	/// Rijndael implementation vector comparison tests.
    /// <para>est vectors derived from Bouncy Castle RijndaelTest.cs and the Nessie unverified vectors:
    /// <see href="https://www.cosic.esat.kuleuven.be/nessie/testvectors/bc/rijndael/Rijndael-256-256.unverified.test-vectors"/>
    /// Tests supported block sizes of 16 and 32 bytes.</para>
    /// </summary>
    class RijndaelTest : public ITest
    {
	private:
		const std::string DESCRIPTION = "Rijndael Known Answer Tests.";
		const std::string FAILURE = "FAILURE! ";
		const std::string SUCCESS = "SUCCESS! Rijndael tests have executed succesfully.";

		TestEventHandler _progressEvent;
        std::vector<std::vector<byte>> _cipherText;
        std::vector<std::vector<byte>> _keys;
        std::vector<std::vector<byte>> _plainText;

    public:
		/// <summary>
		/// Get: The test description
		/// </summary>
		virtual const std::string Description() { return DESCRIPTION; }

		/// <summary>
		/// Progress return event callback
		/// </summary>
		virtual TestEventHandler &Progress() { return _progressEvent; }

		/// <summary>
		/// Compares known answer Rijndael vectors for equality
		/// </summary>
		RijndaelTest()
        {
			const char* keysEncoded[15] =
			{
				("80000000000000000000000000000000"),
				("00000000000000000000000000000080"),
				("000000000000000000000000000000000000000000000000"),
				("0000000000000000000000000000000000000000000000000000000000000000"),
				("2b7e151628aed2a6abf7158809cf4f3c"),
				("2b7e151628aed2a6abf7158809cf4f3c762e7160f38b4da5"),
				("2b7e151628aed2a6abf7158809cf4f3c762e7160f38b4da56a784d9045190cfe"),
				("8000000000000000000000000000000000000000000000000000000000000000"),
				("4000000000000000000000000000000000000000000000000000000000000000"),
				("2000000000000000000000000000000000000000000000000000000000000000"),
				("1000000000000000000000000000000000000000000000000000000000000000"),
				("0000000000000000000000000000000000000000000000000000000000000000"),
				("0000000000000000000000000000000000000000000000000000000000000000"),
				("0000000000000000000000000000000000000000000000000000000000000000"),
				("0000000000000000000000000000000000000000000000000000000000000000")
            };
            HexConverter::Decode(keysEncoded, 15, _keys);

			const char* plainTextEncoded[15] = 
            {
                ("00000000000000000000000000000000"),
                ("00000000000000000000000000000000"),
                ("80000000000000000000000000000000"),
                ("80000000000000000000000000000000"),
                ("3243f6a8885a308d313198a2e03707344a4093822299f31d0082efa98ec4e6c8"),
                ("3243f6a8885a308d313198a2e03707344a4093822299f31d0082efa98ec4e6c8"),
                ("3243f6a8885a308d313198a2e03707344a4093822299f31d0082efa98ec4e6c8"),
                ("0000000000000000000000000000000000000000000000000000000000000000"),
                ("0000000000000000000000000000000000000000000000000000000000000000"),
                ("0000000000000000000000000000000000000000000000000000000000000000"),
                ("0000000000000000000000000000000000000000000000000000000000000000"),
                ("8000000000000000000000000000000000000000000000000000000000000000"),
                ("4000000000000000000000000000000000000000000000000000000000000000"),
                ("2000000000000000000000000000000000000000000000000000000000000000"),
                ("1000000000000000000000000000000000000000000000000000000000000000")
            };
            HexConverter::Decode(plainTextEncoded, 15, _plainText);

			const char* cipherTextEncoded[15] =
            {
                ("0EDD33D3C621E546455BD8BA1418BEC8"),
                ("172AEAB3D507678ECAF455C12587ADB7"),
                ("6CD02513E8D4DC986B4AFE087A60BD0C"),
                ("DDC6BF790C15760D8D9AEB6F9A75FD4E"),
                ("7d15479076b69a46ffb3b3beae97ad8313f622f67fedb487de9f06b9ed9c8f19"),
                ("5d7101727bb25781bf6715b0e6955282b9610e23a43c2eb062699f0ebf5887b2"),
                ("a49406115dfb30a40418aafa4869b7c6a886ff31602a7dd19c889dc64f7e4e7a"),
                ("E62ABCE069837B65309BE4EDA2C0E149FE56C07B7082D3287F592C4A4927A277"),
                ("1F00B4DD622C0B2951F25970B0ED47A65F513112DACA242B5292CA314917BF94"),
                ("2AA9F4BE159F9F8777561281C1CC4FCD7435E6E855E222426C309838ABD5FFEE"),
                ("B4ADF28C3A85C337AA3150E3032B941AA49F12F911221DD91A62919CAD447CFB"),
                ("159A08E46E616E6E9978502010DAFF922EB362E77DCAAF02EAEB7354EB8B8DBA"),
                ("2756DDECD7558B198962F092D7BA3EEF45D9E287380AAB8E852658092AA9DFA1"),
                ("87B829FB7B0C16C408151D323FCB8B56EBC0573747D46C2B47BFD533ED3273C9"),
                ("DB462EEC713D4CC89607DCA35C4FE6E8D618C8BDACD3DD1C0A1B14E6CA8C23C6")
            };
            HexConverter::Decode(cipherTextEncoded, 15, _cipherText);
        }

		/// <summary>
		/// Destructor
		/// </summary>
		~RijndaelTest()
		{
		}

		/// <summary>
		/// Start the tests
		/// </summary>
		virtual std::string Run()
        {
            try
            {
                for (unsigned int i = 0; i < _plainText.size(); i++)
                    CompareVector(_keys[i], _plainText[i], _cipherText[i]);

				OnProgress("RijndaelTest : Passed 128 and 256 bit block tests..");

				return SUCCESS;
			}
			catch (std::string const& ex)
			{
				throw TestException(std::string(FAILURE + " : " + ex));
			}
			catch (...)
			{
				throw TestException(std::string(FAILURE + " : Internal Error"));
			}
        }
        
    private:
        void CompareVector(std::vector<byte> &Key, std::vector<byte> &Input, std::vector<byte> &Output)
        {
            std::vector<byte> outBytes(Input.size(),0);
            std::vector<byte> outBytes2(Input.size(),0);
            RHX engine(Input.size());
			KeyParams k(Key);

            engine.Initialize(true, k);
            engine.Transform(Input, outBytes);

            if (outBytes != Output)
                throw std::string("RijndaelTest: Encrypted arrays are not equal!");

            engine.Initialize(false, k);
            engine.Transform(Output, outBytes);

            if (outBytes != Input)
                throw std::string("RijndaelTest: Decrypted arrays are not equal!");
        }

		void OnProgress(char* Data)
		{
			_progressEvent(Data);
		}
    };
}

#endif

