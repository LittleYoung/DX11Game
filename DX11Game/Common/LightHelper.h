#pragma once
#include <DirectXMath.h>
#include <Windows.h>
using namespace DirectX;

struct DirectionalLight
{
	DirectionalLight() { ZeroMemory(this, sizeof(DirectionalLight)); }
	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;
	XMFLOAT3 Direction;
	float Pad;
};

struct Material
{
	Material() { ZeroMemory(this, sizeof(Material)); }

	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular; // w = SpecPower
};

struct ShadowInfo
{
	XMFLOAT4X4 m_ViewProj;
	XMFLOAT4X4 m_ShadowTrans;
};