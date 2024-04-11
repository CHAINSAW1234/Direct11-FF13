#include "stdafx.h"
#include "Camera_Battle.h"

CCamera_Battle::CCamera_Battle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCamera{ pDevice, pContext }
{
}

CCamera_Battle::CCamera_Battle(const CCamera_Battle& rhs)
    : CCamera{ rhs }
{
}

HRESULT CCamera_Battle::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCamera_Battle::Initialize(void* pArg)
{
    m_eLevel = g_Level;

    if (nullptr == pArg)
        return E_FAIL;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;


    return S_OK;
}

void CCamera_Battle::Tick(_float fTimeDelta)
{

    Update_TargetPosition(fTimeDelta);
    Update_Dist(fTimeDelta);
    Update_With_Mouse(fTimeDelta);

    if (m_pGameInstance->Get_DIMouseState(DIMKS_LBUTTON)) {
        _long	MouseMove = { 0 };

        if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMMS_X))
        {
            m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * MouseMove * m_fMouseSensor);
        }
        if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMMS_Y))
        {
            m_pTransformCom->Turn(m_pTransformCom->Get_State_Vector(CTransform::STATE_RIGHT), fTimeDelta * MouseMove * m_fMouseSensor);
        }
    }

    __super::Bind_PipeLines();
}

HRESULT CCamera_Battle::Late_Tick(_float fTimeDelta)
{
    if (FAILED(__super::Late_Tick(fTimeDelta)))
        return E_FAIL;

    return S_OK;
}

HRESULT CCamera_Battle::Render()
{
    return S_OK;
}



void CCamera_Battle::Update_TargetPosition(_float fTimeDelta)
{
    // 모든 플레이어, 몬스터 객체를 순회하면서 위치를 그 중심으로 위치를 결정 
    _vector vPosition = { 0.f,0.f,0.f,1.f };
    _int iCntSum = 0;

    size_t iChrCnt = m_pGameInstance->Get_LayerCnt(g_Level, g_strChrLayerTag);
    iCntSum += iChrCnt;
    for (size_t i = 0; i < iChrCnt; ++i) {
        CTransform* pTransform = (CTransform*)m_pGameInstance->Get_Component(g_Level, g_strChrLayerTag, g_strTransformTag, (_int)i);
        if (nullptr == pTransform)
            return;
        vPosition += pTransform->Get_State_Vector(CTransform::STATE_POSITION);
    }

    size_t iMonsterCnt = m_pGameInstance->Get_LayerCnt(g_Level, g_strMonsterLayerTag);
    iCntSum += iMonsterCnt;
    for (size_t i = 0; i < iMonsterCnt; ++i) {
        CTransform* pTransform = (CTransform*)m_pGameInstance->Get_Component(g_Level, g_strMonsterLayerTag, g_strTransformTag, (_int)i);
        if (nullptr == pTransform)
            return;
        vPosition += pTransform->Get_State_Vector(CTransform::STATE_POSITION);
    }

    vPosition /= iCntSum;
    vPosition.m128_f32[3] = 1.f;
    vPosition.m128_f32[1] = 0.f;
    XMStoreFloat4(&m_vTargetPosition, vPosition);

    _vector vCurPos = XMLoadFloat4(&m_vCurrentPosition);

    if (XMVector3Length(vCurPos - vPosition).m128_f32[0] >= 1.f) {
        vCurPos += XMVector3Normalize(vPosition - vCurPos) * fTimeDelta;
        vCurPos.m128_f32[3] = 1.f;
        XMStoreFloat4(&m_vCurrentPosition, vCurPos);
    }

}

void CCamera_Battle::Update_Dist(_float fTimeDelta)
{
    _matrix ViewMatrix = m_pTransformCom->Get_WorldMatrix_Inverse();
    _matrix ProjectMatrix = XMMatrixPerspectiveFovLH(m_fFovy, m_fAspect, m_fNear, m_fFar);

    _float2 fabsMaxProjectPos = {0.f,0.f};

    size_t iChrCnt = m_pGameInstance->Get_LayerCnt(g_Level, g_strChrLayerTag);
    for (size_t i = 0; i < iChrCnt; ++i) {
        CTransform* pTransform = (CTransform*)m_pGameInstance->Get_Component(g_Level, g_strChrLayerTag, g_strTransformTag, (_int)i);
        _vector vPosition = pTransform->Get_State_Vector(CTransform::STATE_POSITION);
        vPosition = XMVector3TransformCoord(vPosition, ViewMatrix);
        vPosition = XMVector3TransformCoord(vPosition, ProjectMatrix);
        fabsMaxProjectPos.x = max(fabsMaxProjectPos.x, abs(vPosition.m128_f32[0]));
        fabsMaxProjectPos.y = max(fabsMaxProjectPos.y, abs(vPosition.m128_f32[1]));
    }

    size_t iMonsterCnt = m_pGameInstance->Get_LayerCnt(g_Level, g_strMonsterLayerTag);
    for (size_t i = 0; i < iMonsterCnt; ++i) {
        CTransform* pTransform = (CTransform*)m_pGameInstance->Get_Component(g_Level, g_strMonsterLayerTag, g_strTransformTag, (_int)i);
        _vector vPosition = pTransform->Get_State_Vector(CTransform::STATE_POSITION);
        vPosition = XMVector3TransformCoord(vPosition, ViewMatrix);
        vPosition = XMVector3TransformCoord(vPosition, ProjectMatrix);
        fabsMaxProjectPos.x = max(fabsMaxProjectPos.x, abs(vPosition.m128_f32[0]));
        fabsMaxProjectPos.y = max(fabsMaxProjectPos.y, abs(vPosition.m128_f32[1]));
    }


    if (fabsMaxProjectPos.x >= 0.8f) {
        m_fDist *= 1.001f;
    }
    else if (fabsMaxProjectPos.x <= 0.3f) {
        m_fDist /= 1.001f;
    }
    
    if (fabsMaxProjectPos.y >= 1.0f) {
        m_fDist *= 1.001f;
    }
    else if (fabsMaxProjectPos.y <= 0.5f) {
        m_fDist /= 1.001f;
    }

}

void CCamera_Battle::Update_With_Mouse(_float fTimeDelta)
{
    _vector vTargetPosition = XMLoadFloat4(&m_vCurrentPosition);
    vTargetPosition.m128_f32[1] += m_fYOffset; // temp
    _long MouseMove = { 0 };

    if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMMS_X))
    {
        m_fMouseMoveXAxis = 90.f;
        m_MouseMoveX = std::clamp(MouseMove, (_long)-10, (_long)10);

    }
    else {
        m_fMouseMoveXAxis *= 0.97f;
        if (m_fMouseMoveXAxis < 3) {
            m_fMouseMoveXAxis = 0;
        }
    }
    m_pTransformCom->Turn_With_Look_At(XMVectorSet(0.f, 1.f, 0.f, 0.f), vTargetPosition, m_fDist, fTimeDelta * sin(XMConvertToRadians(m_fMouseMoveXAxis)) * m_MouseMoveX * m_fMouseSensor);

    if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMMS_Y))
    {
        m_fMouseMoveYAxis = 90.f;
        m_MouseMoveY = clamp(MouseMove, (_long)-10, (_long)10);
    }
    else {
        m_fMouseMoveYAxis *= 0.9f;
        if (m_fMouseMoveYAxis < 3) {
            m_fMouseMoveYAxis = 0;
        }
    }

    // y축 각도 제한 걸기
    // y축 거리 차이가 fDist / sqrt(2)인 경우에만 작동
    m_pTransformCom->Turn_With_Look_At(m_pTransformCom->Get_State_Vector(CTransform::STATE_RIGHT), vTargetPosition, m_fDist, fTimeDelta * sin(XMConvertToRadians(m_fMouseMoveYAxis)) * m_MouseMoveY * m_fMouseSensor, XMConvertToDegrees(asin((m_fYOffset-0.01f) / m_fDist)));

}

void CCamera_Battle::Set_CursorPos()
{
    RECT rect;
    GetClientRect(g_hWnd, &rect);
    _int centerX = (rect.right - rect.left) / 2;
    _int centerY = (rect.bottom - rect.top) / 2;

    POINT centerPoint = { centerX, centerY };
    ClientToScreen(g_hWnd, &centerPoint);

    SetCursorPos(centerPoint.x, centerPoint.y);
}

CCamera_Battle* CCamera_Battle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCamera_Battle* pInstance = new CCamera_Battle(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CCamera_Battle"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCamera_Battle::Clone(void* pArg)
{
     CCamera_Battle* pInstance = new CCamera_Battle(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Created : CCamera_Battle"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCamera_Battle::Free()
{
    __super::Free();
}
