#ifndef CEX_CRYPTOSYMMETRICCIPHEREXCEPTION_H
#define CEX_CRYPTOSYMMETRICCIPHEREXCEPTION_H

#include "CexDomain.h"

NAMESPACE_EXCEPTION

/// <summary>
/// Wraps exceptions thrown within a Symmetric cipher operational context
/// </summary>
struct CryptoSymmetricCipherException : std::exception
{
private:
	std::string m_details;
	std::string m_message;
	std::string m_origin;

public:

	/// <summary>
	/// Get/Set: The inner exception string
	/// </summary>
	std::string &Details();

	/// <summary>
	/// Get/Set: The message associated with the error
	/// </summary>
	std::string &Message();

	/// <summary>
	/// Get/Set: The origin of the exception in the format Class
	/// </summary>
	std::string &Origin();

	/// <summary>
	/// Instantiate this class with a message
	/// </summary>
	///
	/// <param name="Message">A custom message or error data</param>
	explicit CryptoSymmetricCipherException(const std::string &Message);

	/// <summary>
	/// Instantiate this class with an origin and message
	/// </summary>
	///
	/// <param name="Origin">The origin of the exception</param>
	/// <param name="Message">A custom message or error data</param>
	explicit CryptoSymmetricCipherException(const std::string &Origin, const std::string &Message);

	/// <summary>
	/// Instantiate this class with an origin, message and inner exception
	/// </summary>
	///
	/// <param name="Origin">The origin of the exception</param>
	/// <param name="Message">A custom message or error data</param>
	/// <param name="Detail">The inner exception string</param>
	explicit CryptoSymmetricCipherException(const std::string &Origin, const std::string &Message, const std::string &Detail);
};

NAMESPACE_EXCEPTIONEND
#endif