#include "stdafx.h"
#include "Leopard.h"

#include "FSM.h"
#include "Leopard_State_Idle.h"
#include "Leopard_State_Attack.h"
#include "Leopard_State_Hit.h"

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
    m_strMonsterName = TEXT("게파드 열조");

    return S_OK;
}

HRESULT CLeopard::Initialize(void* pArg)
{
    GAMEOBJECT_DESC		GameObjectDesc{};

    GameObjectDesc.fSpeedPerSec = 5.f;
    GameObjectDesc.fRotationPerSec = XMConvertToRadians(360.f);

    if (FAILED(__super::Initialize(&GameObjectDesc)))
        return E_FAIL;

    return S_OK;
}

void CLeopard::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    m_pFSMCom->Update(fTimeDelta);

    m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

    if (m_pGameInstance->Get_KeyState(KEY_DOWN, DIK_4))
        Add_Chain(1.3f);

    if (GetKeyState(VK_LEFT) & 0x8000)
    {
        m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * -1.f);
    }

    if (GetKeyState(VK_RIGHT) & 0x8000)
    {
        m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);
    }

    if (GetKeyState('T') & 0x8000)
    {
        m_pTransformCom->Go_Up(fTimeDelta);
    }

    if (GetKeyState('G') & 0x8000)
    {
        m_pTransformCom->Go_Down(fTimeDelta);
    }

    if (GetKeyState(VK_DOWN) & 0x8000)
    {
        m_pTransformCom->Go_Backward(fTimeDelta);
    }

    if (GetKeyState(VK_UP) & 0x8000)
    {
        m_pTransformCom->Go_Straight(fTimeDelta);
    }



}

HRESULT CLeopard::Late_Tick(_float fTimeDelta)
{
    if (FAILED(__super::Late_Tick(fTimeDelta)))
        return E_FAIL;

    // 임시
    m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
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
    //m_pTargetObject = m_pGameInstance->Get_GameObject(g_Level, g_strChrLayerTag, 0);
    //Safe_AddRef(m_pTargetObject);
    Change_Animation(BATTLE_IDLE, true);
    /*m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(_float(rand() % 10 - 5), 0.f, _float(rand() % 10 - 5), 1.f));*/
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
    ColliderOBBDesc.vSize = _float3(1.f, .9f, 2.f);
    ColliderOBBDesc.vCenter = _float3(0.f, ColliderOBBDesc.vSize.y * 0.5f, -.3f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderOBBDesc)))
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

    Change_State(STATE_IDLE);
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
