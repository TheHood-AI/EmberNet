# EmberNet
A network library for games written in c++

# How to use
Everything in the library uses `namespace EmberNet`<br/>
Start by including "PeerWrapper.hpp". In there is a class called IPeer. That's the main interface for all the networking features<br/>
First, call `bool EmberNet::StartWSA`. If it returns true, you can start setting up the peer by calling either `bool IPeer::Startup(const SocketDescriptor&, const CreateParameters&)` or `bool IPeer::Startup(const CreateParameters&)`<br/> 
SocketDescriptor is used for when you want to set the peer on a specific port, like a server and it looks like this:
```
struct SocketDescriptor
{
  unsigned short myPort = 0;
  std::string myIPToRecieveFrom = "0.0.0.0"
};
```
And CreateParameters are a struct that holds some information for the library about how to handle pinging and it looks like this:
```
struct CreateParameters
{
	unsigned short myMillisecondsBetweenPing = 1000;
	unsigned char myPingTriesBeforeDisconnect = 255;
};
```
The library will handle the pinging for you and disconnect another peer if they haven't answered a ping in some time, but CreateParameters lets you decide how often it should ping and how many pings can be sent before a disconnection happens. The functions will return true if they succeded with starting up.<br/><br/>

If you're starting a client and want to connect to a server, you can call `IPeer::Connect(std::string, unsigned short)`. It takes a IP address and a port to connect to, and will send a handshake to that IP and recieves a message with the message ID `ENetMessageType::ConnectionConfirm`

And finally, you need to call `bool IPeer::WorkThroughmessage()` in some kind of Update function. What the function do is that it take the first message in it's internal queue of recevied messages and does work on any message that the library wants to know about, and then sends it to any other function that wants to know about that message. It returns true when there is still messages in the queue, so if you want to empty the buffer you can call it inside of a while loop like this: `while(myPeer.WorkThroughMessage());`

If you want to know when a specific message gets recieved, you can call `IPeer::MapFunctionToMessage(std::function<void(char*, unsigned int)>, unsigned char)`. The first argument is a function that is to be called when a message with the message type of the second argument gets received. To use it you can do with either a lambda, member or static function as long as it have a char* as first argument and unsigned int as second argument: 
```
  myPeer.MapFunctionToMessage(BIND(Server::Connection, this), CAST_TO_UCHAR(EmberNet::ENetMessageType::ConnectMsg));
  myPeer.MapFuncitonToMessage([](char* someData, unsigned int aSize){printf("Disconnection")}, CAST_TO_UCHAR(EmberNet::ENetMessageType::DisconnectMessage))
  myPeer.MapFunctionToMessage(&GlobalFoo, CAST_TO_UCHAR(EmberNet::ENetMessageType::ConnectionConfoirm));
```
To create your own messages, all you need to do is create a class that is derived from the CNetMessage class in "NetMessage.hpp"
You also need to create a enum, which will be what determines what what your message type will be. Set the first enum type to the value you get from `EmberNet::GetUserEnumStart()`, which will give you the end of the libraries own message types cause otherwise your own messages could clash with the libraries messages. 
```
#include "Serializer.h"
enum class ECustomMessageType : char
{
  CreateObject = EmberNet::GetUserEnumStart(),
  SetObjectPosition,
  ...
};

class CreateObjectMessage : public EmberNet::CNetMessage
{
   CreateObjectMessage(myMessageType = CAST_TO_UCHAR(ECustomMessageType::CreateObject);)
   void Serialize(std::vector<char>& someData) const override
   {
      CNetMessage::Serialize(someData);
      
      EmberNet::SerializeType(myPos.x, someData);
      EmberNet::SerializeType(myPos.y, someData);
   };
	 void DeSerialize(std::vector<char>& someData) override
   {
      EmberNet::DeSerializeType(myPos.y, someData);
      EmberNet::DeSerializeType(myPos.x, someData);
      
      CNetMessage::DeSerialize(someData);
   };
   
   Vec2 myPos;
}
```
It's important that you call `CNetMessage::Serialise` in the beginning of your serialize and `CNetMessage::DeSerialize` and the end of your own DeSerialize, otherwise the data won't be correct. And as you can also see is that if you use `EmberNet::DeSerializeType` you will have to call it in reverse order from your serialize since in there I go through the vector backwards.

Finally, to send a message all you need to do is to send `IPeer::Send(char*, unsigned int, EmberNet::SendFlags)` It takes in a char array of data, a size of how much data to send and also a Flag for if you want the message to be Guaranteed, sent as a big package or normally (NoFlag which is default). 
```
  CreateObjectMessage msg;
  msg.myPos = {50, 100};
  std::vector<char> data = msg.Serialize();
  myPeer.Send(data.data(), data.size());
```
And when receiving:
```
  void CreateObject(char* someDataFromNet, unsigned int aSize)
  {
      CreateObjectMessage msg;
      msg.DeSerialize(someDataFromNet);
      printf("X: %d Y: %d", msg.myPos.x, msg.myPos.y);
      //Do more work here...
  }
```

With this, you should be able to easily create a networked environment for your game
