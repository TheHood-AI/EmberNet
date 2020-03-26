// EmberNetChatExample.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "../../Include/PeerWrapper.h"
#include "../../Include/Serializer.h"
#include <array>
#include <random>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 5150

int RandomInt(int aMin, int aMax)
{
	static std::random_device rndDevice;
	static std::mt19937 mt(rndDevice());

	std::uniform_int_distribution<int> rndDist(aMin, aMax);

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


class Server
{
public:
	void Connection(const char* someData, unsigned short aSize);

	void Start();
	void Update();
private:
	EmberNet::IPeer myPeer;
};

void Server::Connection(const char* someData, unsigned short aSize)
{
	printf("SERVER: Received connection!\n");
}

void Server::Start()
{
	EmberNet::SocketDescriptor desc;
	desc.myPort = 5150;
	desc.myIPToRecieveFrom = LISTEN_TO_ALL_IP;
	if (!myPeer.Startup(desc, {}))
	{
		printf("SERVER: Failed to start peer!\n");
	}
	else
	{
		myPeer.MapFunctionToMessage([](const char* someData, unsigned short aSize)
		{
			CNetTextMessage msg;
			EmberNet::DeSerializeMessage(msg, someData, aSize);
			printf("SERvER: Got Message: %s\n", msg.GetMsg().c_str());

		}, CAST_TO_UCHAR(MyMessageType::TextMessage));

		myPeer.MapFunctionToMessage(BIND(Server::Connection, this), CAST_TO_UCHAR(EmberNet::ENetMessageType::ConnectMsg));

		printf("SERVE: Start\n");
	}
}

void Server::Update()
{
	while(myPeer.WorkThroughMessages());
}


class Client
{
public:
	void Start() const;
	void Update();
	void Send(const char* someData, const int aDataSize);

private:
	EmberNet::IPeer myPeer;
};

void Client::Start() const
{
	EmberNet::CreateParameters param;
	param.myMillisecondsBetweenPing = 2000;
	param.myPingTriesBeforeDisconnect = 10;
	if (!myPeer.Startup(param))
	{
		printf("CLIENT: Failed to start peer!\n");
	}
	else
	{
		myPeer.Connect(SERVER_IP, SERVER_PORT);
	}
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
	Server server;
	server.Start();

	Client client;
	client.Start();


	int iterations = 0;
	while (true)
	{
		client.Update();
		server.Update();

		CNetTextMessage textMessage;
		textMessage.SetMessage(greetings[RandomInt(0, 4)]);

		printf("CLIENT: Sending message: %s\n", textMessage.GetMsg().data());
		std::vector<char> data = EmberNet::SerializeMessage(textMessage);
		client.Send(data.data(), data.size());

		if (iterations > 50)
		{
			break;
		}
		++iterations;

		Sleep(500);
	}


}
