#include "pch.h"
#include "Peer.h"

#include <cassert>

#include "Socket.h"
#include <iostream>
#include "NetMessage.h"
#include "CommonNetworkIncludes.h"
#include "Packet.h"
#include <functional>
#include <thread>
#include <utility>

#include "ConnectionMessage.h"
#include "DisconnectMessage.h"
#include "Serializer.h"
#include "PeerWrapper.h"
#include "HelperMacros.h"
#include <algorithm>

namespace EmberNet
{
	unsigned int globalGUID = 0;

	CPeer::~CPeer()
	{
		CDisConnectMessage msg;
		msg.myUID = myGUID;
		auto data = SerializeMessage(msg);
		for (auto& addr : myConnectedAddreses)
		{
			Send(mySocket.GetSocket(), data.data(), static_cast<int>(data.size()), 0, addr.myAddress);

		}
		myConnectedAddreses.clear();
		myShouldShutDown = true;
	}

	bool CPeer::Startup(const SocketDescriptor& aSocketDescriptor, const CreateParameters& someCreateParameters)
	{
		if (!mySocket.SocketSetup(aSocketDescriptor.myPort, aSocketDescriptor.myIPToRecieveFrom))
			return false;


		myTimeBetweenPings = someCreateParameters.myMillisecondsBetweenPing;
		myTriesBeforeDisconnecting = someCreateParameters.myPingTriesBeforeDisconnect;

		std::thread listenThread(std::bind(&CPeer::ListeningThread, this));
		std::thread pingThread(std::bind(&CPeer::PingThread, this));
		std::thread guaranteedThread(std::bind(&CPeer::GuaranteedMessagesThread, this));
		std::thread updateThread(std::bind(&CPeer::UpdateThread, this));

		updateThread.detach();
		listenThread.detach();
		pingThread.detach();
		guaranteedThread.detach();
		return true;
	}

	bool CPeer::Startup(const CreateParameters& someCreateParameters)
	{
		if (!mySocket.SocketSetup())
			return false;

		myTimeBetweenPings = someCreateParameters.myMillisecondsBetweenPing;
		myTriesBeforeDisconnecting = someCreateParameters.myPingTriesBeforeDisconnect;


		std::thread listenThread(std::bind(&CPeer::ListeningThread, this));
		std::thread pingThread(std::bind(&CPeer::PingThread, this));
		std::thread guaranteedThread(std::bind(&CPeer::GuaranteedMessagesThread, this));
		std::thread updateThread(std::bind(&CPeer::GetNetworkDiagnostics, this));

		updateThread.detach();
		listenThread.detach();
		pingThread.detach();
		guaranteedThread.detach();
		return true;
	}

	void CPeer::Connect(const std::string& aRemoteAddres, const unsigned short aPort)
	{
		SOCKADDR_IN addresToConnectTo;
		addresToConnectTo.sin_family = AF_INET;
		addresToConnectTo.sin_port = htons(aPort);
		addresToConnectTo.sin_addr.S_un.S_addr = inet_addr(aRemoteAddres.c_str());

		const CConnectMessage msg;

		std::vector<char> data;
		msg.Serialize(data);


		Send(mySocket.GetSocket(), data.data(), static_cast<unsigned>(data.size()), 0, *reinterpret_cast<SOCKADDR*>(&addresToConnectTo));
	}

	bool CPeer::WorkThroughMessages()
	{
		Packet p;
		if (myRecievedPackets.try_pop(p))
		{
			switch (p.data[0])
			{
			case CAST_TO_UCHAR(ENetMessageType::ConnectMsg):
			{
				char ch[5];
				ch[0] = CAST_TO_UCHAR(ENetMessageType::ConnectionConfirm);
				std::vector<char> data;

				const unsigned int id = ++globalGUID;

				SerializeType(id, data);

				memcpy(&ch[1], data.data(), 4);

				Send(mySocket.GetSocket(), ch, 5, 0, p.sender);

				PeerInfo& inf = myConnectedAddreses.emplace_back();
				inf.myAddress = p.sender;
				inf.myUID = id;

				break;
			}
			case CAST_TO_UCHAR(ENetMessageType::ConnectionConfirm):
			{
				std::vector<char> data;
				data.push_back(p.data[1]);
				data.push_back(p.data[2]);
				data.push_back(p.data[3]);
				data.push_back(p.data[4]);

				DeSerializeType(myGUID, data);
				PeerInfo& inf = myConnectedAddreses.emplace_back();
				inf.myAddress = p.sender;

				break;
			}
			case CAST_TO_UCHAR(ENetMessageType::DisconnectMsg):
			{
				CDisConnectMessage msg;
				DeSerializeMessage(msg, p.data, p.size);

				for (int i = static_cast<int>(myConnectedAddreses.size()) - 1; i >= 0; --i)
				{
					if (myConnectedAddreses[i].myUID == msg.myUID)
					{
						CYCLIC_ERASE(myConnectedAddreses, i);
						break;
					}
				}
				break;
			}
			case CAST_TO_UCHAR(ENetMessageType::Ping):
			{
				char msg[6];
				msg[0] = CAST_TO_UCHAR(ENetMessageType::Pong);
				msg[1] = CAST_TO_UCHAR(EPriority::High);
				std::vector<char> data;

				SerializeType(myGUID, data);

				memcpy(&msg[2], data.data(), 4);

				Send(mySocket.GetSocket(), msg, 6, 0, p.sender);

				break;
			}
			case CAST_TO_UCHAR(ENetMessageType::Pong):
			{
				std::vector<char> data;
				data.push_back(p.data[2]);
				data.push_back(p.data[3]);
				data.push_back(p.data[4]);
				data.push_back(p.data[5]);

				unsigned int id = 0;
				DeSerializeType(id, data);
				for (auto& addr : myConnectedAddreses)
				{
					if (addr.myUID == id)
					{
						addr.myPingTries = 0;
						addr.myStopWatch.Stop();

						myLatency = addr.myStopWatch.GetDeltaMilliSeconds();
						break;
					}
				}


				break;
			}
			case CAST_TO_UCHAR(ENetMessageType::Acknowledge):
			{
				std::vector<char> data;
				data.push_back(p.data[1]);
				data.push_back(p.data[2]);
				unsigned short uid = 0;
				DeSerializeType(uid, data);

				myMutex.lock();
				for (size_t i = static_cast<int>(myGuaranteedMessages.size()); i > 0; --i)
				{
					if (myGuaranteedMessages[i - 1].myMessage.GetUID() == uid)
					{
						CYCLIC_ERASE(myGuaranteedMessages, i - 1);
						break;
					}
				}
				myMutex.unlock();
				break;
			}
			case CAST_TO_UCHAR(ENetMessageType::Guaranteed):
			{
				CGuaranteedMessage msg;
				DeSerializeMessage(msg, p.data, p.size);

				const auto uid = msg.GetUID();
				std::vector<char> data;
				data.push_back(CAST_TO_UCHAR(ENetMessageType::Acknowledge));
				SerializeType(uid, data);
				Send(mySocket.GetSocket(), data.data(), static_cast<int>(data.size()), 0, p.sender);
				if (myRecievedGuaranteedMesssages.find(std::make_pair(uid, (unsigned)msg.mySenderID)) == myRecievedGuaranteedMesssages.end())
				{
					Packet packet;
					memcpy(packet.data, msg.myData.data(), msg.mySizeOfData);
					packet.size = msg.mySizeOfData;
					packet.sender = p.sender;
					myRecievedPackets.push(packet);
					myRecievedGuaranteedMesssages.insert(std::make_pair(uid, (unsigned)msg.mySenderID));
				}
				break;
			}
			case CAST_TO_UCHAR(ENetMessageType::BigPackage):
			{
				CNetBigPackage bigMessage;
				DeSerializeMessage(bigMessage, p.data, p.size);
				myBigBackages[bigMessage.GetBigBackageUID()].myChunks.resize(bigMessage.myAmmountOfChunks);
				break;
			}
			case CAST_TO_UCHAR(ENetMessageType::BigPackageChunk):
			{
				CNetBigPackageChunk chunk;
				DeSerializeMessage(chunk, p.data, p.size);

				if (myFinishedBigPackages.find(chunk.myBigDataID) == myFinishedBigPackages.end() && myBigBackages.find(chunk.myBigDataID) != myBigBackages.end())
				{
					BigPacket& bigPacket = myBigBackages[chunk.myBigDataID];

					bigPacket.myChunks[chunk.myChunkID] = chunk;
					bigPacket.myRecievedChunk.insert(bigPacket.myRecievedChunk.begin(), chunk.myChunkID);

					if (bigPacket.myRecievedChunk.size() == bigPacket.myChunks.size())
					{
						myFinishedBigPackages.insert(myFinishedBigPackages.begin(), chunk.myBigDataID);

						std::string data;
						for (auto& c : bigPacket.myChunks)
						{
							data += c.myDataChunk;
						}
						if (myMappedFunctions.find(bigPacket.myChunks[0].myDataChunk[0]) != myMappedFunctions.end())
						{
							myMappedFunctions[bigPacket.myChunks[0].myDataChunk[0]](data.data(), static_cast<int>(data.size()));
						};
						myBigBackages.erase(chunk.myBigDataID);
					}
				}
				break;
			}
			default:
				break;
			}

			if (myMappedFunctions.find(p.data[0]) != myMappedFunctions.end())
			{
				myMappedFunctions[p.data[0]](p.data, p.size);
			};
		}
		return !myRecievedPackets.empty();
	}

	void CPeer::Shutdown()
	{
		myShouldShutDown = true;
	}

	unsigned CPeer::GetUID() const
	{
		return myGUID;
	}

	void CPeer::Send(const char* someDataToSend, const unsigned aDataSize, SendFlags aFlag)
	{
		assert(aDataSize < 512 || aFlag == SendFlags::BigPackage && "You're about to send more data than is safe in udp. Please set aFlag = BigPackage");
		for (auto& addr : myConnectedAddreses)
		{
			switch (aFlag)
			{
			case SendFlags::BigPackage:
			{
				SendBigPackage(someDataToSend, aDataSize, addr.myAddress);
				break;
			}
			case SendFlags::Guaranteed:
				SendGuaranteedMessage(someDataToSend, aDataSize, addr.myAddress);
				break;
			case SendFlags::NoFlag:
			default:
				Send(mySocket.GetSocket(), someDataToSend, aDataSize, 0, addr.myAddress);
				break;
			}
		}
	}

	void CPeer::SendTo(const char* someDataToSend, const unsigned aDataSize, SendFlags aFlag, int aPeerID)
	{
		assert(aDataSize < 512 || aFlag == SendFlags::BigPackage && "You're about to send more data than is safe in udp. Please set aFlag = BigPackage");
		for (auto& addr : myConnectedAddreses)
		{
			if (addr.myUID == static_cast<unsigned>(aPeerID))
			{
				switch (aFlag)
				{
				case SendFlags::BigPackage:
				{
					SendBigPackage(someDataToSend, aDataSize, addr.myAddress);
					break;
				}
				case SendFlags::Guaranteed:
					SendGuaranteedMessage(someDataToSend, aDataSize, addr.myAddress);
					break;
				case SendFlags::NoFlag:
				default:
					Send(mySocket.GetSocket(), someDataToSend, aDataSize, 0, addr.myAddress);
					break;
				}
			}
		}
	}

	void CPeer::SendToAllExcept(const char* someDataToSend, const unsigned aDataSize, SendFlags aFlag,
		int aPeerIDToIgnore)
	{
		assert(aDataSize < 512 || aFlag == SendFlags::BigPackage && "You're about to send more data than is safe in udp. Please set aFlag = BigPackage");
		for (auto& addr : myConnectedAddreses)
		{
			if (addr.myUID == static_cast<unsigned>(aPeerIDToIgnore))
				continue;

			switch (aFlag)
			{
			case SendFlags::BigPackage:
			{
				SendBigPackage(someDataToSend, aDataSize, addr.myAddress);
				break;
			}
			case SendFlags::Guaranteed:
				SendGuaranteedMessage(someDataToSend, aDataSize, addr.myAddress);
				break;
			case SendFlags::NoFlag:
			default:
				Send(mySocket.GetSocket(), someDataToSend, aDataSize, 0, addr.myAddress);
				break;
			}
		}
	}

	void CPeer::MapFunctionToMessage(std::function<void(char*, unsigned int)> aFunction, unsigned char aMessage)
	{
		myMappedFunctions[aMessage] = std::move(aFunction);
	}

	NetworkDiagnostics CPeer::GetNetworkDiagnostics() const
	{
		NetworkDiagnostics diag;
		diag.mySentbytesLastSecond = myByteSentLastSecond;
		diag.myRecievedBytesLastSecond = myByteRecievedLastSecond;
		diag.myPackageDrop = static_cast<short>(static_cast<float>(myDroppedMessages) / static_cast<float>(myGuaranteedMessagesSent) * 100.f);

		for (auto connection : myConnectedAddreses)
		{
			NetworkDiagnostics::LatencyData& lat = diag.myLatencyInMilliSeconds.emplace_back();
			lat.myMilliSeconds = connection.myStopWatch.GetDeltaMilliSeconds();
			lat.myPeerID = connection.myUID;
		}

		return diag;
	}

	void CPeer::ListeningThread()
	{
		while (!myShouldShutDown)
		{
			char recieveBuffer[512];
			SOCKADDR sender;
			int senderSize = sizeof(sender);

			const int msgSize = recvfrom(mySocket.GetSocket(), recieveBuffer, 512, 0, &sender, &senderSize);

			if (msgSize > 0)
			{
				myByteRecievedThisSecond += msgSize;
				Packet retPacket;
				memcpy(retPacket.data, recieveBuffer, msgSize);
				retPacket.size = msgSize;
				retPacket.sender = sender;
				retPacket.myPriority = static_cast<EPriority>(recieveBuffer[1]);
				myRecievedPackets.push(retPacket);
			}
		}
	}

	void CPeer::PingThread()
	{
		while (!myShouldShutDown)
		{
			char msg[2];
			msg[0] = CAST_TO_UCHAR(ENetMessageType::Ping);
			msg[1] = CAST_TO_UCHAR(EPriority::High);

			for (size_t i = myConnectedAddreses.size(); i > 0; --i)
			{
				PeerInfo& peer = myConnectedAddreses[i - 1];
				if (peer.myPingTries > myTriesBeforeDisconnecting)
				{
					CDisConnectMessage disMsg;
					disMsg.myUID = peer.myUID;

					auto data = SerializeMessage(disMsg);
					Send(data.data(), static_cast<int>(data.size()), SendFlags::NoFlag);


					Packet retPacket;
					memcpy(retPacket.data, data.data(), data.size());
					retPacket.size = static_cast<int>(data.size());
					myRecievedPackets.push(retPacket);

					continue;
				}
				sendto(mySocket.GetSocket(), msg, 2, 0, &peer.myAddress, sizeof(peer.myAddress));
				++peer.myPingTries;

				if (peer.myPingTries == 1)
				{
					peer.myStopWatch.Reset();
				}
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(myTimeBetweenPings));
		}
	}

	void CPeer::UpdateThread()
	{
		while (!myShouldShutDown)
		{
			myByteSentLastSecond = myByteSentThisSecond;
			myByteSentThisSecond = 0;

			myByteRecievedLastSecond = myByteRecievedThisSecond;
			myByteRecievedThisSecond = 0;

			myDroppedMessages = 0;
			myGuaranteedMessagesSent = 0;

			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}

	void CPeer::GuaranteedMessagesThread()
	{
		while (!myShouldShutDown)
		{
			std::this_thread::sleep_for(std::chrono::seconds(1));
			myMutex.lock();
			int i = 0;
			for (auto& message : myGuaranteedMessages)
			{
				++i;
				myDroppedMessages++;
				myGuaranteedMessagesSent++;
				message.myResendTries++;
				auto data = SerializeMessage(message.myMessage);
				Send(mySocket.GetSocket(), data.data(), static_cast<unsigned>(data.size()), 0, message.mySendTo);
			}
			myMutex.unlock();
		}
	}

	void CPeer::SendGuaranteedMessage(const char* someData, const unsigned int aDataSize, const SOCKADDR& aAddres)
	{
		++myGuaranteedMessagesSent;
		CGuaranteedMessage message;
		message.SetData(someData, aDataSize);
		message.mySenderID = static_cast<unsigned short>(myGUID);
		message.AssignUID();

		auto data = SerializeMessage(message);

		GuaranteedMessage msg;

		msg.myMessage = message;
		msg.mySendTo = aAddres;
		myGuaranteedMessages.push_back(msg);


		Send(mySocket.GetSocket(), data.data(), static_cast<int>(data.size()), 0, aAddres);
	}

	void CPeer::SendBigPackage(const char* someData, const unsigned aDataSize, const SOCKADDR& aAddres)
	{
		bool haveCopiedData = false;
		std::thread bigPackageThread([this, aAddres, aDataSize, someData, &haveCopiedData]()
		{
			char* buffer = new char[aDataSize];
			memcpy(buffer, someData, static_cast<size_t>(aDataSize));
			haveCopiedData = true;
			CNetBigPackage msg;
			msg.AssignBigPackageUID();
			msg.SetBigMessage(buffer, aDataSize);
			std::vector<char> data = SerializeMessage(msg);

			SendGuaranteedMessage(data.data(), static_cast<unsigned>(data.size()), aAddres);

			int i = 0;
			for (auto& chunk : msg.myChunks)
			{
				++i;
				std::vector<char> chunkData = SerializeMessage(chunk);
				SendGuaranteedMessage(chunkData.data(), static_cast<unsigned>(chunkData.size()), aAddres);
				if (i % 100 == 0)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(300));
				}
			}
			delete[] buffer;
			buffer = nullptr;
		});

		bigPackageThread.detach();
		while (!haveCopiedData)
		{
			std::this_thread::yield();
		}
	}

	void CPeer::Send(const SOCKET& aSocket, const char* someData, const unsigned int aDataSize, const int aFlag, const SOCKADDR& aAddres)
	{
		myByteSentThisSecond += aDataSize;
		sendto(aSocket, someData, static_cast<int>(aDataSize), aFlag, &aAddres, sizeof(aAddres));
	}
}
