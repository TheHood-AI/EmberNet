#pragma once
#include "CommonNetworkIncludes.h"
#include "NetMessage.h"
#include <mutex>
#include "gsl/gsl_util"
#include "Socket.h"
#include <functional>

class CNetworkPostMaster;
class CNetworkHelpers
{
public:
	void Update(const float aDelta);
	static CNetworkHelpers* GetInstance();

	[[nodiscard]] unsigned int GetSentBytes() const { return mySentBytes; };
	void ResetSentBytes() { mySentBytes = 0; }

	[[nodiscard]] unsigned int GetRecievedBytes()const { return myRecievedBytes; };
	void ResetRecievedBytes() { myRecievedBytes = 0; };
	
	
	void ConvertMessageAndDoWork(const char* aBufferconst, const int aByteRecieved, CNetworkPostMaster* aEventManager);

	void Acknowledge(const unsigned short aUID);

	template<typename MessageType>
	void SendMsg(MessageType& aMessage, const CSocket& aSenderSocket, const SOCKADDR_IN& aRecieverAddres);

	int RecieveMsg(const CSocket& aSocket, char* aRecieveBuffer, SOCKADDR_IN& aSenderAddr);
	
	template<typename T>
	static void SerializeType(const T& aVariable, std::vector<char>& someData);

	template<typename T>
	static void DeSerializeType(T& aVariable, std::vector<char>& someData);

	template<typename T>
	static T DeSerializeType(std::vector<char>& someData);

	//template<typename MessageClass>
	//static MessageClass DeSerializeMessage(const char* aMessage, const int aSize, CNetworkPostMaster* aEventManager);


	inline static void SerializeBaseType(const char* aString, std::vector<char>& someData, const unsigned int aSize);
	inline static void DeSerializeBaseType(char* aString, std::vector<char>& someData, const unsigned int aSize);


	static bool WSASetup();

	struct SGuaranteedMessage
	{
		std::function<void()> myCallback;
		float myTimeoutTimer;
		unsigned short myResendTries = 0;
		unsigned short myMessageUID = 0;
	};

private:
	void SendMsg(const char* aMsg, const int aMessageSize, const CSocket& aSenderSocket, const SOCKADDR_IN& aRecieverAddres);
	
	static CNetworkHelpers* ourInstance;
	std::vector<SGuaranteedMessage> myGuaranteedMessages;
	std::mutex myMutex;
	unsigned int mySentBytes = 0;
	unsigned int myRecievedBytes = 0;
};

template <typename T>
void CNetworkHelpers::SerializeType(const T& aVariable, std::vector<char>& someData)
{
	static_assert(!std::is_polymorphic<T>::value, "T can't have a v-table");

	constexpr int size = sizeof(T);

	char perfType[size];
	memcpy(perfType, &aVariable, size);

	for (int i = 0; i < size; ++i)
	{
		someData.push_back(perfType[i]);
	}
}

template <typename T>
void CNetworkHelpers::DeSerializeType(T& aVariable, std::vector<char>& someData)
{
	static_assert(!std::is_polymorphic<T>::value, "T can't have a v-table");

	constexpr int size = sizeof(T);

	char perfType[size];

	for (int i = size - 1; i >= 0; --i)
	{
		perfType[i] = someData.back(); someData.pop_back();
	}

	memcpy(&aVariable, perfType, size);
}

template <typename T>
T CNetworkHelpers::DeSerializeType(std::vector<char>& someData)
{
	T ret;
	DeSerializeType<T>(ret, someData);
	return ret;
}

//template<typename MessageClass>
//inline MessageClass CNetworkHelpers::DeSerializeMessage(const char* aMessage, const int aSize, CNetworkPostMaster* aEventManager)
//{
//	/*static_assert(std::is_base_of<CNetMessage, MessageClass>::value, "Template class needs to derive from CNetMessage!!");
//	MessageClass msg;
//
//	std::vector<char> vecBuf;
//	vecBuf.reserve(512);
//	for (int i = 0; i < aSize; ++i)
//	{
//		vecBuf.push_back(aMessage[i]);
//	}
//
//	msg.DeSerialize(vecBuf);
//
//	if (msg.IsGuaranteed())
//	{
//		SNetworkEvent event;
//		event.myType = ENetworkEventType::SendAcknowledge;
//		event.u_myAcknowledgeData.myID = msg.GetUID();
//		aEventManager->ScheduleEvent(event);
//	}
//
//	return msg;*/
//}

inline void CNetworkHelpers::SerializeBaseType(const char* aString, std::vector<char>& someData, const unsigned int aSize)
{
	for (unsigned int i = 0; i < aSize; ++i)
	{
		someData.push_back(aString[i]);
	}
}

inline void CNetworkHelpers::DeSerializeBaseType(char* aString, std::vector<char>& someData, const unsigned int aSize)
{
	for (int i = gsl::narrow<int>(aSize) - 1; i >= 0; --i)
	{
		aString[i] = someData.back(); someData.pop_back();
	}
}

template<typename MessageType>
inline void CNetworkHelpers::SendMsg(MessageType& aMessage, const CSocket& aSenderSocket, const SOCKADDR_IN& aRecieverAddres)
{
	/*static_assert(std::is_base_of<CNetMessage, MessageType>::value, "Template class needs to derive from CNetMessage!!");

	if (aMessage.IsGuaranteed())
	{
		myMutex.lock();
		aMessage.AssignUID();
		SGuaranteedMessage guaranteed;
		guaranteed.myMessageUID = aMessage.GetUID();
		guaranteed.myTimeoutTimer = 1.0f;
		guaranteed.myCallback = [this, aMessage, &aSenderSocket, aRecieverAddres]()
		{
			std::vector<char> msg;
			aMessage.Serialize(msg);

			SendMsg(msg.data(), msg.size(), aSenderSocket, aRecieverAddres);
		};
		myGuaranteedMessages.push_back(guaranteed);
		myMutex.unlock();
	}

	std::vector<char> msg;
	aMessage.Serialize(msg);

	SendMsg(msg.data(), msg.size(), aSenderSocket, aRecieverAddres);*/
}
