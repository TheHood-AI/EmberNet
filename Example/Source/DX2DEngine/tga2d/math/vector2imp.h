#pragma once


namespace Tga2D
{
	template <class Type>
	class Vector2
	{
	public:
		Vector2() { x = 0; y = 0; }
		Vector2(Type aX, Type aY) { x = aX; y = aY; }
		Vector2(const Vector2<Type>& aVector) { x = aVector.x; y = aVector.y; }

		template <class OtherType>
		Vector2<Type>& operator=(const Vector2<OtherType>& aVector) { x = (Type)aVector.x; y = (Type)aVector.y; return *this; }

		void Set(Type aX, Type aY) { x = aX; y = aY; }

		Vector2<Type>& operator+=(const Vector2<Type>& aVector) { x += aVector.x; y += aVector.y; return *this; }
		Vector2<Type>& operator-=(const Vector2<Type>& aVector) { x -= aVector.x; y -= aVector.y; return *this; }
		Vector2<Type>& operator*=(const Vector2<Type>& aVector) { x *= aVector.x; y *= aVector.y; return *this; }
		Vector2<Type>& operator/=(const Vector2<Type>& aVector) { x /= aVector.x; y /= aVector.y; return *this; }

		Vector2<Type>& operator*=(Type aValue) { x *= aValue; y *= aValue; return *this; }
		Vector2<Type>& operator/=(Type aValue) { x /= aValue; y /= aValue; return *this; }

		Vector2<Type> operator+(const Vector2<Type>& aVector) const { return Vector2<Type>(x + aVector.x, y + aVector.y); }
		Vector2<Type> operator-(const Vector2<Type>& aVector) const { return Vector2<Type>(x - aVector.x, y - aVector.y); }
		Vector2<Type> operator*(const Vector2<Type>& aVector) const { return Vector2<Type>(x*aVector.x, y*aVector.y); }
		Vector2<Type> operator/(const Vector2<Type>& aVector) const { return Vector2<Type>(x / aVector.x, y / aVector.y); }

		Vector2<Type> operator*(Type aValue) const { return Vector2<Type>(x * aValue, y * aValue); }
		Vector2<Type> operator/(Type aValue) const { return Vector2<Type>(x / aValue, y / aValue); }

		bool operator==(const Vector2<Type>& aVector) const { return (x == aVector.x && y == aVector.y); }
		bool operator!=(const Vector2<Type>& aVector) const { return !(*this == aVector); }

		Vector2<Type> Normal() const { return Vector2<Type>(y, -x); }
		Vector2<Type>& Normalize();


		Type x;
		Type y;
	};


	template <class Type>
	Vector2<Type>& Vector2<Type>::Normalize()
	{
		const Type squaredLength = x * x + y * y;

		if (squaredLength > 0)
		{
			const Type invLen = Type(1) / (Type)sqrt(squaredLength);
			x *= invLen;
			y *= invLen;
		}

		return *this;
	}
	typedef Vector2<float> Vector2f;
	typedef Vector2<unsigned int>  Vector2ui;
}