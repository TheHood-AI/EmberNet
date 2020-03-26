#pragma once
#include "NetMessage.h"
#include <string>

namespace EmberNet
{

	class CNetBigPackageChunk
		: public EmberNet::CNetMessage
	{
	public:
		CNetBigPackageChunk() { myMessageType = CAST_TO_UCHAR(ENetMessageType::BigPackageChunk); }

		void Serialize(std::vector<char>& someData) const override;
		void DeSerialize(std::vector<char>& someData) override;
		[[nodiscard]] bool IsGuaranteed() const override { return true; }

		std::string myDataChunk;
		mutable unsigned short myDataChunkSize = 0;
		unsigned int myChunkID = 0;
		unsigned int myBigDataID = 0;
	};

	class CNetBigPackage
		: public CNetMessage
	{
	public:
		CNetBigPackage() { myMessageType = CAST_TO_UCHAR(ENetMessageType::BigPackage); };
		~CNetBigPackage() = default;

		void SetBigMessage(const char* data, unsigned int aSize);

		void Serialize(std::vector<char>& someData) const override;
		void DeSerialize(std::vector<char>& someData) override;

		void AssignBigPackageUID();
		unsigned GetBigBackageUID() const;

		std::vector<CNetBigPackageChunk> myChunks;
		unsigned int myBigPackageUID = 0;
		unsigned int myAmmountOfChunks = 0;
		static unsigned int ourBigPackageUIDCounter;
	};
}
