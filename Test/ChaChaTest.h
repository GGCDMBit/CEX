#ifndef _CEXTEST_CHACHATEST_H
#define _CEXTEST_CHACHATEST_H

#include "ITest.h"
#include <algorithm>
#include "ChaCha.h"
#include "CSPRsg.h"
#include "KeyParams.h"

namespace Test
{
	using CEX::Cipher::Symmetric::Stream::ChaCha; 
	using CEX::Common::KeyParams;
	using CEX::Seed::CSPRsg;

	/// <summary>
	/// ChaCha implementation vector comparison tests.
	/// <para>Using the BouncyCastle vectors:
    /// <see href="http://grepcode.com/file/repo1.maven.org/maven2/org.bouncycastle/bcprov-ext-jdk15on/1.51/org/bouncycastle/crypto/test/ChaChaTest.java"/></para>
	/// </summary>
	class ChaChaTest : public ITest
	{
	private:
		const std::string DESCRIPTION = "ChaCha Known Answer Tests.";
		const std::string FAILURE = "FAILURE! ";
		const std::string SUCCESS = "SUCCESS! ChaCha tests have executed succesfully.";

		TestEventHandler _progressEvent;
		std::vector<byte> _plainText;
		std::vector<std::vector<byte>> _key;
		std::vector<std::vector<byte>> _iv;
		std::vector<std::vector<byte>> _cipherText;

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
		/// Compares known answer ChaCha vectors for equality
		/// </summary>
		ChaChaTest()
		{
			HexConverter::Decode("00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000", _plainText);

			const char* keyEncoded[4] =
			{
				("80000000000000000000000000000000"),//20-1
				("00400000000000000000000000000000"),//20-2
				("0053A6F94C9FF24598EB3E91E4378ADD3083D6297CCF2275C81B6EC11467BA0D"),
				("0558ABFE51A4F74A9DF04396E93C8FE23588DB2E81D4277ACD2073C6196CBF12")
			};
			HexConverter::Decode(keyEncoded, 4, _key);

			const char* ivEncoded[3] =
			{
				("0000000000000000"),
				("0D74DB42A91077DE"),
				("167DE44BB21980E7")
			};
			HexConverter::Decode(ivEncoded, 3, _iv);

			const char* cipherTextEncoded[6] =
			{
				("FBB87FBB8395E05DAA3B1D683C422046F913985C2AD9B23CFC06C1D8D04FF213D44A7A7CDB84929F915420A8A3DC58BF0F7ECB4B1F167BB1A5E6153FDAF4493D"), //20r-1
				("A276339F99316A913885A0A4BE870F0691E72B00F1B3F2239F714FE81E88E00CBBE52B4EBBE1EA15894E29658C4CB145E6F89EE4ABB045A78514482CE75AFB7C"), //20r-2
				("36CF0D56E9F7FBF287BC5460D95FBA94AA6CBF17D74E7C784DDCF7E0E882DDAE3B5A58243EF32B79A04575A8E2C2B73DC64A52AA15B9F88305A8F0CA0B5A1A25"), //12r
				("BEB1E81E0F747E43EE51922B3E87FB38D0163907B4ED49336032AB78B67C24579FE28F751BD3703E51D876C017FAA43589E63593E03355A7D57B2366F30047C5"), //8r
				("57459975BC46799394788DE80B928387862985A269B9E8E77801DE9D874B3F51AC4610B9F9BEE8CF8CACD8B5AD0BF17D3DDF23FD7424887EB3F81405BD498CC3"), //20r-256k
				("92A2508E2C4084567195F2A1005E552B4874EC0504A9CD5E4DAF739AB553D2E783D79C5BA11E0653BEBB5C116651302E8D381CB728CA627B0B246E83942A2B99")  //20r-256k
			};
			HexConverter::Decode(cipherTextEncoded, 6, _cipherText);
		}

		/// <summary>
		/// Destructor
		/// </summary>
		~ChaChaTest()
		{
		}

		/// <summary>
		/// Start the tests
		/// </summary>
		virtual std::string Run()
		{
			try
			{
				// test vectors with 8/12/20 rounds and 128/256 keys
				CompareVector(20, _key[0], _iv[0], _plainText, _cipherText[0]);
				CompareVector(20, _key[1], _iv[0], _plainText, _cipherText[1]);
				OnProgress("ChaChaTest: Passed 20 round vector tests..");
				CompareVector(12, _key[0], _iv[0], _plainText, _cipherText[2]);
				CompareVector(8, _key[0], _iv[0], _plainText, _cipherText[3]);
				OnProgress("ChaChaTest: Passed 8 and 12 round vector tests..");
				CompareVector(20, _key[2], _iv[1], _plainText, _cipherText[4]);
				CompareVector(20, _key[3], _iv[2], _plainText, _cipherText[5]);
				OnProgress("ChaChaTest: Passed 256 bit key vector tests..");
				CompareParallel();
				OnProgress("ChaChaTest: Passed parallel/linear equality tests..");

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
		void CompareParallel()
		{
			CSPRsg rng;
			std::vector<byte> key(32);
			rng.GetBytes(key);
			std::vector<byte> iv(8);
			rng.GetBytes(iv);
			std::vector<byte> data(2048);
			rng.GetBytes(data);
			std::vector<byte> enc(2048, 0);
			std::vector<byte> dec(2048, 0);
			KeyParams k(key, iv);
			ChaCha cipher(20);

			// encrypt linear
			cipher.Initialize(k);
			cipher.IsParallel() = false;
			cipher.Transform(data, enc);
			// decrypt parallel
			cipher.Initialize(k);
			cipher.IsParallel() = true;
			cipher.ParallelBlockSize() = 2048;
			cipher.Transform(enc, dec);

			if (data != dec)
				throw std::string("ChaCha: Decrypted arrays are not equal!");
		}

		void CompareVector(int Rounds, std::vector<byte> &Key, std::vector<byte> &Vector, std::vector<byte> &Input, std::vector<byte> &Output)
		{
			std::vector<byte> outBytes(Input.size(), 0);
			KeyParams k(Key, Vector);
			ChaCha cipher(Rounds);

			cipher.Initialize(k);
			cipher.Transform(Input, 0, outBytes, 0, Input.size());

			if (outBytes != Output)
				throw std::string("ChaCha: Encrypted arrays are not equal!");

			cipher.Initialize(k);
			cipher.Transform(Output, 0, outBytes, 0, Output.size());

			if (outBytes != Input)
				throw std::string("ChaCha: Decrypted arrays are not equal!");
		}

		void OnProgress(char* Data)
		{
			_progressEvent(Data);
		}
	};
}

#endif
