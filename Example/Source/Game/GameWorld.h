#pragma once
#include "../TShared/Sh_GameWorld.h"
#include "Client.h"
#include "C_Object.h"

class C_GameWorld : public Sh_GameWorld, CNetworkEventListener
{
public:
	C_GameWorld();
	~C_GameWorld();

	void Init();
	void Update(float aTimeDelta);

	void RecieveMessage(const SNetworkEvent* aEvent) override;
private:
	CClient myClient;
	CNetworkPostMaster myPostMaster;
	std::vector<C_Object*> myObjects;
};
