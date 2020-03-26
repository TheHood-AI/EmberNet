#pragma once
#include <assert.h>

namespace Tga2D
{
	template<class TYPE>
	struct SSimplePointerArray
	{
		SSimplePointerArray() :myData(nullptr), myNextCount(-1), mySize(0){}
		~SSimplePointerArray()
		{
			delete[] myData;
		}


		void Init(int aSize)
		{
			myNextCount = -1;
			mySize = aSize;	
			myData = new TYPE[aSize];
		}

		TYPE GetAt(int index)
		{
			assert(index < mySize);
			return myData[index];
		}
		TYPE GetCurrent()
		{
			assert(myNextCount < mySize);
			return myData[myNextCount];
		}
		TYPE *GetNext()
		{
			assert(myNextCount+1 < mySize);

			myNextCount++;
			return &myData[myNextCount];
		}

		void ResetCount()
		{
			myNextCount = -1;
		}

		int NextCount(){ return myNextCount; }
		int TotalCount(){ return mySize; }

		TYPE *myData;
		int mySize;

	private:
		int myNextCount;
	};
}