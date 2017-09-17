#pragma once
#include <windows.h>
#include <DirectXMath.h>
using namespace DirectX;

class MathHelper
{
public:
	static const float Pi;

	template<typename T>
	static T Clamp(const T& x, const T& low, const T& high)
	{
		return x < low ? low : (x > high ? high : x);
	}

	template<typename T>
	static T Min(const T& x, const T& y)
	{
		return x < y ? x : y;
	}

	// Returns random float in [0, 1).
	static float RandF()
	{
		return (float)(rand()) / (float)RAND_MAX;
	}


	static float RandF(float a, float b)
	{
		return a + RandF()*(b - a);
	}

	static XMMATRIX InverseTranspose(CXMMATRIX M)
	{
		// Inverse-transpose is just applied to normals.  So zero out 
		// translation row so that it doesn't get into our inverse-transpose
		// calculation--we don't want the inverse-transpose of the translation.
		XMMATRIX A = M;
		A.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

		XMVECTOR det = XMMatrixDeterminant(A);
		return XMMatrixTranspose(XMMatrixInverse(&det, A));
	}

};