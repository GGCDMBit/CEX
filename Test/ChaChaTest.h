#ifndef _CEXTEST_CHACHATEST_H
#define _CEXTEST_CHACHATEST_H

#include "ITest.h"

namespace Test
{
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
		virtual std::string Run();

	private:
		void CompareParallel();
		void CompareVector(int Rounds, std::vector<byte> &Key, std::vector<byte> &Vector, std::vector<byte> &Input, std::vector<byte> &Output);
		void Initialize();
		void OnProgress(char* Data);
	};
}

#endif