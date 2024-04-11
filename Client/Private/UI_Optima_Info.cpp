#include "stdafx.h"
#include "UI_Optima_Info.h"
#include "Player_Battle.h"

CUI_Optima_Info::CUI_Optima_Info(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice , pContext }
{
}

CUI_Optima_Info::CUI_Optima_Info(const CUI_Optima_Info& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_Optima_Info::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Optima_Info::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	UI_OPTIMA_INFO_DESC* pUI_Optima_Info_Desc = (UI_OPTIMA_INFO_DESC*)pArg;

	m_vStartPosition = pUI_Optima_Info_Desc->vStartPosition;
	m_vTargetPosition = pUI_Optima_Info_Desc->vTargetPosition;
	m_iChrNum = pUI_Optima_Info_Desc->iChrNum;


	m_pTransformFaceCom = CTransform::Create(m_pDevice, m_pContext);
	if (nullptr == m_pTransformFaceCom)
		return E_FAIL;

	if (FAILED(m_pTransformFaceCom->Initialize(pArg)))
		return E_FAIL;

	m_Components.emplace(TEXT("Com_TransformFace"), m_pTransformFaceCom);

	Safe_AddRef(m_pTransformFaceCom);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CUI_Optima_Info::Tick(_float fTimeDelta)
{
	Move(fTimeDelta);
}

HRESULT CUI_Optima_Info::Late_Tick(_float fTimeDelta)
{
	if (FAILED(__super::Late_Tick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Optima_Info::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	/* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이터를 다 던져야한다. */

	if (FAILED(m_pShaderCom->Begin(11)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	m_pTextureFaceCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iChrNum);
	if (FAILED(m_pTransformFaceCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	if (FAILED(Render_Font()))
		return E_FAIL;

	return S_OK;
}

void CUI_Optima_Info::Start()
{

	m_fSizeX = 450;
	m_fSizeY = 42;
	m_pTransformCom->Set_Scaled(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_vTargetPosition), 1.f));
	m_pTransformFaceCom->Set_Scaled(86, m_fSizeY, 1.f);
}

void CUI_Optima_Info::Set_Optima(CAbility::ROLE eRole)
{
	m_strRoleName = CAbility::Get_RoleFullName(eRole);
	m_vRoleColor = CAbility::Get_RoleColor(eRole);
}

void CUI_Optima_Info::Reset_Position()
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_vStartPosition), 1.f));
	m_fTimeDelta = 0.f;
}

HRESULT CUI_Optima_Info::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0);

	if (FAILED(m_pShaderCom->Bind_RawValue("g_SizeX", &m_fSizeX, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_SizeY", &m_fSizeY, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Optima_Info::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture_Face */
	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Texture_UI_Optima_Info"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Texture_Face */
	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Texture_UI_Optima_Info_Face"),
		TEXT("Com_Texture_Face"), (CComponent**)&m_pTextureFaceCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Optima_Info::Render_Font()
{
	if (FAILED(m_pGameInstance->Render_Font(g_strFontEng32Tag, to_wstring(m_iChrNum+1), m_vFont_NumPosition, XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Render_Font(g_strFontAlphaTag, m_strRoleName, m_vFont_RolePosition, XMLoadFloat4(&m_vRoleColor), 0.f)))
		return E_FAIL;

	return S_OK;
}

void CUI_Optima_Info::Move(_float fTimeDelta)
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

	_float4 vFacePos = { vCurPos.x - 120.f, vCurPos.y, 0.f, 1.f };
	m_pTransformFaceCom->Set_State(CTransform::STATE_POSITION, vFacePos);

	// 폰트 위치 지정
	// 1. vCurPos -> WIndow 좌표계로 변경 직교투영이므로 winsize만 뺴면됨
	// { vPosition.x + (g_iWinSizeX - m_fSizeX) * 0.5f + 20 , -vPosition.y + 2 + (g_iWinSizeY - m_fSizeY) * 0.5f };

	vCurPos.x += g_iWinSizeX * 0.5f;
	vCurPos.y = -vCurPos.y + g_iWinSizeY * 0.5f;
	m_vFont_NumPosition = { vCurPos.x - m_fSizeX * 0.5f + 15.f, vCurPos.y - 14.f};
	m_vFont_RolePosition = { vCurPos.x - 85.f , vCurPos.y - 9.f };
}

CUI_Optima_Info* CUI_Optima_Info::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Optima_Info* pInstance = new CUI_Optima_Info(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Optima_Info"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CUI_Optima_Info* CUI_Optima_Info::Clone(void* pArg)
{
	CUI_Optima_Info* pInstance = new CUI_Optima_Info(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Optima_Info"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Optima_Info::Free()
{
	__super::Free();
	Safe_Release(m_pTransformFaceCom);
	Safe_Release(m_pTextureFaceCom);
}
