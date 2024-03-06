#pragma once

#pragma warning (disable : 4251)

#include <d3d11.h>
#include <DirectXCollision.h>
#include <d3dcompiler.h>


#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

namespace Engine
{
	enum KeyState { KEY_FREE = 0, KEY_DOWN, KEY_PRESS, KEY_UP, KEY_END };
	enum MOUSEKEYSTATE { DIMKS_LBUTTON, DIMKS_RBUTTON, DIMKS_WHEEL, DIMKS_X, DIMKS_END };
	enum MOUSEMOVESTATE { DIMMS_X, DIMMS_Y, DIMMS_WHEEL, DIMMS_END };
}

using namespace DirectX;

#include "Effects11/d3dx11effect.h"
#include "DirectXTK/DDSTextureLoader.h"
#include "DirectXTK/WICTextureLoader.h"
#include "assimp\scene.h"
#include "assimp\Importer.hpp"
#include "assimp\postprocess.h"

#include <fstream>
#include <algorithm>
#include <string>
#include <vector>
#include <queue>
#include <list>
#include <map>
using namespace std;

namespace Engine
{
	const wstring g_strTransformTag = TEXT("Com_Transform");
	const wstring g_strModelTag = TEXT("Com_Model");
}

#include "Engine_Macro.h"
#include "Engine_Function.h"
#include "Engine_Typedef.h"
#include "Engine_Struct.h"


using namespace Engine;

//#ifdef _DEBUG
//
//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>
//
//#ifndef DBG_NEW 
//
//#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
//#define new DBG_NEW 
//
//#endif
//
//#endif // _DEBUG

