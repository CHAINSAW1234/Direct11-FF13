#include "stdafx.h"
#include "Trigger.h"
#include "Collider.h"
#include "Level_Loading.h"

CTrigger::CTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CTrigger::CTrigger(const CTrigger& rhs)
    : CGameObject{ rhs }
{
}

HRESULT CTrigger::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTrigger::Initialize(void* pArg)
{
    m_eLevel = g_Level;

    GAMEOBJECT_DESC		GameObjectDesc{};

    GameObjectDesc.fSpeedPerSec = 10.f;
    GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

    if (FAILED(__super::Initialize(&GameObjectDesc)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    return S_OK;
}

void CTrigger::Tick(_float fTimeDelta)
{
    CCollider* pCollider = (CCollider*)m_pGameInstance->Get_Component(g_Level, g_strChrLayerTag, TEXT("Com_Collider"), 0);
    if (nullptr == pCollider)
        return;

    if (m_pColliderCom->Intersect(pCollider)) {
        m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_FIELD_BOSS));

    }
}

HRESULT CTrigger::Late_Tick(_float fTimeDelta)
{
    if(FAILED(__super::Late_Tick(fTimeDelta)))
        return E_FAIL;

    m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
    return S_OK;

}

HRESULT CTrigger::Render()
{
#ifdef _DEBUG
    m_pColliderCom->Render();
#endif
    return S_OK;
}

void CTrigger::Start()
{
    m_pTransformCom->Set_Position({57.f,0.f,260.f,1.f});
    m_pNavigationCom->Set_Index(m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION));
    m_pNavigationCom->Set_Y(m_pTransformCom, -0.2f);
    m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
}


HRESULT CTrigger::Add_Components()
{
    wstring strNaviTag;
    switch (m_eLevel) {
    case LEVEL_FIELD:
        strNaviTag = TEXT("Prototype_Component_Navigation_Field");
        break;
    case LEVEL_BATTLE:
        strNaviTag = TEXT("Prototype_Component_Navigation_Battle");
        break;
    case LEVEL_FIELD_BOSS:
    case LEVEL_BOSS_BATTLE:
        strNaviTag = TEXT("Prototype_Component_Navigation_Boss_Battle");
        break;
    }

    if (FAILED(__super::Add_Component(LEVEL_STATIC, strNaviTag,
        TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom)))
        return E_FAIL;


    /* Com_Collider*/
    CBounding_OBB::BOUNDING_OBB_DESC		ColliderOBBDesc{};

    /* 로컬상의 정보를 셋팅한다. */
    ColliderOBBDesc.vRotation = _float3(0.f, 0.f, 0.f);
    ColliderOBBDesc.vSize = _float3(16.f, 5.f, 1.f);
    //ColliderOBBDesc.vCenter = _float3(0.f, ColliderOBBDesc.vSize.y * 0.5f, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderOBBDesc)))
        return E_FAIL;

    return S_OK;
}

CTrigger* CTrigger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTrigger* pInstance = new CTrigger(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CTrigger"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTrigger::Clone(void* pArg)
{
    CTrigger* pInstance = new CTrigger(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Created : CTrigger"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTrigger::Free()
{
    __super::Free();
    Safe_Release(m_pColliderCom);
    Safe_Release(m_pNavigationCom);
}
