#include "stdafx.h"
#include "Bullet.h"

#include "Model.h"
#include "Shader.h"
#include "Monster.h"
#include "Chr_Battle.h"
#include "Effect.h"

CBullet::CBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CBullet::CBullet(const CBullet& rhs)
    : CGameObject{ rhs }
{
}

HRESULT CBullet::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBullet::Initialize(void* pArg)
{
	m_eLevel = g_Level;

	GAMEOBJECT_DESC		GameObjectDesc{};

	GameObjectDesc.fSpeedPerSec = 20.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(360.f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	BULLET_DESC* pBullet_Desc = (BULLET_DESC*)pArg;

	m_isTargetMonster = pBullet_Desc->isTargetMonster;
	m_iDamage = pBullet_Desc->iDamage;
	m_fChain = pBullet_Desc->fChain;
	m_pTargetObject = pBullet_Desc->pTargetObject;
	Safe_AddRef(m_pTargetObject);

	m_vPosOffset.x = Random_Float(0.5);
	m_vPosOffset.y = Random_Float(0.5);
	m_vPosOffset.z = Random_Float(0.5);

	//m_pTransformCom->Set_Look();
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pBullet_Desc->vStartPosition);
	m_pTransformCom->Set_Scaled(1.f, 1.f, 10.f);
	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

void CBullet::Tick(_float fTimeDelta)
{
	Move(fTimeDelta);

	if (m_pTargetObject->Get_Dead()) {
		Set_Dead(true);
		return;
	}

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
	Check_Collision();
}

HRESULT CBullet::Late_Tick(_float fTimeDelta)
{
	if (FAILED(__super::Late_Tick(fTimeDelta)))
		return E_FAIL;

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponents(m_pColliderCom);
#endif
	return S_OK;
}

HRESULT CBullet::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i) {
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);

		if (FAILED(m_pShaderCom->Begin(4)))
			return E_FAIL;

		m_pModelCom->Render(i);
	}

	return S_OK;
}

void CBullet::Start()
{
}

HRESULT CBullet::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Bullet"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CBounding_OBB::BOUNDING_OBB_DESC ColliderOBBDesc = {};
	ColliderOBBDesc.vSize = { 0.1f,0.1f,0.15f };
	ColliderOBBDesc.vRotation = {};
	ColliderOBBDesc.vCenter = {0.f,0.f,0.f};

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderOBBDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBullet::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

void CBullet::Move(_float fTimeDelta)
{
	CTransform* pTransformCom = (CTransform*)m_pTargetObject->Get_Component(g_strTransformTag);
	_vector vTargetPos = pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);
	vTargetPos += XMLoadFloat4(&m_vPosOffset);

	if (m_isTargetMonster) {
		vTargetPos.m128_f32[1] += ((CMonster*)m_pTargetObject)->Get_ColliderSize().y * 0.5f;
	}
	else {
		vTargetPos.m128_f32[1] += ((CChr_Battle*)m_pTargetObject)->Get_ColliderSize().y * 0.5f;
	}

	m_pTransformCom->Look_At(vTargetPos);
	m_pTransformCom->Go_Straight(fTimeDelta);
}

void CBullet::Check_Collision()
{
	CCollider* pColliderCom = (CCollider*)m_pTargetObject->Get_Component(TEXT("Com_Collider"));
	if (nullptr == pColliderCom)
		return;

	if (m_pColliderCom->Intersect(pColliderCom)) {

		if (m_isTargetMonster) {
			((CMonster*)m_pTargetObject)->Set_Hit(m_iDamage, m_fChain);

			if (FAILED(CEffect::Read_File_NoLoop("../Bin/Resources/Effect/Fire_Hit_Camera_Look_Instance.dat", m_pGameInstance, m_pDevice, m_pContext, m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION))))
				return;

		}
		else {
			((CChr_Battle*)m_pTargetObject)->Set_Hit(m_iDamage);

			if (FAILED(CEffect::Read_File_NoLoop("../Bin/Resources/Effect/Hit_Particle.dat", m_pGameInstance, m_pDevice, m_pContext, m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION))))
				return;
		}

		Set_Dead(true);
	}
}

CBullet* CBullet::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBullet* pInstance = new CBullet(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBullet"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBullet::Clone(void* pArg)
{
	CBullet* pInstance = new CBullet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CBullet"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBullet::Free()
{
	__super::Free();
	Safe_Release(m_pModelCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTargetObject);
}
