#include "pch.h"
#include "NetBigMessage.h"
#include "Serializer.h"
#include <cmath>

namespace EmberNet
{

	unsigned int CNetBigPackage::ourBigPackageUIDCounter = 0;

	void CNetBigPackageChunk::Serialize(std::vector<char>& someData) const
	{
		CNetMessage::Serialize(someData);

		SerializeType(myBigDataID, someData);
		SerializeType(myChunkID, someData);


		for (const char& ch : myDataChunk)
		{
			someData.push_back(ch);
		}
		myDataChunkSize = static_cast<unsigned short>(myDataChunk.size());
		SerializeType(myDataChunkSize, someData);
	}

	void CNetBigPackageChunk::DeSerialize(std::vector<char>& someData)
	{
		DeSerializeType<unsigned short>(myDataChunkSize, someData);

		for (int i = 0; i < myDataChunkSize; ++i)
		{
			myDataChunk.insert(myDataChunk.begin(), someData.back()); someData.pop_back();
		}

		DeSerializeType<unsigned>(myChunkID , someData);
		DeSerializeType<unsigned>(myBigDataID ,someData);

		CNetMessage::DeSerialize(someData);
	}


	//-------------------------------------------------------------------

	void CNetBigPackage::SetBigMessage(const char* data, unsigned int aSize)
	{
		constexpr float dataPerChunk = 480;

		myAmmountOfChunks = std::ceil(static_cast<float>(aSize) / dataPerChunk);

		myChunks.reserve(myAmmountOfChunks);

		int dataIndex = 0;

		for (unsigned int i = 0; i < myAmmountOfChunks; ++i)
		{
			auto& chunk = myChunks.emplace_back();
			for (int j = 0; j < dataPerChunk; ++j)
			{
				if (dataIndex < aSize)
					chunk.myDataChunk.push_back(data[dataIndex++]);
			}
			chunk.myChunkID = i;
			chunk.myBigDataID = myBigPackageUID;
		}
	}

	void CNetBigPackage::Serialize(std::vector<char>& someData) const
	{
		CNetMessage::Serialize(someData);

		SerializeType(myBigPackageUID, someData);
		SerializeType(myAmmountOfChunks, someData);
	}

	void CNetBigPackage::DeSerialize(std::vector<char>& someData)
	{
		DeSerializeType(myAmmountOfChunks, someData);
		DeSerializeType(myBigPackageUID, someData);

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
}
