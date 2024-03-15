#include "stdafx.h"
#include "Weapon_Study.h"

#include "Bone.h"

CWeapon_Study::CWeapon_Study(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CWeapon_Study::CWeapon_Study(const CWeapon_Study& rhs)
	: CPartObject{ rhs }
{

}

HRESULT CWeapon_Study::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWeapon_Study::Initialize(void* pArg)
{
	WEAPON_STUDY_DESC* pWeaponStudyDesc = (WEAPON_STUDY_DESC*)pArg;

	m_pSocket = pWeaponStudyDesc->pSocket;
	Safe_AddRef(m_pSocket);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scaled(0.1f, 0.1f, 0.1f);
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.0f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float4(0.8f, 0.f, 0.f, 1.f));

	return S_OK;
}

void CWeapon_Study::Tick(_float fTimeDelta)
{

}

HRESULT CWeapon_Study::Late_Tick(_float fTimeDelta)
{
	if (FAILED(__super::Late_Tick(fTimeDelta)))
		return S_OK;
	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(m_pSocket->Get_CombinedTransformationMatrix()) * XMLoadFloat4x4(m_pParentMatrix));

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

	return S_OK;
}

HRESULT CWeapon_Study::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		/* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이ㅏ터를 다 던져야한다. */
		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		m_pModelCom->Render(i);
	}


	return S_OK;
}

HRESULT CWeapon_Study::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_ForkLift"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CWeapon_Study::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
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

CWeapon_Study* CWeapon_Study::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeapon_Study* pInstance = new CWeapon_Study(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CWeapon_Study"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CWeapon_Study::Clone(void* pArg)
{
	CWeapon_Study* pInstance = new CWeapon_Study(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CWeapon_Study"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon_Study::Free()
{
	__super::Free();

	Safe_Release(m_pSocket);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
