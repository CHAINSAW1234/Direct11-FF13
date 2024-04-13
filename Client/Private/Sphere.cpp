#include "stdafx.h"
#include "Sphere.h"

#include "Model.h"
#include "Shader.h"

#include "Monster.h"
#include "Chr_Battle.h"

CSphere::CSphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CSphere::CSphere(const CSphere& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CSphere::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSphere::Initialize(void* pArg)
{
	m_eLevel = g_Level;

	GAMEOBJECT_DESC		GameObjectDesc{};

	GameObjectDesc.fSpeedPerSec = 10.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(360.f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	SPHERE_DESC* pSphere_Desc = (SPHERE_DESC*)pArg;

	m_isTargetMonster = pSphere_Desc->isTargetMonster;
	m_iDamage = pSphere_Desc->iDamage;
	m_fChain = pSphere_Desc->fChain;

	m_pTargetObject = pSphere_Desc->pTargetObject;
	Safe_AddRef(m_pTargetObject);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pSphere_Desc->vStartPosition);

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

void CSphere::Tick(_float fTimeDelta)
{
	Move(fTimeDelta);
	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
	Check_Collision();

	if (m_pTargetObject->Get_Dead()) {
		Set_Dead(true);
	}
}

HRESULT CSphere::Late_Tick(_float fTimeDelta)
{
	if (FAILED(__super::Late_Tick(fTimeDelta)))
		return E_FAIL;

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
	return S_OK;
}

HRESULT CSphere::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i) {
		//m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);

		if (FAILED(m_pShaderCom->Begin(2)))
			return E_FAIL;

		m_pModelCom->Render(i);
	}

#ifdef _DEBUG
	m_pColliderCom->Render();
#endif

	return S_OK;
}

void CSphere::Start()
{
}

HRESULT CSphere::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Model_Sphere"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CBounding_Sphere::BOUNDING_SPHERE_DESC ColliderSphereDesc = {};
	ColliderSphereDesc.fRadius = .1f;
	ColliderSphereDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderSphereDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSphere::Bind_ShaderResources()
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

void CSphere::Move(_float fTimeDelta)
{
	CTransform* pTransformCom = (CTransform*)m_pTargetObject->Get_Component(g_strTransformTag);
	_vector vTargetPos = pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);

	if (m_isTargetMonster) {
		vTargetPos.m128_f32[1] += ((CMonster*)m_pTargetObject)->Get_ColliderSize().y * 0.5f;
	}
	else {
		vTargetPos.m128_f32[1] += ((CChr_Battle*)m_pTargetObject)->Get_ColliderSize().y * 0.5f;
	}

	m_pTransformCom->Look_At(vTargetPos);
	m_pTransformCom->Go_Straight(fTimeDelta);
}

void CSphere::Check_Collision()
{
	CCollider* pColliderCom = (CCollider*)m_pTargetObject->Get_Component(TEXT("Com_Collider"));
	if (nullptr == pColliderCom)
		return;

	if (m_pColliderCom->Intersect(pColliderCom)) {

		if (m_isTargetMonster) {
			((CMonster*)m_pTargetObject)->Set_Hit(m_iDamage, m_fChain);
		}
		else {
			((CChr_Battle*)m_pTargetObject)->Set_Hit(m_iDamage);
		}

		Set_Dead(true);
	}
}

CSphere* CSphere::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSphere* pInstance = new CSphere(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CSphere"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSphere::Clone(void* pArg)
{
	CSphere* pInstance = new CSphere(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CSphere"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSphere::Free()
{
	__super::Free();
	Safe_Release(m_pModelCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTargetObject);
}
