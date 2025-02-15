#include"stdafx.h"
#include "Solider.h"

#include "FSM.h"
#include "Solider_State_Idle.h"
#include "Solider_State_Attack.h"
#include "Solider_State_Hit.h"
#include "Solider_State_Field.h"
#include "Solider_State_Start.h"

#include "Weapon_Anim.h"
#include "Chr_Battle.h"
#include "Corpse.h"

CSolider::CSolider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster{ pDevice, pContext }
{
}

CSolider::CSolider(const CSolider& rhs)
	: CMonster{ rhs }
{
}

HRESULT CSolider::Initialize_Prototype()
{
	m_iMaxHp = m_iHp = 1296;
	m_fStagger = 200.f;
	m_fChainResist = 50.f;
	m_iDamage = 64;
	m_vColliderSize = _float3(.8f, 1.6f, 1.f);
	m_strMonsterName = TEXT("PSICOM 특무병");

	return S_OK;
}

HRESULT CSolider::Initialize(void* pArg)
{
	GAMEOBJECT_DESC		GameObjectDesc{};

	GameObjectDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(360.f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(Add_PartObjects()))
		return E_FAIL;

	return S_OK;
}

void CSolider::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	m_pWeapon->Tick(fTimeDelta);
}

HRESULT CSolider::Late_Tick(_float fTimeDelta)
{
	if (FAILED(__super::Late_Tick(fTimeDelta)))
		return E_FAIL;

	if (FAILED(m_pWeapon->Late_Tick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSolider::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CSolider::Start()
{
	if (g_Level == LEVEL_EFFECTTOOL) {
		Change_State(STATE_START);
		return;
	}


	m_pTargetObject = dynamic_cast<CChr_Battle*>(m_pGameInstance->Get_GameObject(g_Level, g_strChrLayerTag, 0));
	m_pNavigationCom->Set_Index(m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION));
	//Safe_AddRef(m_pTargetObject);
	if (g_Level == LEVEL_BATTLE) {
		Change_State(STATE_IDLE);
		Change_Animation(BATTLE_IDLE, true);
	}
	else 
		Change_State(STATE_FIELD);

}

_float4 CSolider::Get_BonePos(const string strBoneName)
{
	_float4 vPos;
	XMStoreFloat4(&vPos, XMLoadFloat4x4(m_pWeapon->Get_WorldMatrix_Ptr()).r[3]);
	return vPos;
}

void CSolider::Set_Hit(_int iDamage, _float fChain)
{
	__super::Set_Hit(iDamage, fChain);

	if (m_iHp <= 0) {
		Set_Dead(true);

		CCorpse::CORPSE_DESC pDesc = {};
		pDesc.pModelCom = (CModel*)m_pWeapon->Get_Component(TEXT("Com_Model"));

		pDesc.WorldMatrix = *m_pWeapon->Get_WorldMatrix_Ptr();

		if (FAILED(m_pGameInstance->Add_Clone(g_Level, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Corpse"), &pDesc)))
			return;
	}

	Change_State(STATE_HIT);
	Set_TrackPosition(0.f);
}

void CSolider::Set_State_Battle_Start()
{
	Change_State(STATE_START);
}

HRESULT CSolider::Change_State(STATE eState)
{
	m_eState = eState;
	m_pFSMCom->Change_State(eState);
	return S_OK;
}

void CSolider::Change_Animation(ANIMATION_SOLIDER iAnimationIndex, _bool isLoop)
{
	m_pModelCom->Set_Animation(iAnimationIndex, isLoop);
}

void CSolider::PlaySound_Attack()
{
	m_pGameInstance->PlaySoundDuplicate(TEXT("Solider_Attack.wav"), CSound_Manager::EFFECT_DUPLICATE, SOUND_DEFAULT);
}

HRESULT CSolider::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Solider"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* Com_Collider_Body */
	CBounding_OBB::BOUNDING_OBB_DESC		ColliderOBBDesc{};

	/* 로컬상의 정보를 셋팅한다. */
	ColliderOBBDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColliderOBBDesc.vSize = m_vColliderSize;
	ColliderOBBDesc.vCenter = _float3(0.f, ColliderOBBDesc.vSize.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderOBBDesc)))
		return E_FAIL;

	CBounding_Sphere::BOUNDING_SPHERE_DESC ColliderSphereDesc = {};
	ColliderSphereDesc.fRadius = .5f;
	ColliderSphereDesc.vCenter = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Push"), (CComponent**)&m_pCollider_PushCom, &ColliderSphereDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSolider::Add_Component_FSM()
{
	if (FAILED(__super::Add_Component_FSM()))
		return E_FAIL;

	m_pFSMCom->Add_State(STATE_IDLE, CSolider_State_Idle::Create(this));
	m_pFSMCom->Add_State(STATE_ATTACK, CSolider_State_Attack::Create(this));
	m_pFSMCom->Add_State(STATE_HIT, CSolider_State_Hit::Create(this));
	m_pFSMCom->Add_State(STATE_FIELD, CSolider_State_Field::Create(this));
	m_pFSMCom->Add_State(STATE_START, CSolider_State_Start::Create(this));

	return S_OK;
}

HRESULT CSolider::Add_PartObjects()
{
	/* For.Part_Weapon */
	CWeapon_Anim::WEAPON_ANIM_DESC	WeaponDesc{};

	WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4_Ptr();
	WeaponDesc.pSocket = m_pModelCom->Get_BonePtr("R_weapon");
	WeaponDesc.strModelTag = TEXT("Prototype_Component_Model_Solider_Weapon");
	WeaponDesc.Bounding_OBB_Desc.vSize = { 0.2f, 1.f,.2f };
	WeaponDesc.Bounding_OBB_Desc.vCenter = { 0.f, 0.4f, 0.f };

	WeaponDesc.Bounding_OBB_Desc.vRotation = { 0.f,0.f,0.f };

	m_pWeapon = dynamic_cast<CWeapon_Anim*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Part_Weapon_Anim"), &WeaponDesc));
	if (nullptr == m_pWeapon)
		return E_FAIL;

	m_pCollider_WeaponCom = m_pWeapon->Get_Collider();
	Safe_AddRef(m_pCollider_WeaponCom);
	return S_OK;
}

CSolider* CSolider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSolider* pInstance = new CSolider(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CSolider"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSolider::Clone(void* pArg)
{
	CSolider* pInstance = new CSolider(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CSolider"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSolider::Free()
{
	__super::Free();
	Safe_Release(m_pWeapon);
}
