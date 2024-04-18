#include "stdafx.h"
#include "Effect_Instance.h"

CEffect_Instance::CEffect_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect{ pDevice, pContext }
{
}

CEffect_Instance::CEffect_Instance(const CEffect& rhs)
	: CEffect{ rhs }
{
}

HRESULT CEffect_Instance::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect_Instance::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	EFFECT_INSTANCE_DESC* pDesc = (EFFECT_INSTANCE_DESC*)pArg;
//	m_strModelTag = pDesc->strModelTag;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CEffect_Instance::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

HRESULT CEffect_Instance::Late_Tick(_float fTimeDelta)
{
	if (FAILED(__super::Late_Tick(fTimeDelta)))
		return E_FAIL;

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
	return S_OK;
}

HRESULT CEffect_Instance::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CEffect_Instance::Start()
{
}

HRESULT CEffect_Instance::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxInstance"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_DiffuseTexture */
	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Texture_Instance"),
		TEXT("Com_Texture"), (CComponent**)&m_pDiffuseTextureCom)))
		return E_FAIL;

	///* For.Com_MaskTexture */
	//if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Texture_Mask"),
	//	TEXT("Com_MaskTexture"), (CComponent**)&m_pMaskTextureCom)))
	//	return E_FAIL;

	/* For.Com_DissolveTexture */
	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Texture_Mask"),
		TEXT("Com_DissolveTexture"), (CComponent**)&m_pDissolveTextureCom)))
		return E_FAIL;


	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(g_Level, L"Prototype_Component_VIBuffer_Instance_" + m_strBufferTag,
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Instance::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (nullptr != m_pDiffuseTextureCom && m_iDiffuseTextureIndex != 999) {

		if (FAILED(m_pDiffuseTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", m_iDiffuseTextureIndex)))
			return E_FAIL;
	}

	if (nullptr != m_pDissolveTextureCom && m_iDissolveTextureIndex != 999) {
		if (FAILED(m_pDissolveTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture", m_iDissolveTextureIndex)))
			return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_DissolveTime", &m_fDissolveTimeDelta, sizeof(_float))))
		return E_FAIL;
	return S_OK;
}

CEffect_Instance* CEffect_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect_Instance* pInstance = new CEffect_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CEffect_Instance"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_Instance::Clone(void* pArg)
{
	CEffect_Instance* pInstance = new CEffect_Instance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CEffect_Instance"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Instance::Free()
{
	CEffect::Free();
	Interface_Instance::Free();
}
