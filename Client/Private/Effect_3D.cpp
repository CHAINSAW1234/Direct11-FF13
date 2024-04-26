#include "stdafx.h"
#include "Effect_3D.h"

CEffect_3D::CEffect_3D(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect{ pDevice, pContext }
{
}

CEffect_3D::CEffect_3D(const CEffect& rhs)
	: CEffect{ rhs }
{
}

HRESULT CEffect_3D::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect_3D::Initialize(void* pArg)
{
	m_eType = EFFECT_3D;

	if (nullptr == pArg)
		return E_FAIL;

	EFFECT_3D_DESC* pDesc = (EFFECT_3D_DESC*)pArg;
	m_strModelTag = pDesc->strModelTag;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	return S_OK;
}

void CEffect_3D::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

HRESULT CEffect_3D::Late_Tick(_float fTimeDelta)
{
	if (FAILED(__super::Late_Tick(fTimeDelta)))
		return E_FAIL;

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
	return S_OK;
}

HRESULT CEffect_3D::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();


	// 1. 텍스처를 사용한다
	// 마스크도 사용한다
	// 1. 둘다 움직인다
	// 2. 텍스처만 움직인다
	// 3. 마스크만 움직인다


	if (999 == m_iDiffuseTextureIndex) {
		if (FAILED(m_pShaderCom->Begin(1)))		// 텍스처 없음 : 1
			return E_FAIL;
	}
	else if (999 != m_iMaskTextureIndex) {
		if (999 != m_iDissolveTextureIndex) {
			if (FAILED(m_pShaderCom->Begin(4)))	// 텍스처 3개 사용
				return E_FAIL;
		}
		else {
			if (FAILED(m_pShaderCom->Begin(2)))	// dissolve 사용 X
				return E_FAIL;
		}
	}
	else {
		if (999 != m_iDissolveTextureIndex) {
			if (FAILED(m_pShaderCom->Begin(3)))	// disslove는 사용
				return E_FAIL;
		}
		else {
			if (FAILED(m_pShaderCom->Begin(0)))
				return E_FAIL;
		}
	}

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		//if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
		//	return E_FAIL;

		m_pModelCom->Render(i);
	}

	return S_OK;
}

void CEffect_3D::Start()
{
}

HRESULT CEffect_3D::Save_Effect(ofstream& OFS)
{
	__super::Save_Effect(OFS);

	size_t strModelTagSize = m_strModelTag.size();
	OFS.write(reinterpret_cast<const char*>(&strModelTagSize), sizeof(size_t));
	OFS.write(reinterpret_cast<const char*>(m_strModelTag.c_str()), sizeof(_tchar) * strModelTagSize);

	return S_OK;
}

HRESULT CEffect_3D::Initialize_Load(ifstream& IFS)
{
	m_eType = EFFECT_3D;
	if (FAILED(Load_Effect(IFS)))
		return E_FAIL;

	GAMEOBJECT_DESC		GameObjectDesc{};

	GameObjectDesc.fSpeedPerSec = 10.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(360.f);

	if (FAILED(CGameObject::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_3D::Load_Effect(ifstream& IFS)
{
	__super::Load_Effect(IFS);

	size_t strModelTagSize = 0;
	IFS.read(reinterpret_cast<char*>(&strModelTagSize), sizeof(size_t));
	vector<_tchar> vecBuffer(strModelTagSize);

	IFS.read(reinterpret_cast<char*>(vecBuffer.data()), sizeof(_tchar) * strModelTagSize);
	m_strModelTag = wstring(vecBuffer.begin(), vecBuffer.end());

	return S_OK;
}

HRESULT CEffect_3D::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel_Effect"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, m_strModelTag,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;
	
	/* For.Com_DiffuseTexture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Diffuse"),
		TEXT("Com_DiffuseTexture"), (CComponent**)&m_pDiffuseTextureCom)))
		return E_FAIL;

	/* For.Com_MaskTexture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Diffuse"),
		TEXT("Com_MaskTexture"), (CComponent**)&m_pMaskTextureCom)))
		return E_FAIL;

	/* For.Com_DissolveTexture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Mask"),
		TEXT("Com_DissolveTexture"), (CComponent**)&m_pDissolveTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_3D::Bind_ShaderResources()
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

	if (nullptr != m_pMaskTextureCom && m_iMaskTextureIndex != 999) {
		if (FAILED(m_pMaskTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", m_iMaskTextureIndex)))
			return E_FAIL;
	}

	if (nullptr != m_pDissolveTextureCom && m_iDissolveTextureIndex != 999) {
		if (FAILED(m_pDissolveTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture", m_iDissolveTextureIndex)))
			return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fColorMagnification", &m_fColorMagnification, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vMaskStartValue", &m_vMaskValue[0], sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vMaskEndValue", &m_vMaskValue[1], sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_TextureMovement", &m_fTextureTimeDelta, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_DissolveTime", &m_fDissolveTimeDelta, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}


CEffect_3D* CEffect_3D::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect_3D* pInstance = new CEffect_3D(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CEffect_3D"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CEffect_3D* CEffect_3D::Clone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ifstream& IFS)
{
	CEffect_3D* pInstance = new CEffect_3D(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Load(IFS))) {

		MSG_BOX(TEXT("Failed To Load : CEffect_3D"));

		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;;
}

CGameObject* CEffect_3D::Clone(void* pArg)
{
	CEffect_3D* pInstance = new CEffect_3D(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CEffect_3D"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_3D::Free()
{
	CEffect::Free();
	Interface_3D::Free();
}
