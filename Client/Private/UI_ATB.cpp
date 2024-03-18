#include"stdafx.h"
#include "UI_ATB.h"
#include "Chr_Battle.h"
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

	// 이 부분 수정 들어가야됨
	m_fSizeX = g_iWinSizeX;
	m_fSizeY = g_iWinSizeY;
	m_fX = g_iWinSizeX * 0.5f;
	m_fY = g_iWinSizeY * 0.5f;

	m_pTransformCom->Set_Scaled(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(
		m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f,
		0.f,
		1.f
	));

	m_pPlayerInfo->RegisterObserver(this);

	return S_OK;
}

void CUI_ATB::Tick(_float fTimeDelta)
{
	m_fRatio = m_pPlayerInfo->Get_Leader()->Get_ATB() / m_pPlayerInfo->Get_Leader()->Get_MaxATB();

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

	/* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이터를 다 던져야한다. */
	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

}

HRESULT CUI_ATB::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture_Border", 0);

	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture_Mask", 0);


	return S_OK;
}

void CUI_ATB::OnNotify()
{
}

HRESULT CUI_ATB::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Texture_Logo"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Texture_Logo"),
		TEXT("Com_Texture_UV"), (CComponent**)&m_pTextureUVCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Texture_Logo"),
		TEXT("Com_Texture_Border"), (CComponent**)&m_pTextureBorderCom)))
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
	Safe_Release(m_pTextureUVCom);
	Safe_Release(m_pPlayerInfo);

}
