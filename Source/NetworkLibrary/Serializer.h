#pragma once
#include <vector>
#include "ExportDefines.h"
#include "NetMessage.h"

namespace EmberNet
{
	EXPORT void SerializeString(const char* aString, std::vector<char>& someData, const unsigned int aSize);
	
	EXPORT void DeSerializeString(char* aString, std::vector<char>& someData, const unsigned int aSize);

	EXPORT std::vector<char> SerializeMessage(EmberNet::CNetMessage& aNetMessage);
	
	EXPORT void DeSerializeMessage(CNetMessage& aNetMessage, const char* someData, const unsigned aMessageSize);
	
	template <typename T>
	void SerializeType(const T& aVariable, std::vector<char>& someData)
	{
		static_assert(!std::is_polymorphic<T>::value, "T can't have a v-table");

		constexpr int size = sizeof(T);

		char perfType[size];
		memcpy(perfType, &aVariable, size);

		for (int i = 0; i < size; ++i)
		{
			someData.push_back(perfType[i]);
		}
	}

	template <typename T>
	void DeSerializeType(T& aVariable, std::vector<char>& someData)
	{
		static_assert(!std::is_polymorphic<T>::value, "T can't have a v-table");

		constexpr int size = sizeof(T);

		char perfType[size];

		for (int i = size - 1; i >= 0; --i)
		{
			perfType[i] = someData.back(); someData.pop_back();
		}

		memcpy(&aVariable, perfType, size);
	}
}
