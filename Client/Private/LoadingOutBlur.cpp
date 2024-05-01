#include "stdafx.h"
#include "LoadingOutBlur.h"

#include "Texture.h"

CLoadingOutBlur::CLoadingOutBlur(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CLoadingOutBlur::CLoadingOutBlur(const CLoadingOutBlur& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CLoadingOutBlur::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLoadingOutBlur::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));

	m_pTransformCom->Set_Scaled(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	return S_OK;
}

void CLoadingOutBlur::Tick(_float fTimeDelta)
{
	m_fTimeDelta += fTimeDelta;
	if (m_fTimeDelta >= .5f)
		Set_Dead(true);
}

HRESULT CLoadingOutBlur::Late_Tick(_float fTimeDelta)
{
	if (FAILED(__super::Late_Tick(fTimeDelta)))
		return E_FAIL;

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI_LATE, this);

	return S_OK;
}

HRESULT CLoadingOutBlur::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(6)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CLoadingOutBlur::Start()
{
	m_pGameInstance->Tick_Engine(0.001f);
	
	m_pGameInstance->Begin_Draw(_float4(1.f, 1.f, 1.f, 1.f));

	m_pGameInstance->Draw();

	m_pGameInstance->End_Draw();

	m_pGameInstance->Save_BackBuffer(TEXT("../Bin/DataFiles/Blur.dds"));

	m_pTextureCom = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Blur.dds"));

	if (nullptr == m_pTextureCom)
		return ;
}

HRESULT CLoadingOutBlur::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoadingOutBlur::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (nullptr == m_pTextureCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_TimeDelta", &m_fTimeDelta, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CLoadingOutBlur* CLoadingOutBlur::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLoadingOutBlur* pInstance = new CLoadingOutBlur(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CLoadingOutBlur"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLoadingOutBlur::Clone(void* pArg)
{
	CLoadingOutBlur* pInstance = new CLoadingOutBlur(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CLoadingOutBlur"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoadingOutBlur::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
}
