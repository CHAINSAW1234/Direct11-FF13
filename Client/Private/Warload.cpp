#include "stdafx.h"
#include "Warload.h"

#include "FSM.h"
#include "Warload_State_Idle.h"
#include "Warload_State_Attack.h"
#include "Warload_State_Hit.h"
#include "Warload_State_Skill.h"
#include "Warload_State_Field.h"
#include "Warload_State_Start.h"

#include "Chr_Battle.h"

CWarload::CWarload(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMonster{ pDevice, pContext }
{
}

CWarload::CWarload(const CWarload& rhs)
    : CMonster{ rhs }
{
}

HRESULT CWarload::Initialize_Prototype()
{
    m_iMaxHp = m_iHp = 4050;
    m_fStagger = 130.f;
    m_iDamage = 70;
    m_strMonsterName = TEXT("PSICOM 강공전술사");

    return S_OK;
}

HRESULT CWarload::Initialize(void* pArg)
{
    GAMEOBJECT_DESC		GameObjectDesc{};

    GameObjectDesc.fSpeedPerSec = 2.f;
    GameObjectDesc.fRotationPerSec = XMConvertToRadians(360.f);

    if (FAILED(__super::Initialize(&GameObjectDesc)))
        return E_FAIL;

    return S_OK;
}

void CWarload::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    if (m_pGameInstance->Get_KeyState(KEY_DOWN, DIK_4))
        Add_Chain(1.3f);

    if (m_pGameInstance->Get_KeyState(KEY_DOWN, DIK_5))
        Change_State(STATE_HIT);
}

HRESULT CWarload::Late_Tick(_float fTimeDelta)
{
    if (FAILED(__super::Late_Tick(fTimeDelta)))
        return E_FAIL;

    m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
    return S_OK;
}

HRESULT CWarload::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

void CWarload::Start()
{
    m_pTargetObject = dynamic_cast<CChr_Battle*>(m_pGameInstance->Get_GameObject(g_Level, g_strChrLayerTag, 1));
    m_pNavigationCom->Set_Index(m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION));
    //Safe_AddRef(m_pTargetObject);
    if (g_Level == LEVEL_BATTLE) {
        Change_State(STATE_IDLE);
        Change_Animation(BATTLE_IDLE, true);
    }
    else
        Change_State(STATE_FIELD);
    /*m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(_float(rand() % 10 - 5), 0.f, _float(rand() % 10 - 5), 1.f));*/
}

void CWarload::Set_Hit(_int iDamage)
{
    Min_Hp(iDamage);
    Create_Damage(iDamage);

    if (m_iHp <= 0)
        m_isDead = true;

    if (!m_isUseSkill && m_iHp <= m_iMaxHp / 2) {
        Change_State(STATE_SKILL);
        return;
    }

    if (m_eState != STATE_SKILL && m_isBreak) {
        Change_State(STATE_HIT);
    }


}

void CWarload::Set_State_Battle_Start()
{
    Change_State(STATE_START);
}

HRESULT CWarload::Change_State(STATE eState)
{
    m_eState = eState;
    m_pFSMCom->Change_State(eState);
    return S_OK;
}

void CWarload::Change_Animation(ANIMATION_WARLOAD iAnimationIndex, _bool isLoop)
{
    m_pModelCom->Set_Animation(iAnimationIndex, isLoop);
}

HRESULT CWarload::Add_Components()
{
    /* For.Com_Model */
    if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Warload"),
        TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
        return E_FAIL;

    /* Com_Collider_Body */
    CBounding_OBB::BOUNDING_OBB_DESC		ColliderOBBDesc{};

    /* 로컬상의 정보를 셋팅한다. */
    ColliderOBBDesc.vRotation = _float3(0.f, 0.f, 0.f);
    ColliderOBBDesc.vSize = _float3(1.2f, 2.1f, .8f);
    m_fColliderSizeZ = .4f;
    ColliderOBBDesc.vCenter = _float3(0.f, ColliderOBBDesc.vSize.y * 0.5f, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderOBBDesc)))
        return E_FAIL;

    ColliderOBBDesc.vSize = _float3(.2f, .3f, 3.f);
    ColliderOBBDesc.vCenter = _float3(0.f, 0.f, -1.f);

    CCollider_Bone::COLLIDER_BONE_DESC  ColliderBone_Desc = {};
    ColliderBone_Desc.pSocket = m_pModelCom->Get_BonePtr("lance");
    ColliderBone_Desc.pBounding_Desc = &ColliderOBBDesc;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Bone_OBB"),
        TEXT("Com_Collider_Weapon"), (CComponent**)&m_pCollider_WeaponCom, &ColliderBone_Desc)))
        return E_FAIL;

    CBounding_Sphere::BOUNDING_SPHERE_DESC ColliderSphereDesc = {};
    ColliderSphereDesc.fRadius = .8f;
    ColliderSphereDesc.vCenter = _float3(0.f, 0.f, 0.f);
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
        TEXT("Com_Collider_Push"), (CComponent**)&m_pCollider_PushCom, &ColliderSphereDesc)))
        return E_FAIL;


    if (FAILED(__super::Add_Components()))
        return E_FAIL;

    return S_OK;
}

HRESULT CWarload::Add_Component_FSM()
{
    if (FAILED(__super::Add_Component_FSM()))
        return E_FAIL;

    m_pFSMCom->Add_State(STATE_IDLE, CWarload_State_Idle::Create(this));
    m_pFSMCom->Add_State(STATE_ATTACK, CWarload_State_Attack::Create(this));
    m_pFSMCom->Add_State(STATE_HIT, CWarload_State_Hit::Create(this));
    m_pFSMCom->Add_State(STATE_SKILL, CWarload_State_Skill::Create(this));
    m_pFSMCom->Add_State(STATE_FIELD, CWarload_State_Field::Create(this));
   m_pFSMCom->Add_State(STATE_START, CWarload_State_Start::Create(this));

    return S_OK;
}

CWarload* CWarload::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CWarload* pInstance = new CWarload(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CWarload"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CWarload::Clone(void* pArg)
{
    CWarload* pInstance = new CWarload(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Created : CWarload"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CWarload::Free()
{
    __super::Free();
}
