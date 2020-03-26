#include "pch.h"
#include "Serializer.h"

void EmberNet::SerializeString(const char* aString, std::vector<char>& someData, const unsigned aSize)
{
	for (unsigned int i = 0; i < aSize; ++i)
	{
		someData.push_back(aString[i]);
	}
}

void EmberNet::DeSerializeString(char* aString, std::vector<char>& someData, const unsigned aSize)
{
	for (int i = aSize - 1; i >= 0; --i)
	{
		aString[i] = someData.back(); someData.pop_back();
	}
}

std::vector<char>  EmberNet::SerializeMessage(CNetMessage& aNetMessage)
{
	std::vector<char> data;
	aNetMessage.Serialize(data);
	return data;
}

void EmberNet::DeSerializeMessage(CNetMessage& aNetMessage, const char* someData, const unsigned aMessageSize)
{
	std::vector<char> data;
	data.resize(aMessageSize);
	for (int i = aMessageSize; i > 0; --i)
	{
		data[i-1] = someData[i-1];
	}
	aNetMessage.DeSerialize(data);
}
