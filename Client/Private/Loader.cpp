#include "stdafx.h"
#include "..\Public\Loader.h"
#include <process.h>
#include "GameInstance.h"

#include "Camera_Field.h"
#include "Camera_Battle.h" 
#include "MapObject.h"
#include "Trigger.h"
#include "Inventory.h"

#include "UI_Warning.h"
#include "UI_Number.h"
#include "UI_Cursor.h"
#include "UI_Pnal_Attack.h"
#include "UI_Pnal_Item.h"
#include "UI_Pnal.h"
#include "UI_ATB.h"
#include "UI_Chr.h"
#include "UI_Chain.h"
#include "UI_Monster_Hp.h"
#include "UI_Skill.h"
#include "UI_Optima.h"
#include "UI_Optima_Info.h"
#include "UI_Optima_Change.h"

#include "Chr_Field.h"
#include "Chr_Battle_Light.h"
#include "Chr_Battle_Sazh.h"
#include "Chr_Battle_Vanila.h"
#include "Body.h"
#include "Weapon_Anim.h"
#include "Sphere.h"
#include "Sphere_Heal.h"
#include "Bullet.h"
#include "HellBlast.h"

#include "Corpse.h"
#include "Leopard.h"
#include "Warload.h"
#include "Solider.h"
#include "Solider_Gun.h"
#include "Boss.h"

#include "LoadingOutBlur.h"
#include "Electricity.h"
#include "Electricity_Left.h"
#include "Effect_2D.h"
#include "Effect_2D_Bone.h"
#include "Effect_3D.h"
#include "Effect_Instance.h"
#include "Effect_Camera_Look.h"

#include "Particle_Blue.h"
#include "Camera_Free.h"
#include "BackGround.h"
#include "Player_Study.h"
#include "Body_Player.h"
#include "Weapon_Study.h"
#include "Monster_Study.h"
#include "Forklift.h"
#include "Terrain.h"
#include "Sky.h"

#pragma region Tool

#include "EffectTool.h"
#include "MapTool.h"
#include "Grid.h"

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

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingMain, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Start()
{
	EnterCriticalSection(&m_Critical_Section);

	static int isStart_Logo = true;

	if (isStart_Logo) {
		Loading_Prototype();
		Loading_For_Static_Component();
		isStart_Logo = false;
	}

	static int isStart = true;
	if (isStart && m_eNextLevelID != LEVEL_LOGO && m_eNextLevelID != LEVEL_PARSING) {
		Loading_For_Static_Component_Effect();
		isStart = false;
	}

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
	case LEVEL_FIELD_BOSS:
		g_Level = LEVEL_FIELD_BOSS;
		hr = Loading_For_Field_Boss();
		break;
	case LEVEL_BOSS_BATTLE:
		g_Level = LEVEL_BOSS_BATTLE;
		hr = Loading_For_Boss_Battle();
		break;
	case LEVEL_MAPTOOL:
		g_Level = LEVEL_MAPTOOL;
		hr = Loading_For_MapTool();
		break;
	case LEVEL_EFFECTTOOL:
		g_Level = LEVEL_EFFECTTOOL;
		hr = Loading_For_EffectTool();
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

#pragma region For_Study
	
	/* For.Prototype_GameObject_Particle_Blue */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Blue"),
		CParticle_Blue::Create(m_pDevice, m_pContext))))
		return E_FAIL;

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
		CMonster_Study::Create(m_pDevice, m_pContext))))
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

#pragma endregion

	/* For.Prototype_GameObject_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"),
		CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Camera_Field */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Field"),
		CCamera_Field::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Camera_Battle */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Battle"),
		CCamera_Battle::Create(m_pDevice, m_pContext))))
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

	/* For.Prototype_GameObject_Chr_Battle_Sazh */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Chr_Battle_Sazh"),
		CChr_Battle_Sazh::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Chr_Battle_Vanila */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Chr_Battle_Vanila"),
		CChr_Battle_Vanila::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Leopard */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Leopard"),
		CLeopard::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Warload */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Warload"),
		CWarload::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Solider */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Solider"),
		CSolider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Solider_Gun */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Solider_Gun"),
		CSolider_Gun::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Boss */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Boss"),
		CBoss::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Corpse */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Corpse"),
		CCorpse::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Sphere */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sphere"),
		CSphere::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Sphere */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sphere_Heal"),
		CSphere_Heal::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Sphere */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bullet"),
		CBullet::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_HellBlast */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HellBlast"),
		CHellBlast::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Electricity */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Electricity"),
		CElectricity::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Electricity_Left */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Electricity_Left"),
		CElectricity_Left::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_LoadingOutBlur */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LoadingOutBlur"),
		CLoadingOutBlur::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Effect_2D */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_2D"),
		CEffect_2D::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Effect_2D */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_2D_Bone"),
		CEffect_2D_Bone::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Effect_3D */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_3D"),
		CEffect_3D::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Effect_Instance */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Instance"),
		CEffect_Instance::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Effect_Camera_Look */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Camera_Look"),
		CEffect_Camera_Look::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Inventory */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Inventory"),
		CInventory::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_MapObject */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MapObject"),
		CMapObject::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Trigger */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Trigger"),
		CTrigger::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Warning */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Warning"),
		CUI_Warning::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/* For.Prototype_GameObject_UI_Cursor */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Number"),
		CUI_Number::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Cursor */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Cursor"),
		CUI_Cursor::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Pnal */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Pnal"),
		CUI_Pnal::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Pnal_Attack */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Pnal_Attack"),
		CUI_Pnal_Attack::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Pnal_Item */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Pnal_Item"),
		CUI_Pnal_Item::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_ATB */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_ATB"),
		CUI_ATB::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Chr */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Chr"),
		CUI_Chr::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Chr */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Chain"),
		CUI_Chain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Monster_Hp */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Monster_Hp"),
		CUI_Monster_Hp::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Skill */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Skill"),
		CUI_Skill::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_GameObject_UI_Optima */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Optima"),
		CUI_Optima::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Optima_Change */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Optima_Change"),
		CUI_Optima_Change::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Optima_Info */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Optima_Info"),
		CUI_Optima_Info::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_EffectTool */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EffectTool"),
		CEffectTool::Create(m_pDevice, m_pContext))))
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

HRESULT CLoader::Loading_For_Static_Component()
{

	/* For.Prototype_Component_FSM */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_FSM"),
		CFSM::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma region Collider

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Bone_AABB"),
		CCollider_Bone::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Bone_OBB"),
		CCollider_Bone::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Bone_Sphere"),
		CCollider_Bone::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;


#pragma endregion

#pragma region Navigation

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_Field"),
		CNavigation::Create_From_Model(m_pDevice, m_pContext, "../Bin/Resources/Models/MapObject/MapNavi/Map_Field_Navi_Origin.bin"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_Battle"),
		CNavigation::Create_From_Model(m_pDevice, m_pContext, "../Bin/Resources/Models/MapObject/MapNavi/Map_Battle_Navi.bin"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_Boss_Battle"),
		CNavigation::Create_From_Model(m_pDevice, m_pContext, "../Bin/Resources/Models/MapObject/MapNavi/Map_Boss_Battle_Navi.bin"))))
		return E_FAIL;

#pragma endregion

#pragma region Shader
	/* For.Prototype_Component_Shader_Trail */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Trail"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Trail.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxNorTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPosTex_UI */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex_UI.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxCube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxCube"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxModel */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModel.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxModel */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel_Effect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModel_Effect.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAnimModel */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimModel.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPosTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPos"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPos.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxInstance_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxInstance_Rect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance_Rect.hlsl"), VTXINSTANCE_RECT::Elements, VTXINSTANCE_RECT::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxInstance_Point */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxInstance_Point"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance_Point.hlsl"), VTXINSTANCE_POINT::Elements, VTXINSTANCE_POINT::iNumElements))))
		return E_FAIL;


#pragma endregion

	return S_OK;
}

HRESULT CLoader::Loading_For_Static_Component_Effect()
{
#pragma region Texture
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Diffuse"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Diffuse/Diffuse (%d).png"), 173))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Mask/Mask_%d.png"), 141))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Particle"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Particle/Particle (%d).png"), 113))))
		return E_FAIL;
#pragma endregion

#pragma region Effect
	string tag = ".bin";
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Capsule"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effect/Default/Capsule" + tag))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Circle"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effect/Default/Circle" + tag))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Circle2"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effect/Default/Circle2" + tag))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Crack"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effect/Default/Crack" + tag))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Cylinder"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effect/Default/Cylinder" + tag))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Cylinder1"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effect/Default/Cylinder1" + tag))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Cylinder2"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effect/Default/Cylinder2" + tag))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Electricity"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effect/Default/Electricity" + tag))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Electricity2"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effect/Default/Electricity2" + tag))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Shock"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effect/Default/Shock" + tag))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Slash"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effect/Default/Slash" + tag))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Sphere"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effect/Default/Sphere" + tag))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Thunder0"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effect/Default/Thunder0" + tag))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Thunder1"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effect/Default/Thunder1" + tag))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Thunder2"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effect/Default/Thunder2" + tag))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Thunder3"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effect/Default/Thunder3" + tag))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Thunder4"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effect/Default/Thunder4" + tag))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Thunder5"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effect/Default/Thunder5" + tag))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Tornado"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effect/Default/Tornado" + tag))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Trail"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effect/Default/Trail" + tag))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Wind"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effect/Default/Wind" + tag))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Effect_1"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effect/Extract/Effect_1" + tag))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Effect_2"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effect/Extract/Effect_2" + tag))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Optima1"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effect/Extract/Optima_1" + tag))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Optima2"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effect/Extract/Optima_2" + tag))))
		return E_FAIL;


#pragma endregion 

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_SkyBox"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/SkyBox/SkyBox" + tag))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_For_Logo()
{
	m_strLoadingText = TEXT("텍스쳐를(을) 로딩 중 입니다.");
	/* For.Prototype_Component_Texture_Logo */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Logo"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Logo.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Logo */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Title"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Title.png")))))
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

	/* Prototype_Component_Texture_Snow */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Snow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Snow/Snow.png"), 1))))
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

	m_strLoadingText = TEXT("네비게이션를(을) 로딩 중 입니다.");
	/* For.Prototype_Component_Navigation */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Navigation.dat")))))
		return E_FAIL;

	m_strLoadingText = TEXT("모델를(을) 로딩 중 입니다.");
	/* Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
		return E_FAIL;

	_matrix		TransformMatrix = XMMatrixIdentity();

	/* Prototype_Component_Model_Fiona */
	TransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));

	//// 상단 fbx, 하단, bin
	//if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Fiona"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Fiona/Fiona.fbx", TransformMatrix))))
	//	return E_FAIL;

	////if (FAILED(m_pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Fiona"),
	////	CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Fiona/Fiona.bin"))))
	////	return E_FAIL;

	///* Prototype_Component_Model_ForkLift */
	//TransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	//if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_ForkLift"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/ForkLift/ForkLift.fbx", TransformMatrix))))
	//	return E_FAIL;

	///* Prototype_Component_VIBuffer_Cube */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"),
	//	CVIBuffer_Cube::Create(m_pGraphic_Device))))
	//	return E_FAIL;

	///*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"),
	//	CVIBuffer_Terrain::Create(m_pGraphic_Device, 1000, 1000))))
	//	return E_FAIL;*/	
	
		/* Prototype_Component_VIBuffer_Instance_Rect */
	CVIBuffer_Instance::INSTANCE_DESC		InstanceDesc{};

	InstanceDesc.vPivot = _float3(0.f, 0.f, 0.f);
	InstanceDesc.vRange = _float3(2.f, 2.f, 2.f);
	InstanceDesc.vMinScale = _float3(0.2f, 0.2f, 0.2f);
	InstanceDesc.vMaxScale = _float3(0.4f, 0.4f, 0.4f);
	InstanceDesc.iNumInstance = 100;
	InstanceDesc.vLifeTime = _float2(0.f, 5.0f);


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Instance_Rect"),
		CVIBuffer_Instance_Rect::Create(m_pDevice, m_pContext, InstanceDesc))))
		return E_FAIL;



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

HRESULT CLoader::Loading_For_EffectTool()
{
	m_strLoadingText = TEXT("텍스쳐를(을) 로딩 중 입니다.");

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_VIBuffer_Line"),
		CVIBuffer_Line::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_strLoadingText = TEXT("모델를(을) 로딩 중 입니다.");

	string tag = ".bin";

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Map_Battle"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/MapObject/MapObject/Map_Battle" + tag))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Solider"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Monster/Solider/Solider" + tag))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Solider_Weapon"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Monster/Solider/Weapon/Solider_Weapon" + tag))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Boss"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Monster/Boss/Boss" + tag))))
		return E_FAIL;

	m_strLoadingText = TEXT("셰이더를(을) 로딩 중 입니다.");

	m_strLoadingText = TEXT("객체를(을) 로딩 중 입니다.");

	m_strLoadingText = TEXT("EFFECT TOOL LEVEL의 로딩이 완료되었습니다.");

	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_For_Field()
{

	m_strLoadingText = TEXT("텍스쳐를(을) 로딩 중 입니다.");

#pragma region Effect

	/* Prototype_Component_Texture_Dust_Color */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Dust_Color"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Dust_Color/Dust_Color%d.png"), 26))))
		return E_FAIL;

	/* Prototype_Component_Texture_Dust_Color */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Fire"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/fIRE.DDS")))))
		return E_FAIL;



#pragma endregion


	m_strLoadingText = TEXT("모델를(을) 로딩 중 입니다.");

	string tag = ".bin";
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Map_Field"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/MapObject/MapObject/Map_Field" + tag))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Light_Field"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Chr/Light/Body/Light_Field" + tag))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Light_Weapon"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Chr/Light/Weapon/Light_Weapon" + tag))))
		return E_FAIL;

#pragma region Monster

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Leopard"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Monster/Leopard/Leopard" + tag))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Warload"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Monster/Warload/Warload" + tag))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Solider"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Monster/Solider/Solider" + tag))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Solider_Weapon"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Monster/Solider/Weapon/Solider_Weapon" + tag))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Solider_Gun"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Monster/Solider/Solider_Gun" + tag))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Solider_Gun_Weapon"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Monster/Solider/Weapon/Solider_Weapon_Gun" + tag))))
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
	/* For.Prototype_Component_VIBuffer_Trail */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_VIBuffer_Trail"),
		CVIBuffer_Trail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_strLoadingText = TEXT("텍스쳐를(을) 로딩 중 입니다.");

#pragma region UI

	/* For.Prototype_Component_Texture_UI_Warning */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_UI_Warning"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Warning.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_Cursor */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_UI_Cursor"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Cursor.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_ATB_Inner */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_UI_Pnal_Anim"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Pnal/Pnalanm_%d.png"), 16))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_ATB_Border */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_UI_Pnal_Border"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Pnal/Pnal_Border.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_ATB_Border */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_UI_Pnal_Inner"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Pnal/Pnal_Inner.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_ATB_Border */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_UI_ATB_Border"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Pnal/Pnal_ATB_Border.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_ATB_Inner */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_UI_ATB_Inner"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Pnal/Pnal_ATB_Inner.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_ATB_Inner */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_UI_ATB_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/ATB_Mask.dds")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_Chain_Inner */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_UI_Chain_Border"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Pnal/Chain_Border.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_Optima */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_UI_Optima"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Tab.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_Optima_Info */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_UI_Optima_Info"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Optima/Optima_Info.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_Optima_Info_Face */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_UI_Optima_Info_Face"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Optima/Optima_Face%d.png"), 3))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_Grad */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_UI_Grad"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Grad.dds")))))
		return E_FAIL;

#pragma endregion

#pragma region Effect
	/* Prototype_Component_Texture_Trail */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Trail"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Trail.dds")))))
		return E_FAIL;


	/* Prototype_Component_Texture_Dust_Color */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Dust_Color"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Dust_Color/Dust_Color%d.png"), 26))))
		return E_FAIL;

	/* Prototype_Component_Texture_Dust_Color */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Fire"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/fIRE.dds")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Gun_Fire */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Gun_Fire"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Gun_Fire/Gun_Fire_%d.png"), 2))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Hit_1 */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Hit_1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Hit/Hit_1/Hit_1 (%d).png"), 6))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Hit_2 */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Hit_2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Hit/Hit_2/Hit_2 (%d).png"), 5))))
		return E_FAIL;

#pragma endregion

	/* For.Prototype_Component_Texture_Corpse */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Corpse"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Corpse_Mask.dds")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_VIBuffer_Line"),
		CVIBuffer_Line::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_strLoadingText = TEXT("모델를(을) 로딩 중 입니다.");

	string tag = ".bin";

#pragma region Chr

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Map_Battle"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/MapObject/MapObject/Map_Battle" + tag))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Light_Battle"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Chr/Light/Body/Light_Battle" + tag))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Light_Weapon"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Chr/Light/Weapon/Light_Weapon" + tag))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Sazh_Battle"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Chr/Sazh/Body/Sazh_Battle" + tag))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Sazh_Weapon"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Chr/Sazh/Weapon/Sazh_Weapon" + tag))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Vanila_Battle"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Chr/Vanila/Body/Vanila_Battle" + tag))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Vanila_Weapon"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Chr/Vanila/Weapon/Vanila_Weapon" + tag))))
		return E_FAIL;

#pragma endregion

#pragma region Monster 
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Leopard"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Monster/Leopard/Leopard" + tag))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Warload"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Monster/Warload/Warload" + tag))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Solider"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Monster/Solider/Solider" + tag))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Solider_Weapon"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Monster/Solider/Weapon/Solider_Weapon" + tag))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Solider_Gun"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Monster/Solider/Solider_Gun" + tag))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Solider_Gun_Weapon"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Monster/Solider/Weapon/Solider_Weapon_Gun" + tag))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Boss"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Monster/Boss/Boss" + tag))))
		return E_FAIL;

#pragma endregion

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Bullet"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Bullet/Bullet" + tag))))
		return E_FAIL;

	m_strLoadingText = TEXT("셰이더를(을) 로딩 중 입니다.");

	m_strLoadingText = TEXT("객체의 원형를(을) 로딩 중 입니다.");

	m_strLoadingText = TEXT("로딩이 완료되었습니다.");

	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_For_Field_Boss()
{

	m_strLoadingText = TEXT("텍스쳐를(을) 로딩 중 입니다.");

	m_strLoadingText = TEXT("모델를(을) 로딩 중 입니다.");

	string tag = ".bin";
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Map_BossBattle_1"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/MapObject/MapObject/Map_BossBattle_1" + tag))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Light_Field"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Chr/Light/Body/Light_Field" + tag))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Boss"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Monster/Boss/Boss" + tag))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Light_Weapon"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Chr/Light/Weapon/Light_Weapon" + tag))))
		return E_FAIL;

	m_strLoadingText = TEXT("셰이더를(을) 로딩 중 입니다.");

	m_strLoadingText = TEXT("객체의 원형를(을) 로딩 중 입니다.");

	m_strLoadingText = TEXT("로딩이 완료되었습니다.");


	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_For_Boss_Battle()
{
	/* For.Prototype_Component_VIBuffer_Trail */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_VIBuffer_Trail"),
		CVIBuffer_Trail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_strLoadingText = TEXT("텍스쳐를(을) 로딩 중 입니다.");

#pragma region UI

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_UI_Warning"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Warning.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_Cursor */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_UI_Cursor"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Cursor.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_ATB_Inner */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_UI_Pnal_Anim"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Pnal/Pnalanm_%d.png"), 16))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_ATB_Border */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_UI_Pnal_Border"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Pnal/Pnal_Border.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_ATB_Border */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_UI_Pnal_Inner"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Pnal/Pnal_Inner.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_ATB_Border */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_UI_ATB_Border"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Pnal/Pnal_ATB_Border.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_ATB_Inner */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_UI_ATB_Inner"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Pnal/Pnal_ATB_Inner.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_ATB_Inner */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_UI_ATB_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/ATB_Mask.dds")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_Chain_Inner */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_UI_Chain_Border"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Pnal/Chain_Border.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_Optima */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_UI_Optima"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Tab.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_Optima_Info */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_UI_Optima_Info"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Optima/Optima_Info.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_Optima_Info_Face */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_UI_Optima_Info_Face"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Optima/Optima_Face%d.png"), 3))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_Grad */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_UI_Grad"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Grad.dds")))))
		return E_FAIL;

#pragma endregion

#pragma region Effect
	/* Prototype_Component_Texture_Trail */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Trail"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Trail.dds")))))
		return E_FAIL;

	/* Prototype_Component_Texture_Dust_Color */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Dust_Color"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Dust_Color/Dust_Color%d.png"), 26))))
		return E_FAIL;

	/* Prototype_Component_Texture_Dust_Color */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Fire"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/fIRE.dds")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Gun_Fire */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Gun_Fire"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Gun_Fire/Gun_Fire_%d.png"), 2))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Hit_1 */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Hit_1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Hit/Hit_1/Hit_1 (%d).png"), 6))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Hit_2 */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Hit_2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Hit/Hit_2/Hit_2 (%d).png"), 5))))
		return E_FAIL;

#pragma endregion

	m_strLoadingText = TEXT("모델를(을) 로딩 중 입니다.");

	/* For.Prototype_Component_Texture_Corpse */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Corpse"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Corpse_Mask.dds")))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_VIBuffer_Line"),
		CVIBuffer_Line::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	string tag = ".bin";

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Map_BossBattle_1"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/MapObject/MapObject/Map_BossBattle_1" + tag))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Bullet"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Bullet/Bullet" + tag))))
		return E_FAIL;

#pragma region Chr

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Light_Battle"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Chr/Light/Body/Light_Battle" + tag))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Light_Weapon"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Chr/Light/Weapon/Light_Weapon" + tag))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Sazh_Battle"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Chr/Sazh/Body/Sazh_Battle" + tag))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Sazh_Weapon"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Chr/Sazh/Weapon/Sazh_Weapon" + tag))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Vanila_Battle"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Chr/Vanila/Body/Vanila_Battle" + tag))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Vanila_Weapon"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Chr/Vanila/Weapon/Vanila_Weapon" + tag))))
		return E_FAIL;

#pragma endregion

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Boss"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Monster/Boss/Boss" + tag))))
		return E_FAIL;

#pragma region Electricity
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Boss_Electricity_0"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effect/Electricity/Boss_Electricity_0" + tag))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Boss_Electricity_1"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effect/Electricity/Boss_Electricity_1" + tag))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Boss_Electricity_2"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effect/Electricity/Boss_Electricity_2" + tag))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Boss_Electricity_3"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Effect/Electricity/Boss_Electricity_3" + tag))))
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

