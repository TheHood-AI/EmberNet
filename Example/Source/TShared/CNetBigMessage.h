#pragma once
#include "NetMessage.h"

class CNetBigPackageChunk
	: public EmberNet::CNetMessage
{
public:
	CNetBigPackageChunk() { myMessageType = CAST_TO_UCHAR(ENetMessageType::BigPackageChunk); }
	
	void Serialize(std::vector<char>& someData) const override;
	void DeSerialize(std::vector<char>& someData) override;
	[[nodiscard]] bool IsGuaranteed() const override { return true; }

	std::string myDataChunk;
	mutable unsigned short myDataChunkSize;
	unsigned int myChunkID;
	unsigned int myBigDataID;
};

class CNetBigPackage
	: public EmberNet::CNetMessage
{
public:
	CNetBigPackage() { myMessageType = CAST_TO_UCHAR(ENetMessageType::BigPackage); };
	~CNetBigPackage() = default;

	void SetBigMessage(char* data, unsigned int aSize);
	
	void Serialize(std::vector<char>& someData) const override;
	void DeSerialize(std::vector<char>& someData) override;
	
	void AssignBigPackageUID();
	unsigned GetBigBackageUID() const;
	
	std::vector<CNetBigPackageChunk> myChunks;
	unsigned int myBigPackageUID;
	unsigned int myAmmountOfChunks;
	static unsigned int ourBigPackageUIDCounter;
};
