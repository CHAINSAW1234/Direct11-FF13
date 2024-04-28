#include "stdafx.h"
#include "Chr_Battle_Light.h"

#include "Chr_Battle_Light_State_Idle.h"
#include "Chr_Battle_Light_State_Attack.h"
#include "Chr_Battle_Light_State_Hit.h"
#include "Chr_Battle_Light_State_Dead.h"
#include "Chr_Battle_Light_State_Item.h"
#include "Chr_Battle_Light_State_Optima.h"
#include "Chr_Battle_Light_State_Finish.h"
#include "Chr_Battle_Light_State_Prepare.h"

#include "FSM.h"
#include "Model.h"
#include "Shader.h"

#include "Effect_2D.h"
#include "Body.h"
#include "Weapon_Anim.h"
#include "Ability.h"
#include "Monster.h"
#include "Sphere.h"

#include "ImGUI_Manager.h"

CChr_Battle_Light::CChr_Battle_Light(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CChr_Battle{ pDevice, pContext }
{
}

CChr_Battle_Light::CChr_Battle_Light(const CChr_Battle_Light& rhs)
    : CChr_Battle{ rhs }
{
}

HRESULT CChr_Battle_Light::Initialize_Prototype()
{   
    m_strChrName = TEXT("라이트닝");
    m_iMaxHp = m_iHp = 300;
    m_iAttack_Physic = 79;
    m_iAttack_Magic = 26;
    m_vColliderSize = _float3(.6f, 1.6f, .6f);
    return S_OK;
}

HRESULT CChr_Battle_Light::Initialize(void* pArg)
{
    GAMEOBJECT_DESC		GameObjectDesc{};

    GameObjectDesc.fSpeedPerSec = 2.f;
    GameObjectDesc.fRotationPerSec = XMConvertToRadians(360.f);

    if (FAILED(__super::Initialize(&GameObjectDesc)))
        return E_FAIL;

#ifdef _DEBUG
    m_pImGUI_Manager = CImGUI_Manager::Get_Instance(m_pDevice, m_pContext);
    Safe_AddRef(m_pImGUI_Manager);
#endif
    return S_OK;
}

void CChr_Battle_Light::Tick(_float fTimeDelta)
{
    if (m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_W))
        m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);

    if (m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_A))
        m_pTransformCom->Go_Left(fTimeDelta, m_pNavigationCom);

    if (m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_S))
        m_pTransformCom->Go_Backward(fTimeDelta, m_pNavigationCom);

    if (m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_D))
        m_pTransformCom->Go_Right(fTimeDelta, m_pNavigationCom);

    __super::Tick(fTimeDelta);

    Update_FSMState(fTimeDelta);
}

HRESULT CChr_Battle_Light::Late_Tick(_float fTimeDelta)
{
    if (FAILED(__super::Late_Tick(fTimeDelta)))
        return E_FAIL;

    // 임시
    m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this);
    return S_OK;
}

HRESULT CChr_Battle_Light::Render()
{
    if(FAILED(__super::Render()))
        return E_FAIL;

#ifdef _DEBUG
    m_pImGUI_Manager->Tick(0);
    Show_ImGUI();
    m_pImGUI_Manager->Render();
#endif

    return S_OK;
}

void CChr_Battle_Light::Start()
{
    m_isAttackable = vector<int>(m_pGameInstance->Get_LayerCnt(g_Level, g_strMonsterLayerTag), true);
    Set_Target(m_pGameInstance->Get_GameObject(g_Level, g_strMonsterLayerTag, 0));
    m_pTransformCom->Look_At_ForLandObject(((CTransform*)m_pTargetObject->Get_Component(g_strTransformTag))->Get_State_Vector(CTransform::STATE_POSITION));
    m_pNavigationCom->Set_Index(m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION));

    Change_Animation_Weapon(WEAPON_OPEN_IDLE);
    
}

HRESULT CChr_Battle_Light::Change_State(STATE eState)
{
    m_eState = eState;
    m_pFSMCom->Change_State(eState);

    return S_OK;
}

void CChr_Battle_Light::Change_Animation(ANIMATION_CHR_BATTLE_LIGHT iAnimationIndex, _bool isLoop)
{
    dynamic_cast<CBody*>(m_PartObjects[0])->Change_Animation(iAnimationIndex, isLoop);
}

void CChr_Battle_Light::Change_Animation_Weapon(ANIMATION_CHR_BATTLE_LIGHT_WEAPON iAnimationIndex)
{
    dynamic_cast<CWeapon_Anim*>(m_PartObjects[1])->Change_Animation(iAnimationIndex, false);
}

_int CChr_Battle_Light::Get_Command_Cost_Sum()
{
    _int iCost = { 0 };
    for (size_t i = 0; i < m_pCommands->size(); ++i) {
        iCost += m_pCommands->at(i).second;
    }
    return iCost;
}

CRole::SKILL CChr_Battle_Light::Get_Current_Command()
{
    if (nullptr == m_pCommands || m_pCommands->empty())
        return CRole::SKILL_END;

    return m_pCommands->front().first;
}

void CChr_Battle_Light::Use_Command()
{
    if (nullptr != m_pCommands && !m_pCommands->empty()) {
        m_fATB -= Get_Current_Command_Cost();
        m_pCommands->pop_front();
    }
}

void CChr_Battle_Light::Lost_Command()
{
    if (nullptr != m_pCommands && !m_pCommands->empty()) {
        m_pCommands->pop_front();
    }
}

void CChr_Battle_Light::Cancel_Command()
{
    if (nullptr != m_pCommands && !m_pCommands->empty())
        m_pCommands->pop_back();
}

void CChr_Battle_Light::Set_Command(deque<pair<CRole::SKILL, _int>>* pCommand)
{
    Safe_Delete(m_pCommands);

    m_pCommands = pCommand;
    if(m_eState == IDLE)
        m_pFSMCom->Change_State(PREPARE);
}

void CChr_Battle_Light::Add_Hp(_int iHp)
{
    __super::Add_Hp(iHp);

    _int iRand = rand() % 4;

    if (!iRand) {
        _int irand = rand() % 2;
        wstring strSoundTag = L"Light_Heal_" + to_wstring(irand) + L".wav";
        const TCHAR* pChar = strSoundTag.c_str();
        m_pGameInstance->PlaySoundW(const_cast<TCHAR*>(strSoundTag.c_str()), CSound_Manager::CHR1, SOUND_DEFAULT);
    }
}

void CChr_Battle_Light::Update_Target()
{
    if (nullptr == m_pTargetObject) {
        return;
    }

    if (m_pTargetObject->Get_Dead()) {
        size_t iNumCnt = m_pGameInstance->Get_LayerCnt(g_Level, g_strMonsterLayerTag);
        if (iNumCnt == 0 || iNumCnt  == 1)
            return;
        while (1) {
            CGameObject* pGameObject = m_pGameInstance->Get_GameObject(g_Level, g_strMonsterLayerTag, rand() % iNumCnt);
            if (pGameObject->Get_Dead() == false) {
                Set_Target(pGameObject);
                ((CMonster*)m_pTargetObject)->Set_isTarget(true);
                return;
            }
        }
    }
}

void CChr_Battle_Light::Set_Hit(_int iDamage)
{
    if (m_eState == DEAD)
        return;

    __super::Set_Hit(iDamage);


    if (m_eState == ATTACK) {
        Lost_Command();
    }

    if (m_iHp <= 0) {
        Change_State(DEAD);
        m_pGameInstance->PlaySoundW(TEXT("Light_Dead.wav"), CSound_Manager::CHR1, SOUND_DEFAULT);
    }
    else {
        Change_State(HIT);
        _int irand = rand() % 3;
        wstring strSoundTag = L"Light_Damage_" + to_wstring(irand) + L".wav";
        const TCHAR* pChar = strSoundTag.c_str();
        m_pGameInstance->PlaySoundW(const_cast<TCHAR*>(strSoundTag.c_str()), CSound_Manager::CHR1, SOUND_DEFAULT);
    }

}

void CChr_Battle_Light::Use_Item()
{
    switch (m_eItem) {
    case CInventory::POTION:
    {
        size_t iNumCnt = m_pGameInstance->Get_LayerCnt(g_Level, g_strChrLayerTag);
        for (size_t i = 0; i < iNumCnt; ++i) {
            CChr_Battle* pChr_Battle = (CChr_Battle*)m_pGameInstance->Get_GameObject(g_Level, g_strChrLayerTag, (_uint)i);
            pChr_Battle->Add_Hp(150);
           
            _float4 vPos = pChr_Battle->Get_Transform()->Get_State_Float4(CTransform::STATE_POSITION);
            vPos.y += pChr_Battle->Get_ColliderSize().y * 0.5f;

            if (FAILED(CEffect::Read_File_NoLoop("../Bin/Resources/Effect/Heal_Camera_Look_Instance.dat", m_pGameInstance, m_pDevice, m_pContext, vPos)))
                return;
        }

    }
    break;
    case CInventory::PHOENIX_DOWN:
    {
        CChr_Battle* pChr_Battle = dynamic_cast<CChr_Battle*>(m_pTargetObject);
        if (nullptr != pChr_Battle) {
            if (pChr_Battle->Get_Hp() == 0) {
                pChr_Battle->Revive();

                _float4 vPos = pChr_Battle->Get_Transform()->Get_State_Float4(CTransform::STATE_POSITION);
                vPos.y += pChr_Battle->Get_ColliderSize().y * 0.5f;

                if (FAILED(CEffect::Read_File_NoLoop("../Bin/Resources/Effect/Heal_Camera_Look_Instance.dat", m_pGameInstance, m_pDevice, m_pContext, vPos)))
                    return;
            }
        }

    }
    break;
    case CInventory::SPECIAL_ITEM:
    {
        size_t iNumCnt = m_pGameInstance->Get_LayerCnt(g_Level, g_strChrLayerTag);
        for (size_t i = 0; i < iNumCnt; ++i) {
            CChr_Battle* pChr_Battle = (CChr_Battle*)m_pGameInstance->Get_GameObject(g_Level, g_strChrLayerTag, (_uint)i);
            _int iHp = pChr_Battle->Get_Hp();
            _int iMaxHp = pChr_Battle->Get_MaxHp();
            if (iHp < iMaxHp) {
                pChr_Battle->Add_Hp(iMaxHp - iHp);
                _float4 vPos = pChr_Battle->Get_Transform()->Get_State_Float4(CTransform::STATE_POSITION);
                vPos.y += pChr_Battle->Get_ColliderSize().y * 0.5f;

                if (FAILED(CEffect::Read_File_NoLoop("../Bin/Resources/Effect/Heal_Camera_Look_Instance.dat", m_pGameInstance, m_pDevice, m_pContext, vPos)))
                    return;
            }

        }
    }
    break;
    }
    m_eItem = CInventory::ITEM_END;
}

void CChr_Battle_Light::Change_Role(CAbility::ROLE eRole)
{
    __super::Change_Role(eRole);
    Change_State(OPTIMA);
}

void CChr_Battle_Light::Check_Interact_Weapon()
{
    CMonster* pMonster = dynamic_cast<CMonster*>(m_pTargetObject);
    if (nullptr == pMonster)
        return;

    if (!m_isAttackable[0] || pMonster->Get_Hp() <= 0)
        return;
    
    if (Get_Collider_Weapon()->Intersect(pMonster->Get_Collider())) {
        Set_AttackAble(0);
        pMonster->Set_Hit(m_iAttack_Physic, 0.5f);
            
        // 이펙트 생성
        _float fDist = m_vColliderSize.y;
        _vector vWeaponPos = XMLoadFloat4(&((CBody*)m_PartObjects[0])->Get_BonePosition("L_weapon"));	// 무의미
        _vector vTargetPos = pMonster->Get_Transform()->Get_State_Vector(CTransform::STATE_POSITION);
        vTargetPos.m128_f32[1] += pMonster->Get_ColliderSize().y * 0.5f;
        _vector vDir = vTargetPos - vWeaponPos;
        
        _float4 vPos;
        XMStoreFloat4(&vPos, vWeaponPos + XMVector3Normalize(vDir) * fDist);

        CEffect_2D::EFFECT_2D_DESC pDesc = {};
        pDesc.eEffect = Interface_2D::HIT_1;
        pDesc.vPosition = vPos;

        m_pGameInstance->Add_Clone(g_Level, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_2D"), &pDesc);

        if ((CEffect::Read_File_NoLoop("../Bin/Resources/Effect/Hit_Particle.dat", m_pGameInstance, m_pDevice, m_pContext, vPos)))
            return;
        
        // 사운드 재생
        _int irand = rand() % 4;
        wstring strSoundTag = L"Light_Attack_" + to_wstring(irand) + L".wav";
        const TCHAR* pChar = strSoundTag.c_str();
        m_pGameInstance->PlaySoundDuplicate(const_cast<TCHAR*>(strSoundTag.c_str()), CSound_Manager::EFFECT_DUPLICATE, SOUND_DEFAULT);

    }
}

void CChr_Battle_Light::Check_Interact_Weapon_Multi()
{
    _int iSizeChr = (_int)m_pGameInstance->Get_LayerCnt(g_Level, g_strMonsterLayerTag);
    for (int i = 0; i < iSizeChr; ++i) {
        if (!m_isAttackable[i])
            continue;

        CMonster* pMonster = dynamic_cast<CMonster*>(m_pGameInstance->Get_GameObject(g_Level, g_strMonsterLayerTag, i));

        if (nullptr == pMonster)
            return;

        if (Get_Collider_Weapon()->Intersect(pMonster->Get_Collider())) {
            Set_AttackAble(i);
            pMonster->Set_Hit((_int)(m_iAttack_Physic * 1.3f), 0.5f);


            // 이펙트 생성
            _float fDist = Get_Collider_Weapon()->IntersectDist(pMonster->Get_Collider());
            _vector vWeaponPos = XMLoadFloat4x4(dynamic_cast<CWeapon_Anim*>(m_PartObjects[1])->Get_WorldMatrix_Ptr()).r[3];
            _vector vTargetPos = pMonster->Get_Transform()->Get_State_Vector(CTransform::STATE_POSITION);
            _vector vDir = vTargetPos - vWeaponPos;

            _float4 vPos;
            XMStoreFloat4(&vPos, vWeaponPos + XMVector3Normalize(vDir) * fDist);

            CEffect_2D::EFFECT_2D_DESC pDesc = {};
            pDesc.eEffect = Interface_2D::HIT_2;
            pDesc.vPosition = vPos;

            m_pGameInstance->Add_Clone(g_Level, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_2D"), &pDesc);
          
            if ((CEffect::Read_File_NoLoop("../Bin/Resources/Effect/Hit_Particle.dat", m_pGameInstance, m_pDevice, m_pContext, vPos)))
                return;

            _int irand = rand() % 4;
            wstring strSoundTag = L"Light_Attack_" + to_wstring(irand) + L".wav";
            const TCHAR* pChar = strSoundTag.c_str();
            m_pGameInstance->PlaySoundDuplicate(const_cast<TCHAR*>(strSoundTag.c_str()), CSound_Manager::EFFECT_DUPLICATE, SOUND_DEFAULT);

        }
    }
}

void CChr_Battle_Light::Create_Sphere(_int iDamage, _int iWeaponNum)
{
    CAbility::ROLE eRole = m_pAbility->Get_CurrentRole();
    
    _float4 vPos = ((CBody*)m_PartObjects[iWeaponNum])->Get_BonePosition("L_weapon");	// 무의미
  
    if (m_pCommands->front().first == CRole::THUNDER) {
        CMonster* pMoster = (CMonster*)m_pTargetObject;

        pMoster->Set_Hit(iDamage, 10);
        _float4 vPos = Get_Target_Position();
        vPos.y = 0.f;
        CEffect::Read_File_NoLoop("../Bin/Resources/Effect/Lightning.dat", m_pGameInstance, m_pDevice, m_pContext, vPos);
        return;
    }

    CSphere::Sphere_Desc Sphere_Desc = {};
    Sphere_Desc.pTargetObject = m_pTargetObject;
    Sphere_Desc.vStartPosition = vPos;
    Sphere_Desc.isTargetMonster = true;
    Sphere_Desc.eSkill = m_pCommands->front().first;
    Sphere_Desc.iDamage = iDamage * 2;
    Sphere_Desc.fChain = 0.5f;


    if (FAILED(m_pGameInstance->Add_Clone(g_Level, TEXT("Layer_Bullet"), TEXT("Prototype_GameObject_Sphere"), &Sphere_Desc))) {
        int i = 0;
        return;
    }

    return;
}

HRESULT CChr_Battle_Light::Add_Components()
{
    if (FAILED(__super::Add_Components()))
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

    return S_OK;
}

HRESULT CChr_Battle_Light::Add_Component_FSM()
{
    if (FAILED(__super::Add_Component_FSM()))
        return E_FAIL;

    m_pFSMCom->Add_State(IDLE, CChr_Battle_Light_State_Idle::Create(this));
    m_pFSMCom->Add_State(ATTACK, CChr_Battle_Light_State_Attack::Create(this));
    m_pFSMCom->Add_State(ITEM, CChr_Battle_Light_State_Item::Create(this));
    m_pFSMCom->Add_State(PREPARE, CChr_Battle_Light_State_Prepare::Create(this));

    m_pFSMCom->Add_State(HIT, CChr_Battle_Light_State_Hit::Create(this));
    m_pFSMCom->Add_State(DEAD, CChr_Battle_Light_State_Dead::Create(this));
    m_pFSMCom->Add_State(OPTIMA, CChr_Battle_Light_State_Optima::Create(this));
    m_pFSMCom->Add_State(FINISH, CChr_Battle_Light_State_Finish::Create(this));

    Change_State(IDLE);
    return S_OK;
}

HRESULT CChr_Battle_Light::Add_Ability()
{
    m_pAbility = CAbility::Create();

    m_pAbility->Add_Role(CAbility::ATTACKER);
    m_pAbility->Add_Skill(CAbility::ATTACKER, CRole::ATTACK, 1);
    m_pAbility->Add_Skill(CAbility::ATTACKER, CRole::AREA_BLAST, 2);
    m_pAbility->Add_Skill(CAbility::ATTACKER, CRole::RUIN, 1);

    m_pAbility->Add_Role(CAbility::BLASTER);
    m_pAbility->Add_Skill(CAbility::BLASTER, CRole::THUNDER, 1);

    m_pAbility->Set_CurrentRole(CAbility::ATTACKER);

    return S_OK;
}

void CChr_Battle_Light::Update_FSMState(_float fTimeDelta)
{
    //if (m_pGameInstance->Get_DIMouseState(DIMKS_RBUTTON)) {
    //    Change_State(HIT);
    //}
}

void CChr_Battle_Light::Show_ImGUI()
{
    _float4x4 worldmatrix = m_pTransformCom->Get_WorldFloat4x4();
    _float vRight[4] = { worldmatrix.m[0][0], worldmatrix.m[0][1] , worldmatrix.m[0][2] , worldmatrix.m[0][3] };
    _float vUp[4] = { worldmatrix.m[1][0], worldmatrix.m[1][1] , worldmatrix.m[1][2] , worldmatrix.m[1][3] };
    _float vLook[4] = { worldmatrix.m[2][0], worldmatrix.m[2][1] , worldmatrix.m[2][2] , worldmatrix.m[2][3] };
    _float vPos[4] = { worldmatrix.m[3][0], worldmatrix.m[3][1] , worldmatrix.m[3][2] , worldmatrix.m[3][3] };

    string str;
    switch (m_eState) {
    case IDLE:
        str = "IDLE";
        break;
    case ATTACK:
        str = "ATTACK";
        break;
    case ITEM:
        str = "ITEM";
        break;
    case HIT:
        str = "HIT";
        break;
    case DEAD:
        str = "DEAD";
        break;
    case OPTIMA:
        str = "OPTIMA";
        break;
    }

    ImGui::Begin("Chr_Battle_Light");
    if (ImGui::TreeNode("Transform")) {
        ImGui::InputFloat4("Right", vRight);
        ImGui::InputFloat4("Up", vUp);
        ImGui::InputFloat4("Look", vLook);
        ImGui::InputFloat4("Pos", vPos);

        ImGui::TreePop();
    }
    ImGui::Text("Current State : ");
    ImGui::SameLine();
    ImGui::Text(str.c_str());
    ImGui::Text("Animation Index : %d", Get_CurrentAnimationIndex());
    ImGui::Text("Animation Frame : %f", Get_CurrentTrackPosition());

    if (nullptr != m_pCommands) {
        ImGui::Text("Queue size : %d", m_pCommands->size());
    }

    ImGui::End();
}

void CChr_Battle_Light::Determine_Action_Based_On_Command()
{
    // IDLE일때만 사용하시오
    if (nullptr != m_pCommands && !m_pCommands->empty() && m_fATB >= Get_Command_Cost_Sum()) {
        Change_State(ATTACK);
    }
    else if (CInventory::ITEM_END !=  m_eItem) {
        Change_State(ITEM);
    }

}

void CChr_Battle_Light::Set_State_Battle_Finish()
{
    Change_State(FINISH);
}

HRESULT CChr_Battle_Light::Add_PartObjects()
{
    /* For.Part_Body */
    CPartObject* pBodyObject = { nullptr };
    CBody::BODY_DESC BodyDesc{};

    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4_Ptr();
    BodyDesc.strModelTag = TEXT("Prototype_Component_Model_Light_Battle");


    pBodyObject = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Part_Body"), &BodyDesc));
    if (nullptr == pBodyObject)
        return E_FAIL;

    m_PartObjects.push_back(pBodyObject);

    /* For.Part_Weapon */
    CPartObject* pWeaponObject = { nullptr };
    CWeapon_Anim::WEAPON_ANIM_DESC	WeaponDesc{};

    CModel* pModel = (CModel*)pBodyObject->Get_Component(TEXT("Com_Model"));

    WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4_Ptr();
    WeaponDesc.pSocket = pModel->Get_BonePtr("R_weapon");
    WeaponDesc.strModelTag = TEXT("Prototype_Component_Model_Light_Weapon");
    WeaponDesc.Bounding_OBB_Desc.vSize = { 0.1f,0.3f,1.2f };
    WeaponDesc.Bounding_OBB_Desc.vCenter = { 0.f, 0.3f, -0.3f };

    WeaponDesc.Bounding_OBB_Desc.vRotation = { XMConvertToRadians(45),XMConvertToRadians(0),XMConvertToRadians(0) };

    pWeaponObject = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Part_Weapon_Anim"), &WeaponDesc));
    if (nullptr == pWeaponObject)
        return E_FAIL;

    m_PartObjects.push_back(pWeaponObject);

    return S_OK;
}

CChr_Battle_Light* CChr_Battle_Light::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CChr_Battle_Light* pInstance = new CChr_Battle_Light(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CChr_Battle_Light"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CChr_Battle_Light::Clone(void* pArg)
{
    CChr_Battle_Light* pInstance = new CChr_Battle_Light(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Created : CChr_Battle_Light"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CChr_Battle_Light::Free()
{
    __super::Free();

    Safe_Delete(m_pCommands);

    CImGUI_Manager::Destroy_Instance();
}
