#pragma once
#include <vector>
#include <assert.h>

namespace Tga2D
{
	template <class Type, class CountType = int>
	class TinyGrowingArray
	{
	public:
		TinyGrowingArray(unsigned int aMaxSize = 1024);
		~TinyGrowingArray();

		TinyGrowingArray& operator=(const TinyGrowingArray& aGrowingArray);

		void Init(int aNrOfRecomendedItems, bool aShouldGrowFlag = true);
		void ReInit(int aNrOfRecomendedItems, bool aShouldGrowFlag = true);

		inline Type& operator[](const CountType& aIndex) const;

		inline void Add(const Type& aObject);
		inline void push_back(const Type& aObject) { Add(aObject); }
		inline void Remove(CountType aItemNumber);

		inline void RemoveAll();
		inline void clear() { RemoveAll(); }

		_forceinline bool IsFull() const;
		__forceinline CountType Count() const;
		__forceinline CountType size() const { return Count(); }
		inline void ReSize(CountType aNewSize, int currentSize);

		typedef Type* iterator;
		typedef const Type* const_iterator;
		iterator begin() { return &myItemList[0]; }
		const_iterator begin() const { return &myItemList[0]; }
		iterator end() { return &myItemList[myCurNrOfItems]; }
		const_iterator end() const { return &myItemList[myCurNrOfItems]; }
		iterator erase(iterator aIter)
		{
			int found = myCurNrOfItems;
			for (int i = 0; i < myCurNrOfItems; i++)
			{
				if (myItemList[i] == *aIter)
				{
					myItemList[i] = myItemList[myCurNrOfItems-1];
					found = i;
					myCurNrOfItems--;
					break;
				}
			}

			return &myItemList[found];
		}
	protected:
		__declspec(align(16))  Type* myItemList;
		CountType myCurNrOfItems;
		CountType myMaxNrOfItems;
		bool myShouldGrowFlag;
	};

	template <class Type, class CountType>
	TinyGrowingArray<Type, CountType>::TinyGrowingArray(unsigned int aMaxSize)
	{
		myItemList = nullptr;
		myCurNrOfItems = 0;
		myMaxNrOfItems = 0;
		Init(aMaxSize, true);
	}

	
	template <class Type, class CountType>
	TinyGrowingArray<Type, CountType>& TinyGrowingArray<Type, CountType>::operator=(const TinyGrowingArray<Type, CountType>& aGrowingArray)
	{
		ReInit(aGrowingArray.myMaxNrOfItems, aGrowingArray.myShouldGrowFlag);

		for (CountType i = 0; i < aGrowingArray.myCurNrOfItems; i++)
		{
			myItemList[i] = aGrowingArray[i];
		}
		myShouldGrowFlag = aGrowingArray.myShouldGrowFlag;
		myCurNrOfItems = aGrowingArray.Count();
		return(*this);
	}

	template <class Type, class CountType>
	TinyGrowingArray<Type, CountType>::~TinyGrowingArray()
	{
		if (myItemList != nullptr) delete[](myItemList);
		myItemList = nullptr;
	}

	template <class Type, class CountType>
	void TinyGrowingArray<Type, CountType>::Init(int aNrOfRecomendedItems, bool aShouldGrowFlag)
	{
		assert((myMaxNrOfItems == 0 || myItemList == nullptr) && "CALLED INIT ON AN ALREADY INITED GROWINGARRAY PLEASE USE REINIT");

		myShouldGrowFlag = aShouldGrowFlag;
		myMaxNrOfItems = aNrOfRecomendedItems;
		myCurNrOfItems = 0;

		myItemList = new Type[myMaxNrOfItems];
	}

	template <class Type, class CountType>
	void TinyGrowingArray<Type, CountType>::ReInit(int aNrOfRecomendedItems, bool aShouldGrowFlag)
	{
		if (myMaxNrOfItems > 0)
		{
			delete[](myItemList);
		}

		myShouldGrowFlag = aShouldGrowFlag;
		myMaxNrOfItems = aNrOfRecomendedItems;
		myCurNrOfItems = 0;

		myItemList = new Type[myMaxNrOfItems];
	}



	template <class Type, class CountType>
	Type& TinyGrowingArray<Type, CountType>::operator[](const CountType& aIndex) const
	{
		assert((aIndex >= 0) && (aIndex < myCurNrOfItems) && "OUT OF BOUNDS ERROR");
		return(myItemList[aIndex]);
	}


	template <class Type, class CountType>
	void TinyGrowingArray<Type, CountType>::Add(const Type& aItem)
	{
		assert(myItemList != nullptr && "USE INIT BEFORE USE!");

		if (myCurNrOfItems == myMaxNrOfItems) ReSize(myMaxNrOfItems * 2, myCurNrOfItems);
		myItemList[myCurNrOfItems] = aItem;
		myCurNrOfItems++;
	}
	template <class Type, class CountType>
	void TinyGrowingArray<Type, CountType>::Remove(CountType aItemNumber)
	{
		for (CountType i = aItemNumber; i < myCurNrOfItems; i++)
		{
			myItemList[i] = myItemList[i + 1];
		}
		myCurNrOfItems--;
	}

	template <class Type, class CountType>
	void TinyGrowingArray<Type, CountType>::RemoveAll()
	{
		myCurNrOfItems = 0;
	}

	template<class Type, class CountType>
	inline bool TinyGrowingArray<Type, CountType>::IsFull() const
	{
		return (myCurNrOfItems >= myMaxNrOfItems);
	}

	template <class Type, class CountType>
	CountType TinyGrowingArray<Type, CountType>::Count() const
	{
		return(myCurNrOfItems);
	}


	template <class Type, class CountType>
	void TinyGrowingArray<Type, CountType>::ReSize(CountType aNewSize, int currentSize)
	{
		assert((myShouldGrowFlag || aNewSize < myMaxNrOfItems) && "this array can't grow!");
		assert((aNewSize != currentSize || aNewSize < myMaxNrOfItems) && "This list can't grow!");
		Type* tempList = new Type[aNewSize];

		for (CountType i = 0; i < myCurNrOfItems && i < aNewSize; i++)
		{
			tempList[i] = myItemList[i];
		}
		if (myItemList)
		{
			delete[](myItemList);
		}
		myMaxNrOfItems = aNewSize;
		myCurNrOfItems = currentSize;
		myItemList = tempList;
	}
}