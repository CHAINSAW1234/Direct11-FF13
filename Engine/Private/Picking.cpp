#include "Picking.h"
#include "PipeLine.h"
#include "Transform.h"

CPicking::CPicking(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}


HRESULT CPicking::Initialize(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY)
{
	m_hWnd = hWnd;

	m_iWinSizeX = iWinSizeX;
	m_iWinSizeY = iWinSizeY;

	return S_OK;
}

void CPicking::Update(class CPipeLine* pPipeLine)
{
	POINT			ptMouse;
	GetCursorPos(&ptMouse);

	/* 뷰포트 상의 마우스 위치를 구하자. */
	ScreenToClient(m_hWnd, &ptMouse);

	_float3 fMousePos;
	_matrix ViewMatrixInv, ProjMatrixInv;
	/* 투영스페이스 상의 마우스 위치를 구하자. */
	fMousePos.x = ptMouse.x / (m_iWinSizeX * 0.5f) - 1.f;
	fMousePos.y = ptMouse.y / -(m_iWinSizeY * 0.5f) + 1.f;
	fMousePos.z = 0.f;

	_vector vMousePos = XMLoadFloat3(&fMousePos);

	ViewMatrixInv = pPipeLine->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_VIEW);
	ProjMatrixInv = pPipeLine->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_PROJ);

	vMousePos = XMVector3TransformCoord(vMousePos, ProjMatrixInv);

	_vector vRayPos, vRayDir;

	vRayDir = vMousePos - XMVectorZero();
	vRayPos = XMVectorSet(0.f,0.f,0.f,1.f);

	XMStoreFloat4(&m_vRayDir, XMVector3TransformNormal(vRayDir, ViewMatrixInv));
	XMStoreFloat4(&m_vRayPos, XMVector3TransformCoord(vRayPos, ViewMatrixInv));

}

void CPicking::Transform_PickingToLocalSpace(const CTransform* pTransform, _Out_ _float4* pRayDir, _Out_ _float4* pRayPos)
{
	_matrix	WorldMatrixInv = pTransform->Get_WorldMatrix_Inverse();

	XMStoreFloat4(pRayDir, XMVector4Normalize(XMVector4Transform(XMLoadFloat4(&m_vRayDir), WorldMatrixInv)));
	XMStoreFloat4(pRayPos, XMVector4Transform(XMLoadFloat4(&m_vRayPos), WorldMatrixInv));
}

CPicking* CPicking::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd, _uint iWinSizeX, _uint iWinSizeY)
{
	CPicking* pInstance = new CPicking(pDevice, pContext);

	if (FAILED(pInstance->Initialize(hWnd, iWinSizeX, iWinSizeY)))
	{
		MSG_BOX(TEXT("Failed To Created : CPicking"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPicking::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
