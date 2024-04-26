#include "stdafx.h"
#include "Sphere.h"

#include "Model.h"
#include "Shader.h"

#include "Effect.h"
#include "Effect_Camera_Look.h"
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
	m_eSkill = pSphere_Desc->eSkill;
	m_pTargetObject = pSphere_Desc->pTargetObject;
	Safe_AddRef(m_pTargetObject);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pSphere_Desc->vStartPosition);

	if (FAILED(Add_Components()))
		return E_FAIL;

	string strEffectFilePath;

	switch (m_eSkill) {
	case CRole::RUIN:	
	case CRole::AERO:
		strEffectFilePath = "../Bin/Resources/Effect/Aero_Projectile.dat";
		break;
	case CRole::WATER:
		strEffectFilePath = "../Bin/Resources/Effect/Water_Projectile.dat";
		break;
	case CRole::FIRE:
		strEffectFilePath = "../Bin/Resources/Effect/Fire_Projectile.dat";
		break;
	case CRole::CURE:
		strEffectFilePath = "../Bin/Resources/Effect/Heal_Projectile.dat";
		break;
	}


	if(FAILED(CEffect::Read_File_Loop(strEffectFilePath, m_pGameInstance, m_pDevice, m_pContext, &m_Effects)))
		return E_FAIL;

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	for (auto& pEffect : m_Effects) {
		pEffect->Set_Position(m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION));
		pEffect->Tick(0);
	}

	return S_OK;
}

void CSphere::Tick(_float fTimeDelta)
{
	Move(fTimeDelta);
	for (auto& pEffect : m_Effects)
		pEffect->Set_Position(m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION));

	if (m_pTargetObject->Get_Dead()) {
		Set_Dead(true);
		for (auto& pEffect : m_Effects)
			pEffect->Set_Dead(true);
		return;
	}

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
	Check_Collision();
}

HRESULT CSphere::Late_Tick(_float fTimeDelta)
{
	if (FAILED(__super::Late_Tick(fTimeDelta)))
		return E_FAIL;

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponents(m_pColliderCom);
#endif

	return S_OK;
}

HRESULT CSphere::Render()
{
	return S_OK;
}

void CSphere::Start()
{
}

HRESULT CSphere::Add_Components()
{
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
		for (auto& pEffect : m_Effects)
			pEffect->Set_Dead(true);

		string strEffectFilePath;

		switch (m_eSkill) {
		case CRole::RUIN:
			strEffectFilePath = "../Bin/Resources/Effect/Ruin_Hit_Camera_Look_Instance.dat";
			break;
		case CRole::AERO:
			strEffectFilePath = "../Bin/Resources/Effect/Aero_Hit_Camera_Look_Instance.dat";
			break;
		case CRole::WATER:
			strEffectFilePath = "../Bin/Resources/Effect/Water_Hit_Camera_Look_Instance.dat";
			break;
		case CRole::FIRE:
			strEffectFilePath = "../Bin/Resources/Effect/Fire_Hit_Camera_Look_Instance.dat";
			break;
		case CRole::CURE:
			strEffectFilePath = "../Bin/Resources/Effect/Heal_Camera_Look_Instance.dat";
			break;
		}

		if (FAILED(CEffect::Read_File_NoLoop(strEffectFilePath, m_pGameInstance, m_pDevice, m_pContext, m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION))))
			return ;

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
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTargetObject);

	for (auto& pEffect : m_Effects)
		Safe_Release(pEffect);
	m_Effects.clear();

}