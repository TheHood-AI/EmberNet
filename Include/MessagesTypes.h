#pragma once
namespace EmberNet
{

#define CAST_TO_UCHAR(aEnum) static_cast<unsigned char>(aEnum)

	/// To define your own enums, use GetUserEnumStart() at the top of your own enums.
	/// \code
	///		enum class EUserEnum : unsigned char
	///		{
	///		FirstEnum = GetUserEnumStart(),
	///		SecondEnum,
	///		...
	///		};
	/// \endcode
	enum class ENetMessageType : unsigned char
	{
		NoMessage,
		ConnectMsg,
		ConnectionConfirm,
		DisconnectMsg,
		Ping,
		Pong,
		Guaranteed,
		Acknowledge,
		BigPackage,
		BigPackageChunk,

		UserEnumStart
	};

	constexpr unsigned char GetUserEnumStart()
	{
		return CAST_TO_UCHAR(ENetMessageType::UserEnumStart);
	}

	
	
}
