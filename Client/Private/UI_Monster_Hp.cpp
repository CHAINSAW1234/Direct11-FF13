#include "stdafx.h"
#include "UI_Monster_Hp.h"
#include "Monster.h"

CUI_Monster_Hp::CUI_Monster_Hp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI{ pDevice, pContext }
{
}

CUI_Monster_Hp::CUI_Monster_Hp(const CUI_Monster_Hp& rhs)
    : CUI{ rhs }
{
}

HRESULT CUI_Monster_Hp::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_Monster_Hp::Initialize(void* pArg)
{
    if (nullptr == pArg)
        return E_FAIL;

    UI_DESC* pUI_Desc = (UI_DESC*)pArg;

    m_pTargetMonster = (CMonster*)(pUI_Desc->pObserver_Hander);


    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    Safe_AddRef(m_pTargetMonster);
    m_pTargetMonster->RegisterObserver(this);

    return S_OK;

}

void CUI_Monster_Hp::Tick(_float fTimeDelta)
{
    Update_Hp(fTimeDelta);	// 체력 선형 보간, 비율까지 보두 계산
    Update_Position();
}

HRESULT CUI_Monster_Hp::Late_Tick(_float fTimeDelta)
{
    if (FAILED(__super::Late_Tick(fTimeDelta)))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Monster_Hp::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(6)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(7)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    if (FAILED(Render_Font()))
        return E_FAIL;

    return S_OK;
}

void CUI_Monster_Hp::Start()
{
    m_isRender = false;
    m_iHp = m_iStartHp = m_iCurHp = m_pTargetMonster->Get_MaxHp();
    m_iMaxHp = m_pTargetMonster->Get_MaxHp();
    m_fRatio = 1;
    m_fSizeX = 300;
    m_fSizeY = 5;
    m_fHpLerpTimeDelta = 0.f;
    m_pTransformCom->Set_Scaled(m_fSizeX, m_fSizeY, 1.f);
}

void CUI_Monster_Hp::OnNotify()
{
    if (m_pTargetMonster->Get_isTarget())
        m_isRender = true;
    else
        m_isRender = false;
}

HRESULT CUI_Monster_Hp::Bind_ShaderResources()
{
    _float4 vColor = { 0.f,1.f,0.f,1.f };

    if (FAILED(__super::Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_SizeX", &m_fSizeX, sizeof(_float))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_SizeY", &m_fSizeY, sizeof(_float))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_Color", &vColor, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_Ratio", &m_fRatio, sizeof(_float))))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Monster_Hp::Add_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
        TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
        return E_FAIL;

    /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
        return E_FAIL;

    return S_OK;
}

void CUI_Monster_Hp::Update_Hp(_float fTimeDelta)
{
    if (m_fHpLerpTimeDelta >= 1.f) {
        return;
    }

    m_fHpLerpTimeDelta += fTimeDelta * 10;
    if (m_fHpLerpTimeDelta >= 1.f) {
        m_fHpLerpTimeDelta = 1.f;
    }

    // 선형 보간 
    m_iCurHp = m_iStartHp + (_int)round(m_fHpLerpTimeDelta * _float(m_iHp - m_iStartHp));
    m_fRatio = (_float)m_iCurHp / (_float)m_iMaxHp;


}

void CUI_Monster_Hp::Update_Position()
{
    _vector vPosition = m_pTargetMonster->Get_Transform()->Get_State_Vector(CTransform::STATE_POSITION);
    vPosition.m128_f32[1] += 2.f;
    _matrix vViewMatrix = m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW);
    _matrix vProjMatrix = m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ);
    vPosition = XMVector3TransformCoord(XMVector3TransformCoord(vPosition, vViewMatrix), vProjMatrix);
    _float4 vPos;


    XMStoreFloat4(&vPos, vPosition);

    vPos.x *= g_iWinSizeX / 2;
    vPos.y *= g_iWinSizeY / 2;
    vPos.z = 0.f;

    vPos.x = clamp(vPos.x, -(_float)g_iWinSizeX / 3.f, (_float)g_iWinSizeX / 3.f);
    vPos.y = clamp(vPos.y, -(_float)g_iWinSizeY / 3.f, (_float)g_iWinSizeY / 3.f);
    vPos.y += 10;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
    m_vFontPosition = { vPos.x +(g_iWinSizeX - m_fSizeX) * 0.5f ,-vPos.y + g_iWinSizeY * 0.5f - 28.f };
}

HRESULT CUI_Monster_Hp::Render_Font()
{
    if (FAILED(m_pGameInstance->Render_Font(g_strFont14Tag, TEXT("Target"), m_vFontPosition, XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f)))
        return E_FAIL;

    return S_OK;
}

CUI_Monster_Hp* CUI_Monster_Hp::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_Monster_Hp* pInstance = new CUI_Monster_Hp(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CUI_Monster_Hp"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CUI_Monster_Hp* CUI_Monster_Hp::Clone(void* pArg)
{
    CUI_Monster_Hp* pInstance = new CUI_Monster_Hp(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Created : CUI_Monster_Hp"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_Monster_Hp::Free()
{
    __super::Free();
    Safe_Release(m_pTargetMonster);
}
