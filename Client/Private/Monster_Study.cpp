#include "stdafx.h"
#include "Monster_Study.h"

CMonster_Study::CMonster_Study(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CMonster_Study::CMonster_Study(const CMonster_Study& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CMonster_Study::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMonster_Study::Initialize(void* pArg)
{
	m_eLevel = g_Level;

	GAMEOBJECT_DESC		GameObjectDesc{};

	GameObjectDesc.fSpeedPerSec = 10.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pModelCom->Set_Animation(rand() % 20, true);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(_float(rand() % 20), 0.f, _float(rand() % 20), 1.f));

	return S_OK;
}

void CMonster_Study::Tick(_float fTimeDelta)
{
	; 
}

HRESULT CMonster_Study::Late_Tick(_float fTimeDelta)
{
	if (FAILED(__super::Late_Tick(fTimeDelta)))
		return E_FAIL;

	for (auto& pColliderCom : m_pColliderCom)
		pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());	

	m_pColliderCom[COLLIDER_HEAD]->Intersect((CCollider*)m_pGameInstance->Get_Component(g_Level, TEXT("Layer_Player"), TEXT("Com_Collider")));


	m_pModelCom->Play_Animation(fTimeDelta);

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
	return S_OK;
}

HRESULT CMonster_Study::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i) {
		if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
			return E_FAIL;

		/* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이터를 다 던져야한다. */
		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		m_pModelCom->Render(i);
	}

#ifdef _DEBUG
	for (auto& pColliderCom : m_pColliderCom)
		pColliderCom->Render();
#endif

	return S_OK;
}

HRESULT CMonster_Study::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Model_Fiona"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;



	/* Com_Collider_Head */
	CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc{};

	/* 로컬상의 정보를 셋팅한다. */
	ColliderDesc.fRadius = 0.32f;
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.fRadius + 0.6f, 0.f);


	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Head"), (CComponent**)&m_pColliderCom[COLLIDER_HEAD], &ColliderDesc)))
		return E_FAIL;

	/* Com_Collider_Body */
	CBounding_OBB::BOUNDING_OBB_DESC		ColliderOBBDesc{};

	/* 로컬상의 정보를 셋팅한다. */
	ColliderOBBDesc.vRotation = _float3(0.f, XMConvertToRadians(45.0f), 0.f);
	ColliderOBBDesc.vSize = _float3(0.8f, 0.6f, 0.8f);
	ColliderOBBDesc.vCenter = _float3(0.f, ColliderOBBDesc.vSize.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_Body"), (CComponent**)&m_pColliderCom[COLLIDER_BODY], &ColliderOBBDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_Study::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
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

CMonster_Study* CMonster_Study::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonster_Study* pInstance = new CMonster_Study(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMonster_Study"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CMonster_Study::Clone(void* pArg)
{
	CMonster_Study* pInstance = new CMonster_Study(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CMonster_Study"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster_Study::Free()
{
	__super::Free();

	for (auto& pColliderCom : m_pColliderCom)
		Safe_Release(pColliderCom);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
