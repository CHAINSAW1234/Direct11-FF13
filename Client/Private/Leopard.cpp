#include "stdafx.h"
#include "Leopard.h"

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
}

HRESULT CLeopard::Late_Tick(_float fTimeDelta)
{
    if (FAILED(__super::Late_Tick(fTimeDelta)))
        return E_FAIL;

    // юс╫ц
    m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this);
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
}

HRESULT CLeopard::Add_Components()
{
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
