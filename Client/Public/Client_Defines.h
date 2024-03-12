#pragma once

#include "GameInstance.h"

namespace Client
{
	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY,
				 LEVEL_FIELD, LEVEL_BATTLE, LEVEL_BOSSBATTLE_1, LEVEL_BOSSBATTLE_2,
				 LEVEL_MAPTOOL, LEVEL_PARSING, LEVEL_END };

	const unsigned int	g_iWinSizeX = 1280;
	const unsigned int	g_iWinSizeY = 720;

}

extern HWND				g_hWnd;
extern HINSTANCE		g_hInst;
extern Client::LEVEL	g_Level;

using namespace std;
using namespace Client;

//#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
//#include "imgui_internal.h"

inline float Saturate(float fMin, float fMax, float fValue) {
	if (fValue > fMax) {
		fValue = fMax;
	}
	if (fValue < fMin) {
		fValue = fMin;
	}

	return fValue;
}
