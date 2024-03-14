#include "stdafx.h"
#include "Camera_Field.h"

CCamera_Field::CCamera_Field(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera{ pDevice, pContext }
{
}

CCamera_Field::CCamera_Field(const CCamera_Field& rhs)
	: CCamera{ rhs }
{
}

HRESULT CCamera_Field::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Field::Initialize(void* pArg)
{
	m_eLevel = g_Level;

	if (nullptr == pArg)
		return E_FAIL;

	CAMERA_FIELD_DESC* pCameraFree = (CAMERA_FIELD_DESC*)pArg;
	m_fMouseSensor = pCameraFree->fMouseSensor;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	return S_OK;
}

void CCamera_Field::Tick(_float fTimeDelta)
{
	// 테스트를 위해서 LBUtton안으로 전체 넣기
	Update_With_Mouse(fTimeDelta);


	//if (m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_A))
	//	m_pTransformCom->Go_Left(fTimeDelta);

	//if (m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_D))
	//	m_pTransformCom->Go_Right(fTimeDelta);

	//if (m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_W))
	//	m_pTransformCom->Go_Straight(fTimeDelta);

	//if (m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_S))
	//	m_pTransformCom->Go_Backward(fTimeDelta);

	//if (m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_T))
	//	m_pTransformCom->Go_Up(fTimeDelta);

	//if (m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_G))
	//	m_pTransformCom->Go_Down(fTimeDelta);

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

HRESULT CCamera_Field::Late_Tick(_float fTimeDelta)
{
	if (FAILED(__super::Late_Tick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Field::Render()
{
	return S_OK;
}

void CCamera_Field::Update_With_Mouse(_float fTimeDelta)
{
	//Set_CursorPos();

	_vector vTargetPosition = dynamic_cast<CTransform*>(m_pTargetObject->Get_Component(g_strTransformTag))->Get_State_Vector(CTransform::STATE_POSITION);
	vTargetPosition.m128_f32[1] += 1; // temp
	_long MouseMove = { 0 };

	if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMMS_X))
	{
		m_fMouseMoveXAxis = 90.f;
		m_MouseMoveX = std::clamp(MouseMove, (_long)-50, (_long)50);

	}
	else {
		m_fMouseMoveXAxis *= 0.9f;
		if (m_fMouseMoveXAxis < 3) {
			m_fMouseMoveXAxis = 0;
		}
	}
	m_pTransformCom->Turn_With_Look_At(XMVectorSet(0.f, 1.f, 0.f, 0.f), vTargetPosition, m_fDist,  fTimeDelta * sin(XMConvertToRadians(m_fMouseMoveXAxis)) * m_MouseMoveX * m_fMouseSensor);

	if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMMS_Y))
	{
		m_fMouseMoveYAxis = 90.f;
		m_MouseMoveY = clamp(MouseMove, (_long)-50, (_long)50);
	}
	else {
		m_fMouseMoveYAxis *= 0.9f;
		if (m_fMouseMoveYAxis < 3) {
			m_fMouseMoveYAxis = 0;
		}
	}


	// y축 각도 제한 걸기
	// y축 거리 차이가 fDist / sqrt(2)인 경우에만 작동
	m_pTransformCom->Turn_With_Look_At(m_pTransformCom->Get_State_Vector(CTransform::STATE_RIGHT), vTargetPosition, m_fDist, fTimeDelta * sin(XMConvertToRadians(m_fMouseMoveYAxis)) * m_MouseMoveY * m_fMouseSensor, 60);



}

void CCamera_Field::Set_CursorPos()
{
	RECT rect;
	GetClientRect(g_hWnd, &rect);
	_int centerX = (rect.right - rect.left) / 2;
	_int centerY = (rect.bottom - rect.top) / 2;

	POINT centerPoint = { centerX, centerY };
	ClientToScreen(g_hWnd, &centerPoint);

	SetCursorPos(centerPoint.x, centerPoint.y);
}

CCamera_Field* CCamera_Field::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Field* pInstance = new CCamera_Field(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CCamera_Field"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_Field::Clone(void* pArg)
{
	CCamera_Field* pInstance = new CCamera_Field(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CCamera_Field"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Field::Free()
{
	__super::Free();
}
