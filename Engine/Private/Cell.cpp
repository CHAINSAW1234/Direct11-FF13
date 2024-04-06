#include "..\Public\Cell.h"
#include "VIBuffer_Cell.h"
#include "Transform.h"

CCell::CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CCell::Initialize(const _float3* vPoints, size_t iIndex)
{
	memcpy(m_vPoints, vPoints, sizeof(_float3) * POINT_END);

	m_iIndex = iIndex;

#ifdef _DEBUG

	m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, m_vPoints);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

#endif

	return S_OK;
}

_bool CCell::isIn(_fvector vPosition, _fmatrix TerrainWorldMatrix, _int* pNeighborIndex)
{
	_vector		vPoints[3];

	for (size_t i = 0; i < POINT_END; i++)
		vPoints[i] = XMVector3TransformCoord(XMLoadFloat3(&m_vPoints[i]), TerrainWorldMatrix);

	for (size_t i = 0; i < LINE_END; i++)
	{
		_vector		vSourDir = XMVector3Normalize(vPosition - vPoints[i]);

		_vector		vDir = vPoints[i + 1 >= POINT_END ? 0 : i + 1] - vPoints[i];
		_vector		vDestDir = XMVector3Normalize(XMVectorSet(XMVectorGetZ(vDir) * -1.f, 0.f, XMVectorGetX(vDir), 0.0f));

		if (0 < XMVectorGetX(XMVector3Dot(vSourDir, vDestDir)))
		{
			*pNeighborIndex = m_iNeighborIndices[i];
			return false;
		}
	}

	return true;
}

_bool CCell::Compare_Points(_fvector vSourPoint, _fvector vDestPoint)
{
	// XMVectorEqual();
	if (true == XMVector3Equal(vSourPoint, XMLoadFloat3(&m_vPoints[POINT_A])))
	{
		if (true == XMVector3Equal(vDestPoint, XMLoadFloat3(&m_vPoints[POINT_B])))
			return true;

		if (true == XMVector3Equal(vDestPoint, XMLoadFloat3(&m_vPoints[POINT_C])))
			return true;
	}

	if (true == XMVector3Equal(vSourPoint, XMLoadFloat3(&m_vPoints[POINT_B])))
	{
		if (true == XMVector3Equal(vDestPoint, XMLoadFloat3(&m_vPoints[POINT_C])))
			return true;

		if (true == XMVector3Equal(vDestPoint, XMLoadFloat3(&m_vPoints[POINT_A])))
			return true;
	}

	if (true == XMVector3Equal(vSourPoint, XMLoadFloat3(&m_vPoints[POINT_C])))
	{
		if (true == XMVector3Equal(vDestPoint, XMLoadFloat3(&m_vPoints[POINT_A])))
			return true;

		if (true == XMVector3Equal(vDestPoint, XMLoadFloat3(&m_vPoints[POINT_B])))
			return true;
	}

	return false;
}

void CCell::Take_Y(CTransform* pTransform,const _float fOffset, const _float4x4 WorldMatrix)
{
	_float4 vPos = pTransform->Get_State_Float4(CTransform::STATE_POSITION);

	_vector		vPoints[3];
	for (size_t i = 0; i < POINT_END; i++)
		vPoints[i] = XMVector3TransformCoord(XMLoadFloat3(&m_vPoints[i]), XMLoadFloat4x4(&WorldMatrix));

	_float3 Cross;
	_vector vCross = XMVector3Cross(vPoints[1] - vPoints[0], vPoints[2] - vPoints[0]);
	XMStoreFloat3(&Cross, vCross);
	_float fCrossD = XMVectorSum(XMVectorMultiply(vCross, vPoints[0])).m128_f32[0];

	vPos.y = (Cross.x * vPos.x + Cross.z * vPos.z - fCrossD) / -Cross.y;
	vPos.y += fOffset;
	pTransform->Set_State(CTransform::STATE_POSITION, vPos);
}

_float4 CCell::Sliding(_fvector vPosition, _fvector vMovement, const _float4x4 WorldMatrix)
{
	_vector		vPoints[3];

	for (size_t i = 0; i < POINT_END; i++)
		vPoints[i] = XMVector3TransformCoord(XMLoadFloat3(&m_vPoints[i]), XMLoadFloat4x4(&WorldMatrix));

	// 평면의 방정식 구하기
	_float3 Cross;
	_vector vCross = XMVector3Cross(vPoints[1] - vPoints[0], vPoints[2] - vPoints[0]);
	XMStoreFloat3(&Cross, vCross);
	_float fCrossD = XMVectorSum(XMVectorMultiply(vCross, vPoints[0])).m128_f32[0];



	for (size_t i = 0; i < LINE_END; i++)
	{
		_vector		vSourDir = XMVector3Normalize(vPosition - vPoints[i]);

		_vector		vDir = vPoints[i + 1 >= POINT_END ? 0 : i + 1] - vPoints[i];
		_vector		vDestDir = XMVector3Normalize(XMVectorSet(XMVectorGetZ(vDir) * -1.f, 0.f, XMVectorGetX(vDir), 0.0f));

		if (0 < XMVectorGetX(XMVector3Dot(vSourDir, vDestDir)))
		{
			// 1. 외적으로 법선 백터 구하기
			_vector n = XMVector3Normalize(XMVector3Cross(vDir, vCross));

			n *= XMVector3Dot(-vMovement, n);
			
			_float4 vSliding;
			XMStoreFloat4(&vSliding, vMovement + n);
			return vSliding;

		}
	}

	return _float4(0.f,0.f,0.f,0.f);
}


#ifdef _DEBUG

HRESULT CCell::Render()
{
	m_pVIBuffer->Bind_Buffers();

	return m_pVIBuffer->Render();
}

#endif

CCell* CCell::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, size_t iIndex)
{
	CCell* pInstance = new CCell(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pPoints, iIndex)))
	{
		MSG_BOX(TEXT("Failed To Created : CCell"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCell::Free()
{
#ifdef _DEBUG
	Safe_Release(m_pVIBuffer);
#endif

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}
