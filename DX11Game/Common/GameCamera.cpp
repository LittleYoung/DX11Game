#include "GameCamera.h"



GameCamera::GameCamera()
	: m_Position(0.0f, 0.0f, 0.0f),
	m_Right(1.0f, 0.0f, 0.0f),
	m_Up(0.0f, 1.0f, 0.0f),
	m_Look(0.0f, 0.0f, 1.0f)
{
}


GameCamera::~GameCamera()
{
}

// Get camera basis vectors.
XMVECTOR GameCamera::GetRightXM()const
{
	return XMLoadFloat3(&m_Right);
}

XMFLOAT3 GameCamera::GetRight()const
{
	return m_Right;
}

XMVECTOR GameCamera::GetUpXM()const
{
	return XMLoadFloat3(&m_Up);
}

XMFLOAT3 GameCamera::GetUp()const
{
	return m_Up;
}

XMVECTOR GameCamera::GetLookXM()const
{
	return XMLoadFloat3(&m_Look);
}

XMFLOAT3 GameCamera::GetLook()const
{
	return m_Look;
}

XMFLOAT3 GameCamera::GetPos()const
{
	return m_Position;
}

XMFLOAT4X4 GameCamera::GetView()const
{
	return m_View;
}

XMMATRIX GameCamera::GetViewXM()const
{
	return XMLoadFloat4x4(&m_View);
}

XMFLOAT4X4 GameCamera::GetProj()const
{
	return m_Proj;
}

XMMATRIX GameCamera::GetProjXM() const {
	return XMLoadFloat4x4(&m_Proj);
}

XMMATRIX GameCamera::GetViewProjXM() const {
	return XMMatrixMultiply(GetViewXM(), GetProjXM());
}

void GameCamera::SetLens(float fovY, float aspect, float zn, float zf)
{
	// cache properties
	m_FovY = fovY;
	m_Aspect = aspect;
	m_NearZ = zn;
	m_FarZ = zf;

	m_NearWindowHeight = 2.0f * m_NearZ * tanf(0.5f*m_FovY);
	m_FarWindowHeight = 2.0f * m_FarZ * tanf(0.5f*m_FovY);

	XMMATRIX P = XMMatrixPerspectiveFovLH(m_FovY, m_Aspect, m_NearZ, m_FarZ);
	XMStoreFloat4x4(&m_Proj, P);
}

void GameCamera::LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp)
{
	XMVECTOR L = XMVector3Normalize(XMVectorSubtract(target, pos));
	XMVECTOR R = XMVector3Normalize(XMVector3Cross(worldUp, L));
	XMVECTOR U = XMVector3Cross(L, R);

	XMStoreFloat3(&m_Position, pos);
	XMStoreFloat3(&m_Look, L);
	XMStoreFloat3(&m_Right, R);
	XMStoreFloat3(&m_Up, U);
}

void GameCamera::Walk(float d)
{
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR l = XMLoadFloat3(&m_Look);
	XMVECTOR p = XMLoadFloat3(&m_Position);
	XMStoreFloat3(&m_Position, XMVectorMultiplyAdd(s, l, p));
}

void GameCamera::Up(float d)
{
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR l = XMLoadFloat3(&m_Up);
	XMVECTOR p = XMLoadFloat3(&m_Position);
	XMStoreFloat3(&m_Position, XMVectorMultiplyAdd(s, l, p));
}

void GameCamera::Strafe(float d)
{
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR r = XMLoadFloat3(&m_Right);
	XMVECTOR p = XMLoadFloat3(&m_Position);
	XMStoreFloat3(&m_Position, XMVectorMultiplyAdd(s, r, p));
}

void GameCamera::Pitch(float a)
{
	XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&m_Right), a);
	XMStoreFloat3(&m_Look, XMVector3TransformNormal(XMLoadFloat3(&m_Look), R));
	XMStoreFloat3(&m_Up, XMVector3TransformNormal(XMLoadFloat3(&m_Up), R));
}

void GameCamera::RotateY(float a)
{
	XMMATRIX R = XMMatrixRotationY(a);
	XMStoreFloat3(&m_Right, XMVector3TransformNormal(XMLoadFloat3(&m_Right), R));
	XMStoreFloat3(&m_Up, XMVector3TransformNormal(XMLoadFloat3(&m_Up), R));
	XMStoreFloat3(&m_Look, XMVector3TransformNormal(XMLoadFloat3(&m_Look), R));
}

void GameCamera::UpdateMatrix()
{
	XMVECTOR L = XMLoadFloat3(&m_Look);
	XMVECTOR R = XMLoadFloat3(&m_Right);
	XMVECTOR U = XMLoadFloat3(&m_Up);
	XMVECTOR P = XMLoadFloat3(&m_Position);

	L = XMVector3Normalize(L);
	U = XMVector3Normalize(XMVector3Cross(L, R));
	R = XMVector3Cross(U, L);

	float x = -XMVectorGetX(XMVector3Dot(P, R));
	float y = -XMVectorGetX(XMVector3Dot(P, U));
	float z = -XMVectorGetX(XMVector3Dot(P, L));

	XMStoreFloat3(&m_Right, R);
	XMStoreFloat3(&m_Up, U);
	XMStoreFloat3(&m_Look, L);

	m_View(0, 0) = m_Right.x;
	m_View(1, 0) = m_Right.y;
	m_View(2, 0) = m_Right.z;
	m_View(3, 0) = x;

	m_View(0, 1) = m_Up.x;
	m_View(1, 1) = m_Up.y;
	m_View(2, 1) = m_Up.z;
	m_View(3, 1) = y;

	m_View(0, 2) = m_Look.x;
	m_View(1, 2) = m_Look.y;
	m_View(2, 2) = m_Look.z;
	m_View(3, 2) = z;

	m_View(0, 3) = 0.0f;
	m_View(1, 3) = 0.0f;
	m_View(2, 3) = 0.0f;
	m_View(3, 3) = 1.0f;
}
