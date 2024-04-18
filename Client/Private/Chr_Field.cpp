#include "stdafx.h"
#include "Chr_Field.h"

#include "FSM.h"
#include "Model.h"
#include "Shader.h"

#include "Level_Loading.h"
#include "Weapon_Anim.h"

#include "Chr_Field_State_Idle.h"
#include "Chr_Field_State_Move.h"
#include "Chr_Field_State_Walk.h"
#include "Chr_Field_State_Battle_Begin.h"
#include "Chr_Field_State_Item.h"

#include "ImGUI_Manager.h"

#include "Effect_2D.h"

CChr_Field::CChr_Field(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CChr_Field::CChr_Field(const CChr_Field& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CChr_Field::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CChr_Field::Initialize(void* pArg)
{
	m_eLevel = g_Level;

	GAMEOBJECT_DESC		GameObjectDesc{};

	GameObjectDesc.fSpeedPerSec = 3.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(360.f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_Weapon()))
		return E_FAIL;

#ifdef _DEBUG
	m_pImGUI_Manager = CImGUI_Manager::Get_Instance(m_pDevice, m_pContext);
	Safe_AddRef(m_pImGUI_Manager);
#endif

	return S_OK;
}

void CChr_Field::Tick(_float fTimeDelta)
{
	m_pFSMCom->Update(fTimeDelta);
	Update_FSMState(fTimeDelta);

	m_pNavigationCom->Set_Y(m_pTransformCom, -0.2f);
	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	m_pWeapon->Tick(fTimeDelta);

	// for test


	if (m_pGameInstance->Get_DIMouseState(DIMKS_RBUTTON)) {

		CEffect_2D::EFFECT_2D_DESC pDesc = {};
		pDesc.eEffect = Interface_2D::DUST_COLOR;
		pDesc.vColor = { 0.f,1.f,1.f,.5f };
		pDesc.vPosition = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
		pDesc.vPosition.y += 1.f;
		m_pGameInstance->Add_Clone(g_Level, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_2D"), &pDesc);
	}

}

HRESULT CChr_Field::Late_Tick(_float fTimeDelta)
{
	if (FAILED(__super::Late_Tick(fTimeDelta)))
		return E_FAIL;

	if(m_eState == BATTLE_BEGIN)
		m_pWeapon->Late_Tick(fTimeDelta);

	m_pModelCom->Play_Animation(fTimeDelta);

	// 임시
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
	return S_OK;
}

HRESULT CChr_Field::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i) {
		if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
			return E_FAIL;

		/* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이터를 다 던져야한다. */
		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		m_pModelCom->Render(i);
	}

#ifdef _DEBUG
	if (nullptr != m_pColliderCom)
		m_pColliderCom->Render();
	m_pImGUI_Manager->Tick(0);
	Show_ImGUI();
	m_pImGUI_Manager->Render();
#endif 


	return S_OK;
}

void CChr_Field::Start()
{
	m_pModelCom->Set_Animation(IDLE_NOR, false);
	Change_Animation_Weapon(CChr_Field::WEAPON_CLOSE_IDLE);
	m_pNavigationCom->Set_Index(m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION));

	_float4 vLook = ((CTransform*)m_pGameInstance->Get_Component(g_Level, g_strCameraLayerTag, g_strTransformTag, 0))->Get_State_Float4(CTransform::STATE_LOOK);
	vLook.y = 0.f;

	m_pTransformCom->Set_Look(XMVector3Normalize(XMLoadFloat4(&vLook)));

}

HRESULT CChr_Field::Change_State(STATE eState)
{
	m_eState = eState;
	m_pFSMCom->Change_State(eState);

	return S_OK;
}

void CChr_Field::Set_State_Battle_Start()
{
	Change_State(BATTLE_BEGIN);
}

void CChr_Field::Open_Level()
{
	switch (m_eLevel) {
	case LEVEL_FIELD:
		m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_BATTLE));
		break;
	case LEVEL_BOSS_BATTLE:
		m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_BOSS_BATTLE));
		break;
	}
}

void CChr_Field::Change_Animation(ANIMATION_CHR_FIELD iAnimationIndex, _bool isLoop)
{
	m_pModelCom->Set_Animation(iAnimationIndex, isLoop);
}

void CChr_Field::Change_Animation_Weapon(ANIMATION_CHR_LIGHT_WEAPON iAnimationIndex)
{
	m_pWeapon->Change_Animation(iAnimationIndex, false);
}

_float4 CChr_Field::Cal_Target_Direction()
{
	_vector vTargetDir = { 0.f,0.f,0.f,0.f };

	// 1. 카메라의 Look, Right 벡터 구한다
	_vector vCamRight, vCamLook;
	vCamRight = m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_VIEW).r[0];
	vCamLook = m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_VIEW).r[2];

	// 2. 키 입력을 기준으로 이동 방향 결정 
	if (m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_W))
		vTargetDir += vCamLook;
	if (m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_S))
		vTargetDir -= vCamLook;

	if (m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_A))
		vTargetDir -= vCamRight;
	if (m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_D))
		vTargetDir += vCamRight;

	// 3. y값 지우기
	vTargetDir.m128_f32[1] = 0;

	// 4. 정규화된 이동 방향 결정 
	vTargetDir = XMVector3Normalize(vTargetDir);

	_float4 vOutputDir = { 0.f,0.f,0.f,0.f };

	XMStoreFloat4(&vOutputDir, vTargetDir);

	return vOutputDir;
}

_float4 CChr_Field::Get_Look()
{
	// Player의 Look vector를 Y값을 지우고 리턴
	_float4 vChrLook = m_pTransformCom->Get_State_Float4(CTransform::STATE_LOOK);
	vChrLook.y = 0;
	XMStoreFloat4(&vChrLook, XMVectorSetW(XMVector3Normalize(XMLoadFloat4(&vChrLook)), 0.f));

	return vChrLook;
}

HRESULT CChr_Field::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Model_Light_Field"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* Com_Collider_Body */
	CBounding_OBB::BOUNDING_OBB_DESC		ColliderOBBDesc{};

	/* 로컬상의 정보를 셋팅한다. */
	ColliderOBBDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColliderOBBDesc.vSize = _float3(.6f, 1.6f, .6f);
	ColliderOBBDesc.vCenter = _float3(0.f, ColliderOBBDesc.vSize.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderOBBDesc)))
		return E_FAIL;

	wstring strNaviTag;
	switch (m_eLevel) {
	case LEVEL_FIELD:
		strNaviTag = TEXT("Prototype_Component_Navigation_Field");
		break;
	case LEVEL_FIELD_BOSS:
		strNaviTag = TEXT("Prototype_Component_Navigation_Boss_Battle");
		break;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, strNaviTag,
	TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom)))
	return E_FAIL;


	if(FAILED(Add_Component_FSM()))
		return E_FAIL;

	return S_OK;
}

HRESULT CChr_Field::Add_Component_FSM()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_FSM"),
		TEXT("Com_FSM"), (CComponent**)&m_pFSMCom)))
		return E_FAIL;

	m_pFSMCom->Add_State(IDLE, CChr_Field_State_Idle::Create(this));
	m_pFSMCom->Add_State(MOVE, CChr_Field_State_Move::Create(this));
	m_pFSMCom->Add_State(WALK, CChr_Field_State_Walk::Create(this));
	m_pFSMCom->Add_State(ITEM, CChr_Field_State_Item::Create(this));
	m_pFSMCom->Add_State(BATTLE_BEGIN, CChr_Field_State_Battle_Begin::Create(this));
	Change_State(IDLE);

	return S_OK;
}

HRESULT CChr_Field::Add_Weapon()
{
	/* For.Part_Weapon */
	CWeapon_Anim::WEAPON_ANIM_DESC	WeaponDesc{};

	CModel* pModel = m_pModelCom;

	WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4_Ptr();
	WeaponDesc.pSocket = pModel->Get_BonePtr("R_weapon");
	WeaponDesc.strModelTag = TEXT("Prototype_Component_Model_Light_Weapon");
	WeaponDesc.isAddCollider = false;

	m_pWeapon = dynamic_cast<CWeapon_Anim*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Part_Weapon_Anim"), &WeaponDesc));
	if (nullptr == m_pWeapon)
		return E_FAIL;

	return S_OK;
}

HRESULT CChr_Field::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

void CChr_Field::Update_FSMState(_float fTimeDelta)
{

	// 에러날 가능성이 매우 높은 코드?
	if (!m_isControl)
		return;

	switch (m_eState) {
	case IDLE:
		// 1. IDLE to WALK
		if (m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_W) ||
			m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_A) ||
			m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_S) ||
			m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_D))
			Change_State(WALK);
		// 2. IDLE to MOVE
		if (m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_LSHIFT) && (
			m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_W) ||
			m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_A) ||
			m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_S) ||
			m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_D)))
			Change_State(MOVE);
		break;
	case WALK:
		// 1. WALK to MOVE
		if(m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_LSHIFT))
			Change_State(MOVE);
		// 2. WALK to IDLE
		if (!(m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_W) ||
			m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_A) ||
			m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_S) ||
			m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_D)))
			Change_State(IDLE);
		break;
	case MOVE:
		// 1. MOVE to IDLE
		if (!m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_LSHIFT))
			Change_State(WALK);
		// 2. MOVE to IDLE
		if (!(m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_W) ||
			m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_A) ||
			m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_S) ||
			m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_D)))
			Change_State(IDLE);
		break;
	}
}

#ifdef _DEBUG
void CChr_Field::Show_ImGUI()
{
	_float4x4 worldmatrix = m_pTransformCom->Get_WorldFloat4x4();
	_float vRight[4] = { worldmatrix.m[0][0], worldmatrix.m[0][1] , worldmatrix.m[0][2] , worldmatrix.m[0][3] };
	_float vUp[4] = { worldmatrix.m[1][0], worldmatrix.m[1][1] , worldmatrix.m[1][2] , worldmatrix.m[1][3] };
	_float vLook[4] = { worldmatrix.m[2][0], worldmatrix.m[2][1] , worldmatrix.m[2][2] , worldmatrix.m[2][3] };
	_float vPos[4] = { worldmatrix.m[3][0], worldmatrix.m[3][1] , worldmatrix.m[3][2] , worldmatrix.m[3][3] };

	string str;
	switch (m_eState) {
	case IDLE:
		str = "IDLE";
		break;
	case WALK:
		str = "WALK";
		break;
	case MOVE:
		str = "MOVE";
		break;
	case ITEM:
		str = "ITEM";
		break;
	case BATTLE_BEGIN:
		str = "BATTLE_BEGIN";
		break;
	}

	ImGui::Begin("Chr_Field Tool");
	if (ImGui::TreeNode("Transform")) {
		ImGui::InputFloat4("Right", vRight);
		ImGui::InputFloat4("Up", vUp);
		ImGui::InputFloat4("Look", vLook);
		ImGui::InputFloat4("Pos", vPos);

		ImGui::TreePop();
	}
	ImGui::Text("Current State : ");
	ImGui::SameLine();
	ImGui::Text(str.c_str());
	ImGui::Text("Animation Index : %d", m_pModelCom->Get_CurrentAnimationIndex());
	ImGui::Text("Animation Frame : %f", m_pModelCom->Get_CurrentTrackPosition());

	ImGui::End();
}
#endif

CChr_Field* CChr_Field::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CChr_Field* pInstance = new CChr_Field(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CChr_Field"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CChr_Field::Clone(void* pArg)
{
	CChr_Field* pInstance = new CChr_Field(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CChr_Field"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChr_Field::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pFSMCom);
	Safe_Release(m_pWeapon);

	CImGUI_Manager::Destroy_Instance();
}
