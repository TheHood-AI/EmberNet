#include "pch.h"
#include "CNetBigMessage.h"

unsigned int CNetBigPackage::ourBigPackageUIDCounter = 0;

void CNetBigPackageChunk::Serialize(std::vector<char>& someData) const
{
	CNetMessage::Serialize(someData);
	
	CNetworkHelpers::SerializeType(myBigDataID, someData);
	CNetworkHelpers::SerializeType(myChunkID, someData);

	
	for(const char& ch : myDataChunk)
	{
		someData.push_back(ch);
	}
	myDataChunkSize = gsl::narrow<unsigned short>(myDataChunk.size());
	CNetworkHelpers::SerializeType(myDataChunkSize, someData);
}

void CNetBigPackageChunk::DeSerialize(std::vector<char>& someData)
{
	myDataChunkSize = CNetworkHelpers::DeSerializeType<unsigned short>(someData);
	
	for(int i = 0; i < myDataChunkSize; ++i)
	{
		myDataChunk.insert(myDataChunk.begin(), someData.back()); someData.pop_back();
	}

	myChunkID = CNetworkHelpers::DeSerializeType<unsigned>(someData);
	myBigDataID = CNetworkHelpers::DeSerializeType<unsigned>(someData);

	CNetMessage::DeSerialize(someData);
}


//-------------------------------------------------------------------

void CNetBigPackage::SetBigMessage(char* data, unsigned int aSize)
{
	constexpr float dataPerChunk = 400;
	
	myAmmountOfChunks = (aSize + 1) / dataPerChunk;

	myChunks.reserve(myAmmountOfChunks);

	int dataIndex = 0;
	
	for(int i = 0; i < myAmmountOfChunks; ++i)
	{
		auto& chunk = myChunks.emplace_back();
		for(int j = 0; j < dataPerChunk; ++j)
		{
			if(dataIndex < aSize)
				chunk.myDataChunk.push_back(data[dataIndex++]);
		}
		chunk.myChunkID = i;
		chunk.myBigDataID = myBigPackageUID;
	}
}

void CNetBigPackage::Serialize(std::vector<char>& someData) const
{
	CNetMessage::Serialize(someData);

	CNetworkHelpers::SerializeType(myBigPackageUID, someData);
	CNetworkHelpers::SerializeType(myAmmountOfChunks, someData);
}

void CNetBigPackage::DeSerialize(std::vector<char>& someData)
{
	CNetworkHelpers::DeSerializeType(myAmmountOfChunks, someData);
	CNetworkHelpers::DeSerializeType(myBigPackageUID, someData);
	
	CNetMessage::DeSerialize(someData);
}

void CNetBigPackage::AssignBigPackageUID()
{
	myBigPackageUID = ourBigPackageUIDCounter++;
}

unsigned CNetBigPackage::GetBigBackageUID() const
{
	return myBigPackageUID;
}
