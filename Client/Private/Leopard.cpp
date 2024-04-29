#include "stdafx.h"
#include "Leopard.h"

#include "FSM.h"
#include "Leopard_State_Idle.h"
#include "Leopard_State_Attack.h"
#include "Leopard_State_Hit.h"
#include "Leopard_State_Field.h"
#include "Leopard_State_Start.h"

#include "Chr_Battle.h"

CLeopard::CLeopard(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMonster{ pDevice, pContext }
{
}

CLeopard::CLeopard(const CMonster& rhs)
    : CMonster{ rhs }
{
}

HRESULT CLeopard::Initialize_Prototype()
{
    m_iMaxHp = m_iHp = 375;
    m_fStagger = 103.f;
    m_fChainResist = 0.f;
    m_iDamage = 15;
    m_vColliderSize = _float3(1.f, .9f, 2.f);
    m_strMonsterName = TEXT("게파드 열조");
    m_strWeaponBoneName = "L_fing";
    return S_OK;
}

HRESULT CLeopard::Initialize(void* pArg)
{
    GAMEOBJECT_DESC		GameObjectDesc{};

    GameObjectDesc.fSpeedPerSec = 2.5f;
    GameObjectDesc.fRotationPerSec = XMConvertToRadians(360.f);

    if (FAILED(__super::Initialize(&GameObjectDesc)))
        return E_FAIL;

    return S_OK;
}

void CLeopard::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);
}

HRESULT CLeopard::Late_Tick(_float fTimeDelta)
{
    if (FAILED(__super::Late_Tick(fTimeDelta)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLeopard::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

void CLeopard::Start()
{
    m_pTargetObject = dynamic_cast<CChr_Battle*>(m_pGameInstance->Get_GameObject(g_Level, g_strChrLayerTag, 0));
    //Safe_AddRef(m_pTargetObject);
    if (g_Level == LEVEL_BATTLE) {
        Change_State(STATE_IDLE);
        Change_Animation(BATTLE_IDLE, true);
    }
    else
        Change_State(STATE_FIELD);
    m_pNavigationCom->Set_Index(m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION));
    /*m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(_float(rand() % 10 - 5), 0.f, _float(rand() % 10 - 5), 1.f));*/
}

void CLeopard::Set_Hit(_int iDamage, _float fChain)
{
    __super::Set_Hit(iDamage, fChain);
    
    Change_State(STATE_HIT);
    Set_TrackPosition(0.f);
}

void CLeopard::Set_State_Battle_Start()
{
    Change_State(STATE_START);
}

HRESULT CLeopard::Change_State(STATE eState)
{
    m_eState = eState;
    m_pFSMCom->Change_State(eState);
    return S_OK;
}

void CLeopard::Change_Animation(ANIMATION_LEOPARD iAnimationIndex, _bool isLoop)
{
    m_pModelCom->Set_Animation(iAnimationIndex, isLoop);
}

void CLeopard::PlaySound_Attack()
{
    if (rand() % 2) {
        m_pGameInstance->PlaySoundDuplicate(TEXT("Leopard_Attack_Hit_0.wav"), CSound_Manager::EFFECT_DUPLICATE, SOUND_DEFAULT);
    }
    else {
        m_pGameInstance->PlaySoundDuplicate(TEXT("Leopard_Attack_Hit_1.wav"), CSound_Manager::EFFECT_DUPLICATE, SOUND_DEFAULT);

    }
}

HRESULT CLeopard::Add_Components()
{
    /* For.Com_Model */
    if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Leopard"),
        TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
        return E_FAIL;

    /* Com_Collider_Body */
    CBounding_OBB::BOUNDING_OBB_DESC		ColliderOBBDesc{};

    /* 로컬상의 정보를 셋팅한다. */
    ColliderOBBDesc.vRotation = _float3(0.f, 0.f, 0.f);
    ColliderOBBDesc.vSize = m_vColliderSize;
    ColliderOBBDesc.vCenter = _float3(0.f, ColliderOBBDesc.vSize.y * 0.5f, -.3f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderOBBDesc)))
        return E_FAIL;

    ColliderOBBDesc.vSize = _float3(1.f, .5f, .5f);
    ColliderOBBDesc.vCenter = _float3(0.f, 0.f, 0.f);

    CCollider_Bone::COLLIDER_BONE_DESC  ColliderBone_Desc = {};
    ColliderBone_Desc.pSocket = m_pModelCom->Get_BonePtr(m_strWeaponBoneName.c_str());
    ColliderBone_Desc.pBounding_Desc = &ColliderOBBDesc;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Bone_OBB"),
        TEXT("Com_Collider_Weapon"), (CComponent**)&m_pCollider_WeaponCom, &ColliderBone_Desc)))
        return E_FAIL;

    CBounding_Sphere::BOUNDING_SPHERE_DESC ColliderSphereDesc = {};
    ColliderSphereDesc.fRadius = 1.f;
    ColliderSphereDesc.vCenter = _float3(0.f, 0.f, 0.f);
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
        TEXT("Com_Collider_Push"), (CComponent**)&m_pCollider_PushCom, &ColliderSphereDesc)))
        return E_FAIL;

    if (FAILED(__super::Add_Components()))
        return E_FAIL;

    return S_OK;
}

HRESULT CLeopard::Add_Component_FSM()
{
    if (FAILED(__super::Add_Component_FSM()))
        return E_FAIL;

    m_pFSMCom->Add_State(STATE_IDLE, CLeopard_State_Idle::Create(this));
    m_pFSMCom->Add_State(STATE_ATTACK, CLeopard_State_Attack::Create(this));
    m_pFSMCom->Add_State(STATE_HIT, CLeopard_State_Hit::Create(this));
    m_pFSMCom->Add_State(STATE_FIELD, CLeopard_State_Field::Create(this));
    m_pFSMCom->Add_State(STATE_START, CLeopard_State_Start::Create(this));

    //Change_State(STATE_IDLE);
    return S_OK;
}

CLeopard* CLeopard::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLeopard* pInstance = new CLeopard(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CLeopard"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLeopard::Clone(void* pArg)
{
    CLeopard* pInstance = new CLeopard(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Created : CLeopard"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLeopard::Free()
{
    __super::Free();
}
