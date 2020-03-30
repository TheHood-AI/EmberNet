#include "../TShared/Sh_GameWorld.h"

#include "NetworkPostMaster.h"
#include "S_Object.h"
struct SNetworkEvent;

class S_GameWorld : public Sh_GameWorld, CNetworkEventListener
{
public:
	~S_GameWorld() override;
	void Init(CNetworkPostMaster* aPostMaster);
	void Update(float aDelta);
	void RecieveMessage(const SNetworkEvent* aEvent) override;
private:
	std::vector<GameObject*> myGameObjects;
	std::vector<S_Object*> myObjects;
	float myCreateTimer = 0;
	char myCreatedObjects = 0;
	CNetworkPostMaster* myPostMaster = nullptr;
};

