#include "stdafx.h"
#include "Solider_Gun.h"

#include "FSM.h"
#include "Solider_State_Idle.h"
#include "Solider_Gun_State_Attack.h"
#include "Solider_State_Hit.h"
#include "Solider_State_Field.h"
#include "Solider_State_Start.h"

#include "Weapon_Anim.h"
#include "Chr_Battle.h"
#include "Bullet.h"

CSolider_Gun::CSolider_Gun(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CSolider{ pDevice, pContext }
{
}

CSolider_Gun::CSolider_Gun(const CSolider_Gun& rhs)
    : CSolider{ rhs }
{
}

HRESULT CSolider_Gun::Initialize_Prototype()
{
    m_iMaxHp = m_iHp = 324;
    m_fStagger = 200.f;
    m_fChainResist = 50.f;
    m_iDamage = 45;
    m_vColliderSize = _float3(.8f, 1.6f, 1.f);
    m_strMonsterName = TEXT("PSICOM 특무엽병");

    return S_OK;
}

void CSolider_Gun::Start()
{
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

void CSolider_Gun::Create_Bullet()
{
    _float4 vPos = m_pWeapon->Get_BonePosition("gatling");                                                                                                                                                                                       

    CBullet::BULLET_DESC Bullet_Desc = {};
    Bullet_Desc.pTargetObject = m_pTargetObject;
    Bullet_Desc.isTargetMonster = false;
    Bullet_Desc.vStartPosition = vPos;
    Bullet_Desc.iDamage = m_iDamage/3;
    Bullet_Desc.fChain = 5.f;

    m_pGameInstance->Add_Clone(g_Level, TEXT("Layer_Bullet"), TEXT("Prototype_GameObject_Bullet"), &Bullet_Desc);
    m_pGameInstance->PlaySoundDuplicate(TEXT("Solider_Gun_Shot.wav"), CSound_Manager::EFFECT_DUPLICATE, SOUND_DEFAULT);
    return;
}

HRESULT CSolider_Gun::Add_Components()
{
    /* For.Com_Model */
    if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Solider_Gun"),
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

    if (FAILED(CMonster::Add_Components()))
        return E_FAIL;

    return S_OK;
}

HRESULT CSolider_Gun::Add_Component_FSM()
{
    if (FAILED(CMonster::Add_Component_FSM()))
        return E_FAIL;

    m_pFSMCom->Add_State(STATE_IDLE, CSolider_State_Idle::Create(this));
    m_pFSMCom->Add_State(STATE_ATTACK, CSolider_Gun_State_Attack::Create(this));
    m_pFSMCom->Add_State(STATE_HIT, CSolider_State_Hit::Create(this));
    m_pFSMCom->Add_State(STATE_FIELD, CSolider_State_Field::Create(this));
    m_pFSMCom->Add_State(STATE_START, CSolider_State_Start::Create(this));

    return S_OK;
}

HRESULT CSolider_Gun::Add_PartObjects()
{
    /* For.Part_Weapon */
    CWeapon_Anim::WEAPON_ANIM_DESC	WeaponDesc{};

    WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4_Ptr();
    WeaponDesc.pSocket = m_pModelCom->Get_BonePtr("R_weapon");
    WeaponDesc.strModelTag = TEXT("Prototype_Component_Model_Solider_Gun_Weapon");
    WeaponDesc.isAddCollider = false;

    m_pWeapon = dynamic_cast<CWeapon_Anim*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Part_Weapon_Anim"), &WeaponDesc));
    if (nullptr == m_pWeapon)
        return E_FAIL;

    return S_OK;
}

CSolider_Gun* CSolider_Gun::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSolider_Gun* pInstance = new CSolider_Gun(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CSolider_Gun"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CSolider_Gun::Clone(void* pArg)
{
    CSolider_Gun* pInstance = new CSolider_Gun(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Created : CSolider_Gun"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSolider_Gun::Free()
{
    __super::Free();
}
