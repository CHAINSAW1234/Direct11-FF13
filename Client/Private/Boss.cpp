#include "stdafx.h"
#include "Boss.h"

#include "FSM.h"
#include "Boss_State_Idle.h"
#include "Boss_State_Attack_Physic.h"
#include "Boss_State_Attack_Magic.h"
#include "Boss_State_Skill_Barrier.h"
#include "Boss_State_Skill_HellBlast.h"
#include "Boss_State_Hit.h"
#include "Boss_State_Field.h"

#include "Chr_Battle.h"

CBoss::CBoss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMonster{ pDevice, pContext }
{
}

CBoss::CBoss(const CBoss& rhs)
    : CMonster{ rhs }
{
}

HRESULT CBoss::Initialize_Prototype()
{
    m_iMaxHp = m_iHp = 16200;
    m_fStagger = 200.f;
    m_iDamage = 58;
    m_strMonsterName = TEXT("전투폭격기 카를라");

    return S_OK;
}

HRESULT CBoss::Initialize(void* pArg)
{
    GAMEOBJECT_DESC		GameObjectDesc{};

    GameObjectDesc.fSpeedPerSec = 10.f;
    GameObjectDesc.fRotationPerSec = XMConvertToRadians(360.f);

    if (FAILED(__super::Initialize(&GameObjectDesc)))
        return E_FAIL;

    return S_OK;
}

void CBoss::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);
}

HRESULT CBoss::Late_Tick(_float fTimeDelta)
{
    if (FAILED(__super::Late_Tick(fTimeDelta)))
        return E_FAIL;

    m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
    return S_OK;
}

HRESULT CBoss::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

void CBoss::Start()
{
    m_pTargetObject = dynamic_cast<CChr_Battle*>(m_pGameInstance->Get_GameObject(g_Level, g_strChrLayerTag, 0));
    _float4 vPosition = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
    vPosition.y += 5.f;
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
    if (g_Level == LEVEL_BOSS_BATTLE) {
        vPosition.z += 2.f;
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
        Change_State(STATE_IDLE);

    }
    else
        Change_State(STATE_FIELD);


    m_pNavigationCom->Set_Index(m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION));

    m_ePhase = PHASE1;

}

void CBoss::Set_Hit(_int iDamage)
{
    Min_Hp(iDamage);
    Create_Damage(iDamage);

    if (m_ePhase == PHASE1 && m_iHp <= m_iMaxHp * 0.7) {
        m_ePhase = PHASE2;
        Clear_Pattern();
        Change_State(STATE_SKILL_BARRIER);

    }

    if (m_iHp <= 0)
        m_isDead = true;
    if(m_isBreak)
        Change_State(STATE_HIT);
}

HRESULT CBoss::Change_State(STATE eState)
{
    m_eState = eState;
    m_pFSMCom->Change_State(eState);
    return S_OK;
}

void CBoss::Change_Animation(ANIMATION_SOLIDER iAnimationIndex, _bool isLoop)
{
    m_pModelCom->Set_Animation(iAnimationIndex, isLoop);
}

void CBoss::Update_Pattern()
{
    if (m_Patterns.empty()) {
        switch (m_ePhase) {
        case PHASE1:
            m_Patterns.push(STATE_ATTACK_PHYSIC);
            break;
        case PHASE2:
            if (m_isBreak) {
                m_Patterns.push(STATE_ATTACK_PHYSIC);
            }
            else {
                m_Patterns.push(STATE_ATTACK_MAGIC);
                m_Patterns.push(STATE_SKILL_HELLBLAST);
            }
            break;
        }
    }

    m_pFSMCom->Change_State(m_Patterns.front());
    m_eState = m_Patterns.front();
    m_Patterns.pop();

}

void CBoss::Clear_Pattern()
{
    while (!m_Patterns.empty()) {
        m_Patterns.pop();
    }

}

void CBoss::Change_Phase()
{
}

void CBoss::Add_Chain(_float fChain)
{
    m_fChain += fChain;
    m_fCurChain = m_fChain;
    m_fMagnification = 1 / (m_fStagger - 100) * 0.1f;
    if (!m_isBreak && m_fChain >= m_fStagger) {
        m_fChain = m_fStagger + 100.f;
        m_isBreak = true;
        m_fBreakTimeDelta = 0.f;
        m_isBarrier = false;
    }
}

void CBoss::Update_Chain(_float fTimeDelta)
{
    if (m_isBreak) {
        m_fBreakTimeDelta += fTimeDelta;
        if (m_fBreakTimeDelta >= 20.f) {
            m_isBreak = false;
            m_fChain = 100.f;
            m_fCurChain = 100.f;
            
            if (m_ePhase == PHASE2 ) {
                Clear_Pattern();
                Change_State(STATE_SKILL_BARRIER);
                m_eState = STATE_SKILL_BARRIER;
            }
        }
    }
    else {
        m_fMagnification += fTimeDelta / 10;
        m_fCurChain -= fTimeDelta * m_fMagnification;
        if (m_fCurChain <= 100.f) {
            m_fChain = 100.f;
            m_fCurChain = 100.f;
            m_fMagnification = 1 / (m_fStagger - 100) * 0.1f;
        }
    }
}

HRESULT CBoss::Add_Components()
{
    /* For.Com_Model */
    if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Boss"),
        TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
        return E_FAIL;

    /* Com_Collider_Body */
    CBounding_OBB::BOUNDING_OBB_DESC		ColliderOBBDesc{};

    /* 로컬상의 정보를 셋팅한다. */
    ColliderOBBDesc.vRotation = _float3(0.f, 0.f, 0.f);
    ColliderOBBDesc.vSize = _float3(6.f, 6.f, 6.f);
    m_fColliderSizeZ = 3.f;
    ColliderOBBDesc.vCenter = _float3(0.f, 0.f, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderOBBDesc)))
        return E_FAIL;

    ColliderOBBDesc.vSize = _float3(5.f, .5f, .5f);
    ColliderOBBDesc.vCenter = _float3(0.f, 0.f, 0.f);

    CCollider_Bone::COLLIDER_BONE_DESC  ColliderBone_Desc = {};
    ColliderBone_Desc.pSocket = m_pModelCom->Get_BonePtr("socket");
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

HRESULT CBoss::Add_Component_FSM()
{
    if (FAILED(__super::Add_Component_FSM()))
        return E_FAIL;

    m_pFSMCom->Add_State(STATE_IDLE, CBoss_State_Idle::Create(this));
    m_pFSMCom->Add_State(STATE_ATTACK_PHYSIC, CBoss_State_Attack_Physic::Create(this));
    m_pFSMCom->Add_State(STATE_ATTACK_MAGIC, CBoss_State_Attack_Magic::Create(this));
    m_pFSMCom->Add_State(STATE_SKILL_BARRIER, CBoss_State_Skill_Barrier::Create(this));
    m_pFSMCom->Add_State(STATE_SKILL_HELLBLAST, CBoss_State_Skill_HellBlast::Create(this));
    m_pFSMCom->Add_State(STATE_FIELD, CBoss_State_Field::Create(this));
    m_pFSMCom->Add_State(STATE_HIT, CBoss_State_Hit::Create(this));

    return S_OK;
}

CBoss* CBoss::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBoss* pInstance = new CBoss(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CBoss"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBoss::Clone(void* pArg)
{
    CBoss* pInstance = new CBoss(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Created : CBoss"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBoss::Free()
{
    __super::Free();

    while(!m_Patterns.empty())
        m_Patterns.pop();
}
