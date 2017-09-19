#pragma once
#include "Common\WinGame.h"
#include "Rain.h"

class DXApp :public WinGame
{
public:
	DXApp(HINSTANCE);
	~DXApp();

	bool Init() override;

	void Update(float dt);
	void Render(float dt);

private:
	Rain m_Rain;
};