#pragma once
#include "dxerr.h"

template<class T> void SafeRelease(T& t)
{
	if (t) {
		t->Release();
		t = nullptr;
	}
}

#if defined(DEBUG) | defined(_DEBUG)
#ifndef HR
#define HR(x)																\
	{																		\
		HRESULT hr_in_macro = (x);											\
		if(FAILED(hr_in_macro))												\
		{																	\
			DXTrace(__FILEW__, (DWORD)__LINE__, hr_in_macro, L#x, true);	\
		}																	\
	}

#endif

#else
#ifndef HR
#define HR(x) (x)
#endif
#endif