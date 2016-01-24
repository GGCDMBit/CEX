#ifndef _CEXTEST_SKEINTEST_H
#define _CEXTEST_SkeinTest_H

#include "ITest.h"
#include "Skein256.h"
#include "Skein512.h"
#include "Skein1024.h"

namespace Test
{
	using CEX::Digest::Skein256;
	using CEX::Digest::Skein512;
	using CEX::Digest::Skein1024;

	/// <summary>
	/// Tests the Skein digest implementation using vector comparisons.
	/// <para>Tests the 256, 512, and 1024 bit versions of Skein against known test vectors from the skein 1.3 document, appendix C:
    /// <see href="http://www.skein-hash.info/sites/default/files/skein1.3.pdf"/></para>
	/// </summary>
	class SkeinTest : public ITest
	{
	private:
		const std::string DESCRIPTION = "Tests the 256, 512, and 1024 bit versions of Skein.";
		const std::string FAILURE = "FAILURE! ";
		const std::string SUCCESS = "SUCCESS! All Skein tests have executed succesfully.";

		std::vector<std::vector<byte>> _expected256;
		std::vector<std::vector<byte>> _expected512;
		std::vector<std::vector<byte>> _expected1024;
		std::vector<std::vector<byte>> _message256;
		std::vector<std::vector<byte>> _message512;
		std::vector<std::vector<byte>> _message1024;
		TestEventHandler _progressEvent;

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
		/// Known answer tests for the 256, 512, and 1024 bit versions of Skein
		/// </summary>
		SkeinTest()
		{
			const char* message256Encoded[3] =
			{
				("FF"),
				("FFFEFDFCFBFAF9F8F7F6F5F4F3F2F1F0EFEEEDECEBEAE9E8E7E6E5E4E3E2E1E0"),
				("FFFEFDFCFBFAF9F8F7F6F5F4F3F2F1F0EFEEEDECEBEAE9E8E7E6E5E4E3E2E1E0DFDEDDDCDBDAD9D8D7D6D5D4D3D2D1D0CFCECDCCCBCAC9C8C7C6C5C4C3C2C1C0")
			};
			HexConverter::Decode(message256Encoded, 3, _message256);

			const char* message512Encoded[3] =
			{
				("FF"),
				("FFFEFDFCFBFAF9F8F7F6F5F4F3F2F1F0EFEEEDECEBEAE9E8E7E6E5E4E3E2E1E0DFDEDDDCDBDAD9D8D7D6D5D4D3D2D1D0CFCECDCCCBCAC9C8C7C6C5C4C3C2C1C0"),
				("FFFEFDFCFBFAF9F8F7F6F5F4F3F2F1F0EFEEEDECEBEAE9E8E7E6E5E4E3E2E1E0DFDEDDDCDBDAD9D8D7D6D5D4D3D2D1D0CFCECDCCCBCAC9C8C7C6C5C4C3C2C1C0BFBEBDBCBBBAB9B8B7B6B5B4B3B2B1B0AFAEADACABAAA9A8A7A6A5A4A3A2A1A09F9E9D9C9B9A999897969594939291908F8E8D8C8B8A89888786858483828180")
			};
			HexConverter::Decode(message512Encoded, 3, _message512);

			const char* message1024Encoded[3] =
			{
				("FF"),
				("FFFEFDFCFBFAF9F8F7F6F5F4F3F2F1F0EFEEEDECEBEAE9E8E7E6E5E4E3E2E1E0DFDEDDDCDBDAD9D8D7D6D5D4D3D2D1D0CFCECDCCCBCAC9C8C7C6C5C4C3C2C1C0BFBEBDBCBBBAB9B8B7B6B5B4B3B2B1B0AFAEADACABAAA9A8A7A6A5A4A3A2A1A09F9E9D9C9B9A999897969594939291908F8E8D8C8B8A89888786858483828180"),
				("FFFEFDFCFBFAF9F8F7F6F5F4F3F2F1F0EFEEEDECEBEAE9E8E7E6E5E4E3E2E1E0DFDEDDDCDBDAD9D8D7D6D5D4D3D2D1D0CFCECDCCCBCAC9C8C7C6C5C4C3C2C1C0BFBEBDBCBBBAB9B8B7B6B5B4B3B2B1B0AFAEADACABAAA9A8A7A6A5A4A3A2A1A09F9E9D9C9B9A999897969594939291908F8E8D8C8B8A898887868584838281807F7E7D7C7B7A797877767574737271706F6E6D6C6B6A696867666564636261605F5E5D5C5B5A595857565554535251504F4E4D4C4B4A494847464544434241403F3E3D3C3B3A393837363534333231302F2E2D2C2B2A292827262524232221201F1E1D1C1B1A191817161514131211100F0E0D0C0B0A09080706050403020100")
			};
			HexConverter::Decode(message1024Encoded, 3, _message1024);

			const char* expected256Encoded[3] =
			{
				("0B98DCD198EA0E50A7A244C444E25C23DA30C10FC9A1F270A6637F1F34E67ED2"),
				("8D0FA4EF777FD759DFD4044E6F6A5AC3C774AEC943DCFC07927B723B5DBF408B"),
				("DF28E916630D0B44C4A849DC9A02F07A07CB30F732318256B15D865AC4AE162F")
			};
			HexConverter::Decode(expected256Encoded, 3, _expected256);

			const char* expected512Encoded[3] =
			{
				("71B7BCE6FE6452227B9CED6014249E5BF9A9754C3AD618CCC4E0AAE16B316CC8CA698D864307ED3E80B6EF1570812AC5272DC409B5A012DF2A579102F340617A"),
				("45863BA3BE0C4DFC27E75D358496F4AC9A736A505D9313B42B2F5EADA79FC17F63861E947AFB1D056AA199575AD3F8C9A3CC1780B5E5FA4CAE050E989876625B"),
				("91CCA510C263C4DDD010530A33073309628631F308747E1BCBAA90E451CAB92E5188087AF4188773A332303E6667A7A210856F742139000071F48E8BA2A5ADB7")
			};
			HexConverter::Decode(expected512Encoded, 3, _expected512);

			const char* expected1024Encoded[3] =
			{
				("E62C05802EA0152407CDD8787FDA9E35703DE862A4FBC119CFF8590AFE79250BCCC8B3FAF1BD2422AB5C0D263FB2F8AFB3F796F048000381531B6F00D85161BC0FFF4BEF2486B1EBCD3773FABF50AD4AD5639AF9040E3F29C6C931301BF79832E9DA09857E831E82EF8B4691C235656515D437D2BDA33BCEC001C67FFDE15BA8"),
				("1F3E02C46FB80A3FCD2DFBBC7C173800B40C60C2354AF551189EBF433C3D85F9FF1803E6D920493179ED7AE7FCE69C3581A5A2F82D3E0C7A295574D0CD7D217C484D2F6313D59A7718EAD07D0729C24851D7E7D2491B902D489194E6B7D369DB0AB7AA106F0EE0A39A42EFC54F18D93776080985F907574F995EC6A37153A578"),
				("842A53C99C12B0CF80CF69491BE5E2F7515DE8733B6EA9422DFD676665B5FA42FFB3A9C48C217777950848CECDB48F640F81FB92BEF6F88F7A85C1F7CD1446C9161C0AFE8F25AE444F40D3680081C35AA43F640FD5FA3C3C030BCC06ABAC01D098BCC984EBD8322712921E00B1BA07D6D01F26907050255EF2C8E24F716C52A5")
			};
			HexConverter::Decode(expected1024Encoded, 3, _expected1024);
		}

		/// <summary>
		/// Destructor
		/// </summary>
		~SkeinTest()
		{
		}

		/// <summary>
		/// Start the tests
		/// </summary>
		virtual std::string Run()
		{
			try
			{
				Skein256* sk256 =  new Skein256();
				CompareVector(sk256, _message256[0], _expected256[0]);
				CompareVector(sk256, _message256[1], _expected256[1]);
				CompareVector(sk256, _message256[2], _expected256[2]);
				OnProgress("Passed Skein 256 bit digest vector tests..");
				delete sk256;

				Skein512* sk512 =  new Skein512();
				CompareVector(sk512, _message512[0], _expected512[0]);
				CompareVector(sk512, _message512[1], _expected512[1]);
				CompareVector(sk512, _message512[2], _expected512[2]);
				delete sk512;
				OnProgress("Passed Skein 512 bit digest vector tests..");

				Skein1024* sk1024 =  new Skein1024();
				CompareVector(sk1024, _message1024[0], _expected1024[0]);
				CompareVector(sk1024, _message1024[1], _expected1024[1]);
				CompareVector(sk1024, _message1024[2], _expected1024[2]);
				delete sk1024;
				OnProgress("Passed Skein 1024 bit digest vector tests..");

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
		void CompareVector(IDigest *Digest, std::vector<byte> Input, std::vector<byte> Expected)
		{
			std::vector<byte> hash(Digest->DigestSize(), 0);

			Digest->BlockUpdate(Input, 0, Input.size());
			Digest->DoFinal(hash, 0);
			// must call reset with skein if using DoFinal method!
			Digest->Reset();

			if (Expected != hash) 
				throw std::string("SKein Vector: Expected hash is not equal!");

			Digest->ComputeHash(Input, hash);
			if (Expected != hash)
				throw std::string("SKein Vector: Expected hash is not equal!");
		}

		void OnProgress(char* Data)
		{
			_progressEvent(Data);
		}
	};
}

#endif
