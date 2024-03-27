#include "stdafx.h"
#include "Leopard.h"

#include "FSM.h"

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

    GameObjectDesc.fSpeedPerSec = 10.f;
    GameObjectDesc.fRotationPerSec = XMConvertToRadians(360.f);

    if (FAILED(__super::Initialize(&GameObjectDesc)))
        return E_FAIL;

    return S_OK;
}

void CLeopard::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    if (m_pGameInstance->Get_KeyState(KEY_DOWN, DIK_4))
        Add_Chain(1.f);

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
    Change_Animation(BATTLE_IDLE, true);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(_float(rand() % 10 - 5), 0.f, _float(rand() % 10 - 5), 1.f));
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
    if (FAILED(__super::Add_Components()))
        return E_FAIL;

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Leopard"),
        TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLeopard::Add_Component_FSM()
{
    if (FAILED(__super::Add_Component_FSM()))
        return E_FAIL;

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
