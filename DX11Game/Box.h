#pragma once
#include "WinGame.h"

class Box :public WinGame
{
public:
	Box(HINSTANCE);
	~Box();

	bool Init();

	void Update(float dt);
	void Render(float dt);

private:

};