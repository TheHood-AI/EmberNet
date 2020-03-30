#pragma once
class GameObject
{
public:
	virtual ~GameObject() = default;
	virtual void Update(float aDelta) = 0;
	
	void SetID(unsigned aID);
	[[nodiscard]] unsigned GetID()const;

	[[nodiscard]] const VECTOR2F& GetPos()const;
	void SetPosition(const VECTOR2F& aPos);
	
protected:
	VECTOR2F myPos;
	unsigned int myID = 0;
	
private:
};

