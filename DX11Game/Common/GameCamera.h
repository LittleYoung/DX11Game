#pragma once
#include <DirectXMath.h>
using namespace DirectX;

class GameCamera
{
public:
	GameCamera();
	~GameCamera();

	void SetLens(float fovY, float aspect, float zn, float zf);
	void LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp);

	void Walk(float d);		//前后移动
	void Up(float d);		//上下移动
	void Strafe(float d);	//左右移动
	void Pitch(float a);
	void RotateY(float a);

	void UpdateMatrix();

	void SetPosition(XMFLOAT3 pos) { m_Position = pos; }
	// Get camera basis vectors.
	XMVECTOR GetRightXM()const;
	XMFLOAT3 GetRight()const;
	XMVECTOR GetUpXM()const;
	XMFLOAT3 GetUp()const;
	XMVECTOR GetLookXM()const;
	XMFLOAT3 GetLook()const;
	XMFLOAT3 GetPos()const;

	XMFLOAT4X4 GetView()const;
	XMMATRIX GetViewXM()const;
	XMFLOAT4X4 GetProj()const;
	XMMATRIX GetProjXM()const;
	XMMATRIX GetViewProjXM()const;

	float GetFovY() { return m_FovY; }
	float GetAspect() { return m_Aspect; }
	float GetFarZ() { return m_FarZ; }

private:
	// cache properties
	float m_FovY;
	float m_Aspect;
	float m_NearZ;
	float m_FarZ;

	float m_NearWindowHeight;
	float m_FarWindowHeight;

	XMFLOAT3 m_Position;
	XMFLOAT3 m_Look;
	XMFLOAT3 m_Right;
	XMFLOAT3 m_Up;

	XMFLOAT4X4 m_Proj;
	XMFLOAT4X4 m_View;
};