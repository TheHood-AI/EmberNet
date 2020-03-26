#pragma once
#include <string>
#include "S_GameWorld.h"
#include "../TShared/NetworkPostMaster.h"
#include "ServerMainInteface.h"

class S_Game final : public CNetworkEventListener
{
public:
	S_Game();
	~S_Game();
	bool Init(const std::wstring& aVersion = L"", HWND aHWND = nullptr, int aX = 0, int aY = 0);
	
	virtual void RecieveMessage(const SNetworkEvent* aEvent) override;

	IServerMain myServerMainInterface;

private:
	void InitCallBack();
	void UpdateCallBack();
	LRESULT WinProc(HWND aHWnd, UINT aMessage, WPARAM aWParam, LPARAM aLParam);

	S_GameWorld myGameWorld;
	CNetworkPostMaster myPostMaster;
	std::vector<float> myNetworkDataRecieved;
	std::vector<float> myNetworkDataSent;
};

