#include "stdafx.h"
#include "..\Public\Camera_Free.h"

CCamera_Free::CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera{ pDevice, pContext }
{

}

CCamera_Free::CCamera_Free(const CCamera_Free& rhs)
	: CCamera{ rhs }
{

}

HRESULT CCamera_Free::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Free::Initialize(void* pArg)
{
	m_eLevel = g_Level;

	if (nullptr == pArg)
		return E_FAIL;

	CAMERA_FREE_DESC* pCameraFree = (CAMERA_FREE_DESC*)pArg;
	m_fMouseSensor = pCameraFree->fMouseSensor;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CCamera_Free::Tick(_float fTimeDelta)
{
	if (m_eLevel != LEVEL_MAPTOOL) {
		Move_Camera(fTimeDelta);
	}
	__super::Bind_PipeLines();
}

HRESULT CCamera_Free::Late_Tick(_float fTimeDelta)
{
	if (FAILED(__super::Late_Tick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Free::Render()
{
	return S_OK;
}

void CCamera_Free::Move_Camera(_float fTimeDelta)
{
	if (m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_LSHIFT)) {

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
	}

}

CCamera_Free* CCamera_Free::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Free* pInstance = new CCamera_Free(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CCamera_Free"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CCamera_Free::Clone(void* pArg)
{
	CCamera_Free* pInstance = new CCamera_Free(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CCamera_Free"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Free::Free()
{
	__super::Free();

}
