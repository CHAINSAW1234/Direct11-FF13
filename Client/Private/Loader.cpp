#include "stdafx.h"
#include "..\Public\Loader.h"
#include <process.h>
#include "GameInstance.h"

#include "Camera_Field.h"
#include "MapObject.h"
#include "Grid.h"
#include "Chr.h"
#include "Chr_Field.h"
#include "Chr_Battle_Light.h"
#include "Body.h"
#include "Weapon_Anim.h"


#include "Camera_Free.h"
#include "BackGround.h"
#include "Player_Study.h"
#include "Body_Player.h"
#include "Weapon_Study.h"
#include "Monster.h"
#include "Forklift.h"
#include "Terrain.h"
//#include "Effect.h"
//#include "Sky.h"

#pragma region MapTool

#include "MapTool.h"

#pragma endregion

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext}
	, m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

_uint APIENTRY LoadingMain(void* pArg)
{
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	/* 로더에게 지정된 레벨을 준비해라*/
	CLoader*		pLoader = (CLoader*)pArg;

	if (FAILED(pLoader->Start()))
		return 1;

	CoUninitialize();

	return 0;
}

HRESULT CLoader::Initialize(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	InitializeCriticalSection(&m_Critical_Section);

	Loading_Prototype();

	/* 스레드를 생성하낟. */
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingMain, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Start()
{
	EnterCriticalSection(&m_Critical_Section);

	HRESULT		hr = { 0 };

	switch (m_eNextLevelID)
	{
	case LEVEL_LOGO:
		g_Level = LEVEL_LOGO;
		hr = Loading_For_Logo();
		break;
	case LEVEL_GAMEPLAY:
		g_Level = LEVEL_GAMEPLAY;
		hr = Loading_For_GamePlay();
		break;
	case LEVEL_FIELD:
		g_Level = LEVEL_FIELD;
		hr = Loading_For_Field();
		break;
	case LEVEL_BATTLE:
		g_Level = LEVEL_BATTLE;
		hr = Loading_For_Battle();
		break;
	case LEVEL_MAPTOOL:
		g_Level = LEVEL_MAPTOOL;
		hr = Loading_For_MapTool();
		break;
	case LEVEL_PARSING:
		hr = S_OK;
		m_isFinished = true;
		m_strLoadingText = TEXT("로딩이 완료되었습니다.");
		g_Level = LEVEL_PARSING;
		break;
	}

	LeaveCriticalSection(&m_Critical_Section);

	if (FAILED(hr))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_Prototype()
{
	/* For.Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Camera_Free */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Free"),
		CCamera_Free::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_Study"),
		CPlayer_Study::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Monster */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster"),
		CMonster::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Monster */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ForkLift"),
		CForkLift::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Part_Body_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Part_Body_Player"),
		CBody_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Part_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Part_Weapon_Study"),
		CWeapon_Study::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Camera_Field */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Field"),
		CCamera_Field::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Grid */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Chr"),
		CChr::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Part_Body */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Part_Body"),
		CBody::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Part_Weapon_Anim */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Part_Weapon_Anim"),
		CWeapon_Anim::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Chr_Field */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Chr_Field"),
		CChr_Field::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Chr_Battle_Light */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Chr_Battle_Light"),
		CChr_Battle_Light::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_MapObject */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MapObject"),
		CMapObject::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_MapTool */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MapTool"),
		CMapTool::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Grid */
 	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Grid"),
		CGrid::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_For_Logo()
{
	m_strLoadingText = TEXT("텍스쳐를(을) 로딩 중 입니다.");
	/* For.Prototype_Component_Texture_Logo */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Default%d.jpg"), 2))))
		return E_FAIL;

	m_strLoadingText = TEXT("모델를(을) 로딩 중 입니다.");
	
	m_strLoadingText = TEXT("셰이더를(을) 로딩 중 입니다.");
	
	m_strLoadingText = TEXT("객체의 원형를(을) 로딩 중 입니다.");
	
	m_strLoadingText = TEXT("로딩이 완료되었습니다.");

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay()
{
	m_strLoadingText = TEXT("텍스쳐를(을) 로딩 중 입니다.");
	///* Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Tile%d.dds"), 2))))
		return E_FAIL;

	/*Prototype_Component_Texture_Mask*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Mask.dds"), 1))))
		return E_FAIL;

	/*Prototype_Component_Texture_Brush */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Brush"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Brush.png"), 1))))
		return E_FAIL;


	///* Prototype_Component_Texture_Player */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Player"),
	//	CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resources/Textures/Player/Player.png")))))
	//	return E_FAIL;

	///* Prototype_Component_Texture_Monster */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster"),
	//	CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resources/Textures/Monster/Monster.png")))))
	//	return E_FAIL;

	///* Prototype_Component_Texture_Sky */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Sky"),
	//	CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXCUBE, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 4))))
	//	return E_FAIL;

	///* Prototype_Component_Texture_Effect */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect"),
	//	CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resources/Textures/Explosion/Explosion%d.png"), 90))))
	//	return E_FAIL;
	//
	m_strLoadingText = TEXT("모델를(을) 로딩 중 입니다.");
	/* Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
		return E_FAIL;

	_matrix		TransformMatrix = XMMatrixIdentity();

	/* Prototype_Component_Model_Fiona */
	TransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));

	// 상단 fbx, 하단, bin
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Fiona"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Fiona/Fiona.fbx", TransformMatrix))))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Fiona"),
	//	CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Fiona/Fiona.bin"))))
	//	return E_FAIL;

	/* Prototype_Component_Model_ForkLift */
	TransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_ForkLift"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/ForkLift/ForkLift.fbx", TransformMatrix))))
		return E_FAIL;

	///* Prototype_Component_VIBuffer_Cube */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"),
	//	CVIBuffer_Cube::Create(m_pGraphic_Device))))
	//	return E_FAIL;

	///*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"),
	//	CVIBuffer_Terrain::Create(m_pGraphic_Device, 1000, 1000))))
	//	return E_FAIL;*/	
	
	m_strLoadingText = TEXT("셰이더를(을) 로딩 중 입니다.");
	
	m_strLoadingText = TEXT("객체를(을) 로딩 중 입니다.");


	///* For.Prototype_GameObject_Player */
	//if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_Study"),
	//	CPlayer_Study::Create(m_pGraphic_Device))))
	//	return E_FAIL;

	///* For.Prototype_GameObject_Effect */
	//if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect"),
	//	CEffect::Create(m_pGraphic_Device))))
	//	return E_FAIL;

	///* For.Prototype_GameObject_Sky */
	//if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
	//	CSky::Create(m_pGraphic_Device))))
	//	return E_FAIL;
	//
	m_strLoadingText = TEXT("로딩이 완료되었습니다.");

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_MapTool()
{
	m_strLoadingText = TEXT("텍스쳐를(을) 로딩 중 입니다.");

	///* Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Tile%d.jpg")))))
		return E_FAIL;

	///* Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_PrevMapObject"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Models/MapObject/PrevMapObject/Map%d.jpg"),4))))
		return E_FAIL;

	m_strLoadingText = TEXT("모델를(을) 로딩 중 입니다.");

#pragma region 모델 로딩
	/* Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_VIBuffer_Line"),
		CVIBuffer_Line::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	// 주석으로 바이너리 변경 하기
	//string tag = ".fbx";
	string tag = ".bin";

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Map_Field"),
		CModel::Create(m_pDevice, m_pContext, /*CModel::TYPE_NONANIM,*/ "../Bin/Resources/Models/MapObject/MapObject/Map_Field" + tag/*, XMMatrixIdentity()*/))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Map_Battle"),
		CModel::Create(m_pDevice, m_pContext, /*CModel::TYPE_NONANIM,*/ "../Bin/Resources/Models/MapObject/MapObject/Map_Battle" + tag/*, XMMatrixIdentity()*/))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Map_BossBattle_1"),
		CModel::Create(m_pDevice, m_pContext, /*CModel::TYPE_NONANIM,*/ "../Bin/Resources/Models/MapObject/MapObject/Map_BossBattle_1" + tag/*, XMMatrixIdentity()*/))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Map_BossBattle_2"),
		CModel::Create(m_pDevice, m_pContext, /*CModel::TYPE_NONANIM,*/ "../Bin/Resources/Models/MapObject/MapObject/Map_BossBattle_2" + tag/*, XMMatrixIdentity()*/))))
		return E_FAIL;

#pragma endregion 



	m_strLoadingText = TEXT("셰이더를(을) 로딩 중 입니다.");
	m_strLoadingText = TEXT("객체를(을) 로딩 중 입니다.");

	m_strLoadingText = TEXT("MAP TOOL LEVEL의 로딩이 완료되었습니다.");

	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_For_Field()
{

	m_strLoadingText = TEXT("텍스쳐를(을) 로딩 중 입니다.");

	m_strLoadingText = TEXT("모델를(을) 로딩 중 입니다.");

	string tag = ".bin";
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Map_Field"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/MapObject/MapObject/Map_Field" + tag))))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Light"),
	//	CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/chr/Light/Light" + tag))))
	//	return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Light_Field"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Chr/Light/Body/Light_Field" + tag))))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Light_Weapon"),
	//	CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Chr/Light/Weapon/Light_Weapon" + tag))))
	//	return E_FAIL;

#pragma region Temp

	_matrix		TransformMatrix = XMMatrixIdentity();

	/* Prototype_Component_Model_Fiona */
	TransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Fiona"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Fiona/Fiona.fbx", TransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_ForkLift */
	TransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_ForkLift"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/ForkLift/ForkLift.fbx", TransformMatrix))))
		return E_FAIL;

#pragma endregion


	m_strLoadingText = TEXT("셰이더를(을) 로딩 중 입니다.");

	m_strLoadingText = TEXT("객체의 원형를(을) 로딩 중 입니다.");

	m_strLoadingText = TEXT("로딩이 완료되었습니다.");


	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_For_Battle()
{
	m_strLoadingText = TEXT("텍스쳐를(을) 로딩 중 입니다.");

	m_strLoadingText = TEXT("모델를(을) 로딩 중 입니다.");

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_VIBuffer_Line"),
		CVIBuffer_Line::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	string tag = ".bin";
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Map_Battle"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/MapObject/MapObject/Map_Battle" + tag))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Light_Battle"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Chr/Light/Body/Light_Battle" + tag))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Light_Weapon"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Chr/Light/Weapon/Light_Weapon" + tag))))
		return E_FAIL;

#pragma region Temp

	_matrix		TransformMatrix = XMMatrixIdentity();

	/* Prototype_Component_Model_Fiona */
	TransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Fiona"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Fiona/Fiona.fbx", TransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_ForkLift */
	TransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_ForkLift"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/ForkLift/ForkLift.fbx", TransformMatrix))))
		return E_FAIL;

#pragma endregion


	m_strLoadingText = TEXT("셰이더를(을) 로딩 중 입니다.");

	m_strLoadingText = TEXT("객체의 원형를(을) 로딩 중 입니다.");

	m_strLoadingText = TEXT("로딩이 완료되었습니다.");


	m_isFinished = true;
	return S_OK;
}

CLoader * CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	CLoader*		pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX(TEXT("Failed To Created : CLoader"));

		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	DeleteObject(m_hThread);

	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_Critical_Section);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}

