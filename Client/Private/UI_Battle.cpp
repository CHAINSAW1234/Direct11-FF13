#include "stdafx.h"
#include "UI_Battle.h"

CUI_Battle::CUI_Battle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice , pContext }
{
}

CUI_Battle::CUI_Battle(const CUI_Battle& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_Battle::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Battle::Initialize(void* pArg)
{
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

	return S_OK;
}

void CUI_Battle::Tick(_float fTimeDelta)
{
}

HRESULT CUI_Battle::Late_Tick(_float fTimeDelta)
{
	if (FAILED(__super::Late_Tick(fTimeDelta)))
		return E_FAIL;
	return S_OK;

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this);
}

HRESULT CUI_Battle::Render()
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

	return S_OK;
}

void CUI_Battle::OnNotify()
{
}

HRESULT CUI_Battle::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Battle::Add_Components()
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;
	
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Texture_***"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

CUI_Battle* CUI_Battle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Battle* pInstance = new CUI_Battle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Battle"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CUI* CUI_Battle::Clone(void* pArg)
{
	CUI_Battle* pInstance = new CUI_Battle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Battle"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Battle::Free()
{
	__super::Free();

}
