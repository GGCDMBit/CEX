#ifndef _CEXENGINE_CRYPTOPROCESSINGEXCEPTION_H
#define _CEXENGINE_CRYPTOPROCESSINGEXCEPTION_H

#include "Common.h"

NAMESPACE_EXCEPTION

/// <summary>
/// Generalized cryptographic error container
/// </summary>
struct CryptoProcessingException : std::exception
{
private:
	std::string _origin;
	std::string _message;

public:
	/// <summary>
	/// The origin of the exception in the format Class:Method
	/// </summary>
	const std::string &Origin() const { return _origin; }
	std::string &Origin() { return _origin; }

	const std::string &Message() const { return _message; }
	std::string &Message() { return _message; }

	/// <summary>
	/// Exception constructor
	/// </summary>
	///
	/// <param name="Message">A custom message or error data</param>
	CryptoProcessingException(const std::string &Message) 
		: 
		_message(Message)
	{
	}

	/// <summary>
	/// Exception constructor
	/// </summary>
	///
	/// <param name="Origin">The origin of the exception</param>
	/// <param name="Message">A custom message or error data</param>
	CryptoProcessingException(const std::string &Origin, const std::string &Message) 
		: 
		_origin(Origin), 
		_message(Message)
	{
	}
};

NAMESPACE_EXCEPTIONEND
#endif