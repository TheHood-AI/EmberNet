#pragma once
#include <vector>
#include "MessagesTypes.h"
#include "ExportDefines.h"
#include <memory>

namespace EmberNet
{
	class CNetMessage;
	
	enum class EPriority : char
	{
		Low,
		Medium,
		High
	};

	class EXPORT CNetMessage
	{
	public:
		CNetMessage();
		virtual ~CNetMessage();

		virtual void Serialize(std::vector<char>& someData) const = 0;
		virtual void DeSerialize(std::vector<char>& someData) = 0;
		virtual bool IsGuaranteed() const { return false; }

		void AssignUID();
		[[nodiscard]] unsigned short GetUID()const;
		unsigned short mySenderID = 0;
	protected:
		unsigned char myMessageType = CAST_TO_UCHAR(ENetMessageType::NoMessage);
		unsigned char myPriority = CAST_TO_UCHAR(EPriority::Low);
		unsigned short myUID = 0;
	};


	class CGuaranteedMessage : public CNetMessage
	{
	public:
		CGuaranteedMessage();
		
		void Serialize(std::vector<char>& someData) const override;
		void DeSerialize(std::vector<char>& someData) override;
		void SetData(const char* someData, unsigned short aSize);
	
		std::vector<char> myData;
		unsigned short mySizeOfData = 0;
	};
}