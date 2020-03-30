#include "pch.h"
#include "NetMessage.h"
#include "Serializer.h"

namespace EmberNet
{
	static unsigned short globalNetMessageUIDCounter = 0;

	CNetMessage::CNetMessage()
		= default;

	CNetMessage::~CNetMessage()
		= default;

	void CNetMessage::Serialize(std::vector<char>& someData) const
	{
		SerializeType(myMessageType, someData);
		SerializeType(myPriority, someData);
		SerializeType(myUID, someData);
	}

	void CNetMessage::DeSerialize(std::vector<char>& someData)
	{
		DeSerializeType(myUID, someData);
		DeSerializeType(myPriority, someData);
		DeSerializeType(myMessageType, someData);
	}

	unsigned short CNetMessage::GetUID() const
	{
		return myUID;
	}


	CGuaranteedMessage::CGuaranteedMessage()
	{
		myMessageType = CAST_TO_UCHAR(ENetMessageType::Guaranteed);
	}

	void CGuaranteedMessage::Serialize(std::vector<char>& someData) const
	{
		CNetMessage::Serialize(someData);

		for (unsigned int i = 0; i < mySizeOfData; ++i)
		{
			someData.push_back(myData[i]);
		}

		SerializeType(mySizeOfData, someData);
	}

	void CGuaranteedMessage::DeSerialize(std::vector<char>& someData)
	{
		DeSerializeType(mySizeOfData, someData);

		myData.resize(mySizeOfData);
		for (int i = mySizeOfData; i > 0; --i)
		{
			myData[i - 1] = someData.back(); someData.pop_back();
		}

		CNetMessage::DeSerialize(someData);
	}

	void CGuaranteedMessage::SetData(const char* someData, unsigned int aSize)
	{
		mySizeOfData = aSize;
		myData.resize(mySizeOfData);
		for(unsigned int i = 0; i < aSize; ++i)
		{
			myData[i] = someData[i];
		}
	}

	void CNetMessage::AssignUID()
	{
		myUID = globalNetMessageUIDCounter++;
	}
}