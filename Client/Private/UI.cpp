#include "stdafx.h"
#include "UI.h"

CUI::CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{pDevice, pContext}
    //, CObserver{}
{
}

CUI::CUI(const CUI& rhs)
    : CGameObject{ rhs }
    , m_isLateGroup{rhs.m_isLateGroup }
    //, CObserver{}
{
}

HRESULT CUI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI::Initialize(void* pArg)
{
    m_eLevel = g_Level;

    GAMEOBJECT_DESC		GameObjectDesc{};
    GameObjectDesc.fSpeedPerSec = 10.f;
    GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

    if (FAILED(__super::Initialize(&GameObjectDesc)))
        return E_FAIL;

    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));

    return S_OK;
}

void CUI::Tick(_float fTimeDelta)
{
}

HRESULT CUI::Late_Tick(_float fTimeDelta)
{
    if (FAILED(__super::Late_Tick(fTimeDelta)))
        return E_FAIL;

    if (m_isRender) {
        if(m_isLateGroup)
            m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this);
        else
            m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI_LATE, this);

    }

    return S_OK;
}

HRESULT CUI::Render()
{
	return S_OK;
}

void CUI::OnNotify()
{
}

HRESULT CUI::Bind_ShaderResources()
{
    if (nullptr == m_pShaderCom)
        return E_FAIL;

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    return S_OK;
}


void CUI::Free()
{
    __super::Free();

    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
}
