#include "..\Public\Camera.h"

CCamera::CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CCamera::CCamera(const CCamera & rhs)
	: CGameObject(rhs)
{
}

HRESULT CCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera::Initialize(void * pArg)
{
	//CAMERA_DESC*		pCameraDesc = (CAMERA_DESC*)pArg;

	//m_fFovy = pCameraDesc->fFovy;
	//m_fAspect = pCameraDesc->fAspect;
	//m_fNear = pCameraDesc->fNear;
	//m_fFar = pCameraDesc->fFar;

	//if (FAILED(__super::Initialize(pArg)))
	//	return E_FAIL;

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, pCameraDesc->vEye);
	//m_pTransformCom->LookAt(pCameraDesc->vAt);

	return S_OK;
}

void CCamera::Tick(_float fTimeDelta)
{
}

void CCamera::Late_Tick(_float fTimeDelta)
{
}


HRESULT CCamera::Bind_PipeLines()
{	
	//_float4x4		ViewMatrix = *m_pTransformCom->Get_WorldMatrix_Inverse();
	///* SetTransform함수는 한번 바인딩 되면 재 바인딩될때 까지 계속 값을 유지한다. */
	//m_pGraphic_Device->SetTransform(D3DTS_VIEW, &ViewMatrix);

	//_float4x4		ProjMatrix;
	//m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, D3DXMatrixPerspectiveFovLH(&ProjMatrix, m_fFovy, m_fAspect, m_fNear, m_fFar));

	return S_OK;
}

void CCamera::Free()
{
	__super::Free();
}
