#include "stdafx.h"
#include "Corpse.h"

CCorpse::CCorpse(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CCorpse::CCorpse(const CCorpse& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CCorpse::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCorpse::Initialize(void* pArg)
{
	m_eLevel = g_Level;

	GAMEOBJECT_DESC		GameObjectDesc{};

	GameObjectDesc.fSpeedPerSec = 10.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(360.f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	CORPSE_DESC* pDesc = (CORPSE_DESC*)pArg;

	m_pModelCom = pDesc->pModelCom;
	m_pTransformCom->Set_WorldMatrix(pDesc->WorldMatrix);

	Safe_AddRef(m_pModelCom);

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CCorpse::Tick(_float fTimeDelta)
{
	m_fTimeDelta += fTimeDelta/2;

	if (m_fTimeDelta >= 1.f)
		Set_Dead(true);
}

HRESULT CCorpse::Late_Tick(_float fTimeDelta)
{
	if (FAILED(__super::Late_Tick(fTimeDelta)))
		return E_FAIL;

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);

	return S_OK;
}

HRESULT CCorpse::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
			return E_FAIL;

		/* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이ㅏ터를 다 던져야한다. */
		if (FAILED(m_pShaderCom->Begin(2)))
			return E_FAIL;

		m_pModelCom->Render(i);
	}

	return S_OK;
}

void CCorpse::Start()
{
}

HRESULT CCorpse::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Texture_Corpse"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCorpse::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_DissolveTime", &m_fTimeDelta, sizeof(_float))))
		return E_FAIL;


	return S_OK;
}

CCorpse* CCorpse::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCorpse* pInstance = new CCorpse(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CCorpse"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCorpse::Clone(void* pArg)
{
	CCorpse* pInstance = new CCorpse(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CCorpse"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCorpse::Free()
{
	__super::Free();
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
}
