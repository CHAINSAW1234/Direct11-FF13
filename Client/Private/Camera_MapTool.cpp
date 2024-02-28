#include "stdafx.h"
#include "Camera_MapTool.h"

CCamera_MapTool::CCamera_MapTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera{ pDevice, pContext }
{
}

CCamera_MapTool::CCamera_MapTool(const CCamera_MapTool& rhs)
	: CCamera{ rhs }
{
}

HRESULT CCamera_MapTool::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_MapTool::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	CAMERA_MAPTOOL_DESC* pCameraFree = (CAMERA_MAPTOOL_DESC*)pArg;

	m_fMouseSensor = pCameraFree->fMouseSensor;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CCamera_MapTool::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_A))
		m_pTransformCom->Go_Left(fTimeDelta);

	if (m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_D))
		m_pTransformCom->Go_Right(fTimeDelta);

	if (m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_W))
		m_pTransformCom->Go_Straight(fTimeDelta);

	if (m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_S))
		m_pTransformCom->Go_Backward(fTimeDelta);

	if (m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_T))
		m_pTransformCom->Go_Up(fTimeDelta);

	if (m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_G))
		m_pTransformCom->Go_Down(fTimeDelta);

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

void CCamera_MapTool::Late_Tick(_float fTimeDelta)
{
}

HRESULT CCamera_MapTool::Render()
{
	return S_OK;
}

CCamera_MapTool* CCamera_MapTool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_MapTool* pInstance = new CCamera_MapTool(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CCamera_MapTool"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_MapTool::Clone(void* pArg)
{
	CCamera_MapTool* pInstance = new CCamera_MapTool(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CCamera_MapTool"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_MapTool::Free()
{
	__super::Free();
}
