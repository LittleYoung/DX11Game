#pragma once
#include "Common\WinGame.h"

class DXApp :public WinGame
{
public:
	DXApp(HINSTANCE);
	~DXApp();

	bool Init();

	void Update(float dt);
	void Render(float dt);

private:

};