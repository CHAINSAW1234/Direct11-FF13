#include "stdafx.h"
#include "Chr_Field.h"

#include "FSM.h"
#include "Model.h"
#include "Shader.h"

#include "Chr_Field_State_Idle.h"
#include "Chr_Field_State_Move.h"
#include "Chr_Field_State_Walk.h"
#include "Chr_Field_State_Battle_Begin.h"
#include "Chr_Field_State_Item.h"

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

	GameObjectDesc.fSpeedPerSec = 10.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pModelCom->Set_Animation(0, false);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(rand() % 20, 2.f, rand() % 20, 1.f));

	return S_OK;
}

void CChr_Field::Tick(_float fTimeDelta)
{
	m_pFSMCom->Update(fTimeDelta);
	Update_FSMState(fTimeDelta);
	/*if (m_pGameInstance->Get_KeyState(KEY_DOWN, DIK_UPARROW))
		m_pModelCom->Set_Animation(m_pModelCom->Get_CurrentAnimationIndex() + 1, true);
	if (m_pGameInstance->Get_KeyState(KEY_DOWN, DIK_DOWNARROW))
		m_pModelCom->Set_Animation(m_pModelCom->Get_CurrentAnimationIndex() - 1, true);*/
}

HRESULT CChr_Field::Late_Tick(_float fTimeDelta)
{
	if (FAILED(__super::Late_Tick(fTimeDelta)))
		return E_FAIL;

	m_pModelCom->Play_Animation(fTimeDelta);

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
	return S_OK;
}

HRESULT CChr_Field::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; ++i) {
		if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
			return E_FAIL;

		/* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이터를 다 던져야한다. */
		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CChr_Field::Change_State(STATE eState)
{
	m_eState = eState;
	m_pFSMCom->ChangeState(eState);

	return S_OK;
}

void CChr_Field::Change_Animation(ANIMATION_CHR_FIELD iAnimationIndex, _bool isLoop)
{
	m_pModelCom->Set_Animation(iAnimationIndex, isLoop);
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

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pFSMCom);
}
