// ConsoleServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include "../NetworkLibrary/CommonNetworkIncludes.h"
#include "../NetworkLibrary/MessagesTypes.h"
#include "../NetworkLibrary/NetMessage.h"
#include "../NetworkLibrary/Packet.h"
#include "../NetworkLibrary/Serializer.h"
#include "../NetworkLibrary/PeerWrapper.h"
#include "../NetworkLibrary/SocketDescriptor.h"

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
	Server();
	void Connection(const char* someData, unsigned short aSize);

	void Start();
	void Update();

private:
	EmberNet::IPeer myPeer;
};

Server::Server()
{
}

void Server::Connection(const char* someData, unsigned short aSize)
{
	printf("SERVER: Connection!");
}

void Server::Start()
{
	EmberNet::SocketDescriptor desc;
	desc.myPort = 5150;
	desc.myIPToRecieveFrom = "0.0.0.0";
	myPeer.Startup(desc, {});
	myPeer.MapFunctionToMessage([](const char* someData, unsigned short aSize)
	{
		CNetTextMessage msg;
		EmberNet::DeSerializeMessage(msg, someData, aSize);
		printf("Message: %s\n", msg.GetMsg().c_str());

	}, CAST_TO_UCHAR(MyMessageType::TextMessage));

	
	myPeer.MapFunctionToMessage(BIND(Server::Connection, this), CAST_TO_UCHAR(EmberNet::ENetMessageType::ConnectMsg));

		printf("SERVE: Start\n");

}

void Server::Update()
{
	myPeer.WorkThroughMessages();

}



int main()
{
	EmberNet::StartWSA();

	Server server;
	server.Start();
	Sleep(100);
	while (true)
	{
		server.Update();

		Sleep(100);
	}

	std::cout << "Hello World!\n";
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
