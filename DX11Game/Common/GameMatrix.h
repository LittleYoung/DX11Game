#pragma once
#include <DirectXMath.h>

using namespace DirectX;

struct GameMatrix
{
	float m[4][4];
	void Set(CXMMATRIX xm) {
		XMFLOAT4X4 xmf;
		XMStoreFloat4x4(&xmf, xm);
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				m[i][j] = xmf.m[j][i];
			}
		}
	}
};