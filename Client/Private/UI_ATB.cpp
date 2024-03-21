#include"stdafx.h"
#include "UI_ATB.h"
#include "Chr_Battle_Light.h"
#include "Player_Battle.h"

CUI_ATB::CUI_ATB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice , pContext }
{
}

CUI_ATB::CUI_ATB(const CUI_ATB& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_ATB::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_ATB::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	UI_DESC* pUI_Desc = (UI_DESC*)pArg;
	m_pPlayerInfo = (CPlayer_Battle*)pUI_Desc->pObserver_Hander;
	if (nullptr == m_pPlayerInfo)	// 이부분 주의
		return E_FAIL;
	Safe_AddRef(m_pPlayerInfo);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pPlayerInfo->RegisterObserver(this);

	return S_OK;
}

void CUI_ATB::Tick(_float fTimeDelta)
{
	m_fRatio = m_pPlayerInfo->Get_Leader()->Get_ATB() / m_pPlayerInfo->Get_Leader()->Get_MaxATB();

	if (m_fRatio >= m_fCurRatio) {
		m_fCurRatio += fTimeDelta * 5;
		if (m_fCurRatio > m_fRatio) {
			m_fCurRatio = m_fRatio;
		}
	}
	else {
		m_fCurRatio -= fTimeDelta * 5;
		if (m_fCurRatio < m_fRatio) {
			m_fCurRatio = m_fRatio;
		}
	}

	m_fMaskMovement += fTimeDelta/10;
}

HRESULT CUI_ATB::Late_Tick(_float fTimeDelta)
{
	if (FAILED(__super::Late_Tick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_ATB::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	/* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이터를 다 던져야한다. */

	if (FAILED(m_pShaderCom->Begin(2)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(3)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CUI_ATB::Start()
{
	// 이 부분 수정 들어가야됨
	m_fSizeX = 128 * 3;
	m_fSizeY = 10;
	m_pTransformCom->Set_Scaled(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(
		-315.f,  -175.f,
		0.f ,1.f)); 
}

HRESULT CUI_ATB::Bind_ShaderResources()
{
	_float4 vColor = { 0.f,1.f,1.f,1.f };
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0);
	m_pTextureBorderCom->Bind_ShaderResource(m_pShaderCom, "g_Texture_Border", 0);
	m_pTextureMaskCom->Bind_ShaderResource(m_pShaderCom, "g_Texture_Mask", 0);

	if (FAILED(m_pShaderCom->Bind_RawValue("g_SizeX", &m_fSizeX, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_SizeY", &m_fSizeY, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Color", &vColor, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Ratio", &m_fCurRatio, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_MaskMovement", &m_fMaskMovement, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

void CUI_ATB::OnNotify()
{
	//if (m_pPlayerInfo->Get_Stage() == CPlayer_Battle::STAGE_ITEM) {
	//	m_isRender = false;
	//}
	//else {
	//	m_isRender = true;
	//}

}

HRESULT CUI_ATB::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))    
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture_Inner */
	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Texture_UI_ATB_Inner"),
		TEXT("Com_Texture_Inner"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Texture_Border */
	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Texture_UI_ATB_Border"),
		TEXT("Com_Texture_Border"), (CComponent**)&m_pTextureBorderCom)))
		return E_FAIL;

	/* For.Com_Texture_UV */
	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Texture_UI_ATB_Mask"),
		TEXT("Com_Texture_UV"), (CComponent**)&m_pTextureMaskCom)))
		return E_FAIL;

	return S_OK;
}

CUI_ATB* CUI_ATB::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_ATB* pInstance = new CUI_ATB(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_ATB"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CUI* CUI_ATB::Clone(void* pArg)
{
	CUI_ATB* pInstance = new CUI_ATB(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_ATB"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_ATB::Free()
{
	__super::Free();

	Safe_Release(m_pTextureBorderCom);
	Safe_Release(m_pTextureMaskCom);
	Safe_Release(m_pPlayerInfo);

}
