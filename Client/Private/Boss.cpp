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

#include "Effect.h"
#include "Electricity.h"
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
    m_fChainResist = 50.f;
    m_iDamage = 58;
    m_vColliderSize = _float3(6.f, 6.f, 6.f);
    m_strMonsterName = TEXT("전투폭격기 카를라");
    m_strWeaponBoneName = "socket";

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

    m_fBarrierTimeDelta += fTimeDelta;

    for (auto& pBarrier : m_Barrier) {
        _float3 vScaled = pBarrier->Get_Transform()->Get_Scaled();
        pBarrier->Get_Transform()->Set_WorldMatrix(m_pTransformCom->Get_WorldFloat4x4());
        pBarrier->Get_Transform()->Set_Scaled(vScaled.x, vScaled.y, vScaled.z);
    }

    if (m_pGameInstance->Get_KeyState(KEY_DOWN, DIK_P)) {
        m_iHp -= 300;
    }
    if (m_pGameInstance->Get_DIMouseState(DIMKS_RBUTTON)) {
        Change_State(STATE_ATTACK_MAGIC);
    }
}

HRESULT CBoss::Late_Tick(_float fTimeDelta)
{
    if (FAILED(__super::Late_Tick(fTimeDelta)))
        return E_FAIL;

    return S_OK;
}

HRESULT CBoss::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            return E_FAIL;

        /* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이ㅏ터를 다 던져야한다. */

        if (m_isBreak || m_isBarrier) {
            if (FAILED(m_pShaderCom->Begin(3)))
                return E_FAIL;
        }
        else {
            if (FAILED(m_pShaderCom->Begin(0)))
                return E_FAIL;
        }

        m_pModelCom->Render(i);
    }

    return S_OK;
}

void CBoss::Start()
{
    m_pTargetObject = dynamic_cast<CChr_Battle*>(m_pGameInstance->Get_GameObject(g_Level, g_strChrLayerTag, 0));
    _float4 vPosition = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
    vPosition.y += 5.f;
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
    if (g_Level == LEVEL_BOSS_BATTLE) {
        vPosition.z += 5.f;
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
        Change_State(STATE_IDLE);

    }
    else
        Change_State(STATE_FIELD);

    if (g_Level == LEVEL_EFFECTTOOL) {
        Change_State(STATE_FIELD);
        return;
    }

    m_pNavigationCom->Set_Index(m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION));
    m_ePhase = PHASE1;
}

void CBoss::Set_Barrier(_bool isBarrier)
{
    m_isBarrier = isBarrier;

    if (m_isBarrier) {
        CEffect::Read_File_Loop("../Bin/Resources/Effect/Boss_Shield.dat", m_pGameInstance, m_pDevice, m_pContext, &m_Barrier);
    }
    else {
        for (auto& pBarrier : m_Barrier) {
            pBarrier->Set_Dead(true);
            Safe_Release(pBarrier);
        }
        m_Barrier.clear();
    }
}

void CBoss::Add_Chain(_float fChain)
{
    __super::Add_Chain(fChain);
    if (m_isBreak) {
        Set_Barrier(false);
        Clear_Pattern();
    }
}

void CBoss::Set_Hit(_int iDamage, _float fChain)
{
    if (m_isBarrier) {
        __super::Set_Hit(5, fChain);
    }
    else {
        __super::Set_Hit(iDamage, fChain);
    }

    if (m_ePhase == PHASE1 && m_iHp <= m_iMaxHp * 0.5) {
        m_ePhase = PHASE2;
        m_fBreakTimeDelta = 30.f;
        m_fChain = m_fCurChain = 100.f;
        m_fMagnification = 1 / (m_fStagger - 100) * 0.1f;
        Update_Chain(0);
        Clear_Pattern();
        Change_State(STATE_SKILL_BARRIER);
        m_Patterns.push(STATE_SKILL_HELLBLAST);
    }

    if (m_isBreak && (m_eState == STATE_IDLE || m_eState == STATE_HIT)) {
        Change_State(STATE_HIT);
        Set_TrackPosition(0.f);
    }

}

void CBoss::Create_UI_Number(CUI_Number::TYPE eType, _int iNum)
{
    CUI_Number::UI_NUMBER_DESC UI_Number_desc = {};
    UI_Number_desc.eType = eType;
    UI_Number_desc.iNumber = iNum;
    UI_Number_desc.vPosition = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
    UI_Number_desc.vPosition.y -= 3.f;
    if (FAILED(m_pGameInstance->Add_Clone(g_Level, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Number"), &UI_Number_desc)))
        return;
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
                m_Patterns.push(STATE_ATTACK_MAGIC);
                m_Patterns.push(STATE_ATTACK_MAGIC);
                m_Patterns.push(STATE_ATTACK_MAGIC);
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

_bool CBoss::Check_Pattern()
{
    if (m_Patterns.empty())
        return false;

    if( m_Patterns.front() == STATE_SKILL_BARRIER ||
        m_Patterns.front() == STATE_SKILL_HELLBLAST)
        return true;

    return false;
}

void CBoss::Clear_Pattern()
{
    while (!m_Patterns.empty()) {
        m_Patterns.pop();
    }

}

void CBoss::PlaySound_Attack()
{
    m_pGameInstance->PlaySoundDuplicate(TEXT("Boss_Attack_Physic_Attack.wav"), CSound_Manager::EFFECT_DUPLICATE, SOUND_DEFAULT);
}

void CBoss::Create_Electricity()
{
    CElectricity::ELECTRICITY_DESC pDesc;
    pDesc.fStartPosition = Get_BonePos("chin_a");
    pDesc.fTargetPosition = Get_TargetPosition();
    pDesc.pTargetObject = m_pTargetObject;
    pDesc.iDamage = m_iDamage/2;
    pDesc.iTurnDist = 0;
    if (FAILED(m_pGameInstance->Add_Clone(g_Level, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Electricity"), &pDesc)))
        return;

    pDesc.fTargetPosition.x += Random_Float(1.f);
    pDesc.fTargetPosition.z += Random_Float(1.f);
    pDesc.iTurnDist = 60;
    if (FAILED(m_pGameInstance->Add_Clone(g_Level, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Electricity"), &pDesc)))
        return;

    pDesc.fTargetPosition.x += Random_Float(1.f);
    pDesc.fTargetPosition.z += Random_Float(1.f);
    pDesc.iTurnDist = 120;
    if (FAILED(m_pGameInstance->Add_Clone(g_Level, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Electricity"), &pDesc)))
        return;
}

void CBoss::Change_Phase()
{
}

void CBoss::Update_Chain(_float fTimeDelta)
{
    if (m_isBreak) {
        m_fBreakTimeDelta += fTimeDelta;
        if (m_fBreakTimeDelta >= 30.f) {
            m_isBreak = false;
            m_fChain = 100.f;
            m_fCurChain = 100.f;
            
            if (m_ePhase == PHASE2 ) {
                Clear_Pattern();
                m_Patterns.push(STATE_SKILL_BARRIER);
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
    ColliderOBBDesc.vSize = m_vColliderSize;
    ColliderOBBDesc.vCenter = _float3(0.f, 0.f, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderOBBDesc)))
        return E_FAIL;

    ColliderOBBDesc.vSize = _float3(5.5f, 20.f, 5.5f);
    ColliderOBBDesc.vCenter = _float3(0.f, 0.f, 0.f);

    CCollider_Bone::COLLIDER_BONE_DESC  ColliderBone_Desc = {};
    ColliderBone_Desc.pSocket = m_pModelCom->Get_BonePtr(m_strWeaponBoneName.c_str());
    ColliderBone_Desc.pBounding_Desc = &ColliderOBBDesc;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Bone_OBB"),
        TEXT("Com_Collider_Weapon"), (CComponent**)&m_pCollider_WeaponCom, &ColliderBone_Desc)))
        return E_FAIL;

    CBounding_Sphere::BOUNDING_SPHERE_DESC ColliderSphereDesc = {};
    ColliderSphereDesc.fRadius = 3.f;
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

HRESULT CBoss::Bind_ShaderResources()
{
    if (FAILED(__super::Bind_ShaderResources()))
        return E_FAIL;

    if (m_isBarrier) {
        _float4 vColor = { 0.44f, 0.92f, 0.65f,1.f };

        if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_DissolveTime", &m_fBarrierTimeDelta, sizeof(_float))))
            return E_FAIL;
    }

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

    for (auto& pBarrier : m_Barrier) {
        pBarrier->Set_Dead(true);
        Safe_Release(pBarrier);
    }
    m_Barrier.clear();
}
