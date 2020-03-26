// TestingGround.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "../NetworkLibrary/CommonNetworkIncludes.h"
#include "../NetworkLibrary/Peer.h"
#include "../NetworkLibrary/Packet.h"
#include "../NetworkLibrary/NetMessage.h"
#include "../NetworkLibrary/MessagesTypes.h"
#include "../NetworkLibrary/Serializer.h"
#include "array"
#include <memory>
#include <iostream>
#include <random>

#include "../NetworkLibrary/PeerWrapper.h"

int RandomInt(int aMin, int aMax)
{
	static std::random_device rndDevice;
	static std::mt19937 mt(rndDevice());

	std::uniform_int_distribution <int> rndDist(aMin, aMax);

	return rndDist(mt);
}

enum class MyMessageType
{
	TextMessage = EmberNet::GetUserEnumStart()
};

class CNetTextMessage : public EmberNet::CNetMessage
{
public:
	CNetTextMessage();
	void SetMessage(const std::string& aString);
	std::string GetMsg();
	void Serialize(std::vector<char>& someData) const override;
	void DeSerialize(std::vector<char>& someData) override;
private:
	char myMessage[256];
};

CNetTextMessage::CNetTextMessage()
	:myMessage()
{
	myMessageType = CAST_TO_UCHAR(MyMessageType::TextMessage);
	ZeroMemory(myMessage, 256);
}

void CNetTextMessage::SetMessage(const std::string& aString)
{
	ZeroMemory(myMessage, 256);
	int index = 0;
	for (char ch : aString)
	{
		myMessage[index] = ch;
		index++;
	}
	myMessage[aString.size()] = '\0';
}

std::string CNetTextMessage::GetMsg()
{
	return std::string(myMessage);
}

void CNetTextMessage::Serialize(std::vector<char>& someData)const
{
	CNetMessage::Serialize(someData);


	EmberNet::SerializeString(myMessage, someData, 256);
}

void CNetTextMessage::DeSerialize(std::vector<char>& someData)
{
	EmberNet::DeSerializeString(myMessage, someData, 256);

	CNetMessage::DeSerialize(someData);
}



constexpr unsigned short serverPort = 5150;
#define SERVER_IP "127.0.0.1"

class Client
{
public:
	Client();

	void Start();
	void Connect();
	void Update();
	void Send(const char* someData, const int aDataSize);

private:
	EmberNet::IPeer myPeer;
};

Client::Client()
{
}

void Client::Start()
{
	EmberNet::CreateParameters c;
	c.myMillisecondsBetweenPing = 2000;
	myPeer.Startup(c);
	printf("CLIENT: Start\n");
}

void Client::Connect()
{
	myPeer.Connect(SERVER_IP, serverPort);
	printf("sent connectionn\n");
}

void Client::Update()
{
	myPeer.WorkThroughMessages();
}
void Client::Send(const char* someData, const int aDataSize)
{
	myPeer.Send(someData, aDataSize);
}

std::array<std::string, 5> greetings = { "Hey", "Hello", "Sup", "What's up", "Yooo" };


int main()
{
	EmberNet::StartWSA();

	Sleep(1000);
	Client client;
	client.Start();
	client.Connect();

	int i = 0;
	while (true)
	{
		client.Update();

		CNetTextMessage textMessage;
		textMessage.SetMessage(greetings[RandomInt(0,4)]);
		
		std::vector<char> data = EmberNet::SerializeMessage(textMessage);
		client.Send(data.data(), data.size());

		if(i > 50)
		{
			break;
		}
		++i;
		Sleep(100);
	}

}

