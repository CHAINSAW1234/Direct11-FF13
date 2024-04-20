#pragma once

#pragma warning (disable : 4251)

#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <DirectXCollision.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#ifdef _DEBUG
#define _ASSIMP
#endif

namespace Engine
{
	enum KeyState { KEY_FREE = 0, KEY_DOWN, KEY_PRESS, KEY_UP, KEY_END };
	enum MOUSEKEYSTATE { DIMKS_LBUTTON, DIMKS_RBUTTON, DIMKS_WHEEL, DIMKS_X, DIMKS_END };
	enum MOUSEMOVESTATE { DIMMS_X, DIMMS_Y, DIMMS_WHEEL, DIMMS_END };
}

#include "Effects11/d3dx11effect.h"
#include "DirectXTK/DDSTextureLoader.h"
#include "DirectXTK/WICTextureLoader.h"
#include "DirectXTK/ScreenGrab.h"
#include "DirectXTK/PrimitiveBatch.h"
#include "DirectXTK/VertexTypes.h"
#include "DirectXTK/Effects.h"

#include "DirectXTK/SpriteBatch.h"
#include "DirectXTK/SpriteFont.h"

#ifdef _ASSIMP
#include "assimp/scene.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#endif

#ifndef _ASSIMP
#define aiTextureType_DIFFUSE 1
#define aiTextureType_NORMALS 6
#define AI_TEXTURE_TYPE_MAX 21
#endif 



using namespace DirectX;

#include <fstream>
#include <algorithm> 
#include <string>
#include <vector>
#include <stack>
#include <deque>
#include <queue>
#include <list>
#include <map>
#include <unordered_map>
#include <random>

using namespace std;

namespace Engine
{
	const wstring g_strTransformTag = TEXT("Com_Transform");
	const wstring g_strModelTag = TEXT("Com_Model");
}

#include "Engine_Typedef.h"
#include "Engine_Macro.h"
#include "Engine_Function.h"
#include "Engine_Struct.h"


using namespace Engine;