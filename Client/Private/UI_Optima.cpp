#include "stdafx.h"
#include "UI_Optima.h"
#include "Player_Battle.h"

CUI_Optima::CUI_Optima(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice , pContext }
{
}

CUI_Optima::CUI_Optima(const CUI_Optima& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_Optima::Initialize_Prototype()
{
	m_isLateGroup = true;
	return S_OK;
}

HRESULT CUI_Optima::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	UI_DESC* pUI_Desc = (UI_DESC*)pArg;
	m_pPlayerInfo = (CPlayer_Battle*)pUI_Desc->pObserver_Hander;
	if (nullptr == m_pPlayerInfo)	// 이부분 주의
		return E_FAIL;
	Safe_AddRef(m_pPlayerInfo);

	m_pTransformTabCom = CTransform::Create(m_pDevice, m_pContext);
	if (nullptr == m_pTransformTabCom)
		return E_FAIL;

	if (FAILED(m_pTransformTabCom->Initialize(pArg)))
		return E_FAIL;

	m_Components.emplace(TEXT("Com_TransformTab"), m_pTransformTabCom);

	Safe_AddRef(m_pTransformTabCom);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pPlayerInfo->RegisterObserver(this);

	return S_OK;
}

void CUI_Optima::Tick(_float fTimeDelta)
{
	Move(fTimeDelta);
}

HRESULT CUI_Optima::Late_Tick(_float fTimeDelta)
{
	if (FAILED(__super::Late_Tick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Optima::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	/* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이터를 다 던져야한다. */

	if (FAILED(m_pShaderCom->Begin(10)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	if (FAILED(m_pTransformTabCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	if (FAILED(Render_Font()))
		return E_FAIL;

	return S_OK;
}

void CUI_Optima::Start()
{
	m_strOptimaName = m_pPlayerInfo->Get_Current_Optima_Name();
	m_fSizeX = 400;
	m_fSizeY = 50;
	m_vStartPosition = { g_iWinSizeX * -0.5f,-108.f, 0.f };
	m_vTargetPosition = { -440.f, -108.f, 0.f };
	m_pTransformCom->Set_Scaled(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_vTargetPosition), 1.f));

	m_pTransformTabCom->Set_Scaled(24, 24, 1.f);
}

void CUI_Optima::OnNotify()
{
	m_strOptimaName = m_pPlayerInfo->Get_Current_Optima_Name();

	if (m_pPlayerInfo->Get_Stage() == CPlayer_Battle::STAGE_BEGIN ||
		m_pPlayerInfo->Get_Stage() == CPlayer_Battle::STAGE_WAIT_OPTIMA ||
		m_pPlayerInfo->Get_Stage() == CPlayer_Battle::STAGE_FINISH) {
		m_isRender = false;
	}
	else {
		if (m_isRender == false) {
			Reset_Position();
			m_isRender = true;
		}
	}
}

void CUI_Optima::Reset_Position()
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_vStartPosition), 1.f));
	m_fTimeDelta = 0.f;
}

HRESULT CUI_Optima::Bind_ShaderResources()
{
	_float4 vColor = _float4(0.f, 0.f, 0.f, 1.f);
	
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0);

		
	if (FAILED(m_pShaderCom->Bind_RawValue("g_SizeX", &m_fSizeX, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_SizeY", &m_fSizeY, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Color", &vColor, sizeof(_float4))))
		return E_FAIL;


	return S_OK;
}

HRESULT CUI_Optima::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Texture_UI_Optima"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;


	return S_OK;
}

HRESULT CUI_Optima::Render_Font()
{
	if (FAILED(m_pGameInstance->Render_Font(g_strFont14Tag, m_strOptimaName, m_vFont_OptimaPosition, XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Render_Font(g_strFont14Tag, TEXT("OPTIMA CHANGE"), m_vFont_StrPosition, XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f)))
		return E_FAIL;

	return S_OK;
}

void CUI_Optima::Move(_float fTimeDelta)
{
	if (m_fTimeDelta >= 1.f) {
		return;
	}

	m_fTimeDelta += fTimeDelta;
	if (m_fTimeDelta >= 1.0f) {
		m_fTimeDelta = 1.0f;
	}

	_float4 vCurPos = {};
	XMStoreFloat4(&vCurPos, XMVectorSetW(XMVectorLerp(m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION), XMLoadFloat3(&m_vTargetPosition), m_fTimeDelta), 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCurPos);

	_float4 vTabPos = { vCurPos.x - 80.f, vCurPos.y - 10.f, 0.f, 1.f };
	m_pTransformTabCom->Set_State(CTransform::STATE_POSITION, vTabPos);

	// 폰트 위치 지정
	// 1. vCurPos -> WIndow 좌표계로 변경 직교투영이므로 winsize만 뺴면됨
	// { vPosition.x + (g_iWinSizeX - m_fSizeX) * 0.5f + 20 , -vPosition.y + 2 + (g_iWinSizeY - m_fSizeY) * 0.5f };

	vCurPos.x += g_iWinSizeX * 0.5f - 55.f;
	vCurPos.y = -vCurPos.y + g_iWinSizeY * 0.5f;
	m_vFont_OptimaPosition = { vCurPos.x + 10.f, vCurPos.y - 22.f };
	m_vFont_StrPosition = { vCurPos.x, vCurPos.y  };
}

CUI_Optima* CUI_Optima::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Optima* pInstance = new CUI_Optima(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Optima"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CUI_Optima* CUI_Optima::Clone(void* pArg)
{
	CUI_Optima* pInstance = new CUI_Optima(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Optima"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Optima::Free()
{
	__super::Free();
	Safe_Release(m_pTransformTabCom);
	Safe_Release(m_pPlayerInfo);
}
