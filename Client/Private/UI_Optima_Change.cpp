#include "stdafx.h"
#include "UI_Optima_Change.h"

CUI_Optima_Change::CUI_Optima_Change(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CUI{ pDevice, pContext }
{
}

CUI_Optima_Change::CUI_Optima_Change(const CUI_Optima_Change& rhs)
    :CUI{ rhs }
{
}

HRESULT CUI_Optima_Change::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_Optima_Change::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_pTransformNameCom = CTransform::Create(m_pDevice, m_pContext);
    if (nullptr == m_pTransformNameCom)
        return E_FAIL;

    if (FAILED(m_pTransformNameCom->Initialize(pArg)))
        return E_FAIL;

    m_Components.emplace(TEXT("Com_TransformName"), m_pTransformNameCom);

    Safe_AddRef(m_pTransformNameCom);

    if (FAILED(Add_Components()))
        return E_FAIL;

    return S_OK;
}

void CUI_Optima_Change::Tick(_float fTimeDelta)
{
    Move1(fTimeDelta);
    Move2(fTimeDelta);
    Update_FontPosition();
}

HRESULT CUI_Optima_Change::Late_Tick(_float fTimeDelta)
{
    if (FAILED(__super::Late_Tick(fTimeDelta)))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Optima_Change::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    /* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이터를 다 던져야한다. */

    if (FAILED(m_pShaderCom->Begin(8)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    if (FAILED(Render_Font()))
        return E_FAIL;
  
    return S_OK;
}

void CUI_Optima_Change::Start()
{
    m_vStartPosition = { -(_float)g_iWinSizeX * 0.5f, 0.f, 0.f};
    m_vMiddlePosition = { 0.f, 0.f, 0.f };
    m_vTargetPosition = { g_iWinSizeX * 0.5f, 0.f, 0.f};

    m_fTimeDelta = 0.f;
    m_isRender = false;
    m_fSizeX = g_iWinSizeX * 0.8f;
    m_fSizeY = 5.f;
    m_pTransformCom->Set_Scaled(m_fSizeX, m_fSizeY, 1.f);
  
}

void CUI_Optima_Change::Reset_Position()
{
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_vStartPosition), 1.f));
    m_pTransformNameCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_vTargetPosition), 1.f));

    m_isRender = true;
    m_eState = EASEOUT;
    m_fTimeDelta = 0.f;
}

HRESULT CUI_Optima_Change::Bind_ShaderResources()
{
    _float4 vColor = { 1.f,1.f,1.f,1.f };
    _float fRatio = { 1.f };

	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_SizeX", &m_fSizeX, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_SizeY", &m_fSizeY, sizeof(_float))))
		return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_Color", &vColor, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_Ratio", &fRatio, sizeof(_float))))
        return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	return S_OK;
}

void CUI_Optima_Change::Move1(_float fTimeDelta)
{
    if (m_eState != EASEOUT)
        return;
        
    m_fTimeDelta += fTimeDelta;
    if (m_fTimeDelta >= 1.0f) {
        m_fTimeDelta = 1.0f;
    }

    _float4 vCurPos = {};   
    _float fEaseTime = (_float)(1 - pow(1 - m_fTimeDelta, 5));

    /*
    return x < 0.5
  ? (1 - Math.sqrt(1 - Math.pow(2 * x, 2))) / 2
  : (Math.sqrt(1 - Math.pow(-2 * x + 2, 2)) + 1) / 2;
    
    */

    XMStoreFloat4(&vCurPos, XMVectorSetW(XMVectorLerp(XMLoadFloat3(&m_vStartPosition), XMLoadFloat3(&m_vMiddlePosition), fEaseTime), 1.f));

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCurPos);

    XMStoreFloat4(&vCurPos, XMVectorSetW(XMVectorLerp(XMLoadFloat3(&m_vTargetPosition), XMLoadFloat3(&m_vMiddlePosition), fEaseTime), 1.f));
        
    m_pTransformNameCom->Set_State(CTransform::STATE_POSITION, vCurPos);

    if (m_fTimeDelta >= 1.0f) {
        m_fTimeDelta = 0.0f;
        m_eState = EASEIN;
    }

}

void CUI_Optima_Change::Move2(_float fTimeDelta)
{
    if (m_eState != EASEIN)
        return;

    m_fTimeDelta += fTimeDelta;
    if (m_fTimeDelta >= 1.0f) {
        m_fTimeDelta = 1.0f;
    }

    _float4 vCurPos = {};
    _float fEaseTime = (_float)pow(m_fTimeDelta, 5);

    /*
    return x < 0.5
  ? (1 - Math.sqrt(1 - Math.pow(2 * x, 2))) / 2
  : (Math.sqrt(1 - Math.pow(-2 * x + 2, 2)) + 1) / 2;

    */

    XMStoreFloat4(&vCurPos, XMVectorSetW(XMVectorLerp(XMLoadFloat3(&m_vMiddlePosition), XMLoadFloat3(&m_vTargetPosition), fEaseTime), 1.f));

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCurPos);

    XMStoreFloat4(&vCurPos, XMVectorSetW(XMVectorLerp(XMLoadFloat3(&m_vMiddlePosition), XMLoadFloat3(&m_vStartPosition), fEaseTime), 1.f));

    m_pTransformNameCom->Set_State(CTransform::STATE_POSITION, vCurPos);

    if (m_fTimeDelta >= 1.0f) {
        m_fTimeDelta = 0.0f;
        m_eState = STATE_END;
        m_isRender = false;
    }
}

HRESULT CUI_Optima_Change::Add_Components()
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

void CUI_Optima_Change::Update_FontPosition()
{
    _float4 vPosition = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
    m_vFontUpperPosition = { vPosition.x + (g_iWinSizeX) * 0.5f , -vPosition.y - 33.f + (g_iWinSizeY - m_fSizeY) * 0.5f };
    m_vFontUpperPosition.x -= 30 * 8;

    vPosition = m_pTransformNameCom->Get_State_Float4(CTransform::STATE_POSITION);
    m_vFontDownPosition = { vPosition.x + (g_iWinSizeX) * 0.5f , -vPosition.y + 5.f + (g_iWinSizeY - m_fSizeY) * 0.5f };
    m_vFontDownPosition.x -= m_strOptimaName.size() * 7;
}

HRESULT CUI_Optima_Change::Render_Font()
{
    if (FAILED(m_pGameInstance->Render_Font(g_strFontEng32Tag, TEXT ("OPTIMA CHANGE"), m_vFontUpperPosition, XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Render_Font(g_strFont14Tag, m_strOptimaName, { m_vFontDownPosition.x -1, m_vFontDownPosition.y-1 }, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.f)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Render_Font(g_strFont14Tag, m_strOptimaName, { m_vFontDownPosition.x -1, m_vFontDownPosition.y-1 }, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.f)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Render_Font(g_strFont14Tag, m_strOptimaName, { m_vFontDownPosition.x +1, m_vFontDownPosition.y+1 }, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.f)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Render_Font(g_strFont14Tag, m_strOptimaName, { m_vFontDownPosition.x +1, m_vFontDownPosition.y+1 }, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.f)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Render_Font(g_strFont14Tag, m_strOptimaName, { m_vFontDownPosition.x, m_vFontDownPosition.y }, XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f)))
        return E_FAIL;

    return S_OK;
}

CUI_Optima_Change* CUI_Optima_Change::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_Optima_Change* pInstance = new CUI_Optima_Change(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CUI_Optima_Change"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CUI_Optima_Change* CUI_Optima_Change::Clone(void* pArg)
{
    CUI_Optima_Change* pInstance = new CUI_Optima_Change(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Created : CUI_Optima_Change"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_Optima_Change::Free()
{
    __super::Free();
    Safe_Release(m_pTransformNameCom);
}
