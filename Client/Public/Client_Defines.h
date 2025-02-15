#pragma once

#include "GameInstance.h"

namespace Client
{
	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, 
				 LEVEL_FIELD, LEVEL_FIELD_BOSS, LEVEL_BATTLE, LEVEL_BOSS_BATTLE,
				 LEVEL_MAPTOOL, LEVEL_EFFECTTOOL, LEVEL_PARSING, LEVEL_END };

	const unsigned int	g_iWinSizeX = 1280;
	const unsigned int	g_iWinSizeY = 720;
	const wstring		g_strFont10Tag = TEXT("Font_10");
	const wstring		g_strFont14Tag = TEXT("Font_14");
	const wstring		g_strFontEng32Tag = TEXT("Font_Eng32");
	const wstring		g_strFontNum14Tag = TEXT("Font_Num14");
	const wstring		g_strFontNum24Tag = TEXT("Font_Num24");
	const wstring		g_strFontAlphaTag = TEXT("Font_Alpha");

	const wstring		g_strCameraLayerTag = TEXT("Layer_ZZZCamera");
	const wstring		g_strBackGroundLayerTag = TEXT("Layer_ZZZZBackGround");
	const wstring		g_strMonsterLayerTag = TEXT("Layer_Monster");
	const wstring		g_strChrLayerTag = TEXT("Layer_Chr");
}

#define Random_Float(size) ( static_cast<_float>(std::rand()) / (RAND_MAX / (_float)size ) - (_float)size / 2.f );
#define EaseOutCublic(fTimeDelta)  (1 - pow(1-fTimeDelta, 3));

#define EaseOutInCirc(fTimeDelta) ( fTimeDelta < 0.5 ? ( 1.f - sqrt(1.f - pow( fTimeDelta, 2)))/2.f :  ( 1.f - sqrt(1.f - pow( fTimeDelta - 1, 2.f)))/2.f);

extern HWND				g_hWnd;
extern HINSTANCE		g_hInst;
extern Client::LEVEL	g_Level;

using namespace std;
using namespace Client;

//inline wstring StrToWstr(const string& value)
//{
//	return wstring(value.cbegin(), value.cend());
//}
//
//inline string WstrToStr(const wstring& value)
//{
//	return string(value.cbegin(), value.cend());
//}


//#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
//#include "imgui_internal.h"


#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif

#endif // _DEBUG
