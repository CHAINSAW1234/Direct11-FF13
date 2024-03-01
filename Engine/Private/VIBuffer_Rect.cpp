#include "..\Public\VIBuffer_Rect.h"
#include "GameInstance.h"
#include "Transform.h"


CVIBuffer_Rect::CVIBuffer_Rect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer{ pDevice, pContext }
{

}

CVIBuffer_Rect::CVIBuffer_Rect(const CVIBuffer_Rect & rhs)
	: CVIBuffer{ rhs }
{

}

HRESULT CVIBuffer_Rect::Initialize_Prototype()
{
	m_iNumVertices = 4;
	m_pVerticesPos = new _float3[m_iNumVertices];
	m_iVertexStride = sizeof(VTXPOSTEX);
	m_iNumIndices = 6;
	m_iIndexStride = sizeof(_ushort);
	m_iNumVertexBuffers = 1;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	/* ���������� byteũ�� */
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	VTXPOSTEX*		pVertices = new VTXPOSTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);

	pVertices[0].vPosition = m_pVerticesPos[0] =  _float3(-0.5f, 0.5f, 0.f);
	pVertices[0].vTexcoord = _float2(0.0f, 0.f);

	pVertices[1].vPosition = m_pVerticesPos[1] = _float3(0.5f, 0.5f, 0.f);
	pVertices[1].vTexcoord = _float2(1.0f, 0.f);

	pVertices[2].vPosition = m_pVerticesPos[2] = _float3(0.5f, -0.5f, 0.f);
	pVertices[2].vTexcoord = _float2(1.0f, 1.f);

	pVertices[3].vPosition = m_pVerticesPos[3] = _float3(-0.5f, -0.5f, 0.f);
	pVertices[3].vTexcoord = _float2(0.0f, 1.f);

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
#pragma endregion


#pragma region INDEX_BUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	/* �ε��� ������ byteũ�� */
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;
	
	_ushort*		pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;

	pIndices[3] = 0;
	pIndices[4] = 2;
	pIndices[5] = 3;

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Rect::Initialize(void * pArg)
{
	return S_OK;
}

_bool CVIBuffer_Rect::Compute_Picking(const CTransform* pTransform, _Out_ _float4* vOutPos)
{
	_float4		fRayDir, fRayPos;
	m_pGameInstance->Transform_PickingToLocalSpace(pTransform, &fRayDir, &fRayPos);
	_vector vRayDir = XMLoadFloat4(&fRayDir);
	_vector vRayPos = XMLoadFloat4(&fRayPos);

	_uint		iIndices[4] = { 0, 1, 2, 3 };
	_float		fDist;
	_vector		vOut = { 0.f,0.f,0.f,0.f };

	/* ������ �� �ﰢ���� �浹�ΰ�? */
	if (TriangleTests::Intersects(vRayPos, vRayDir,
		XMVectorSetW(XMLoadFloat3(&m_pVerticesPos[iIndices[0]]), 1.f),
		XMVectorSetW(XMLoadFloat3(&m_pVerticesPos[iIndices[1]]), 1.f),
		XMVectorSetW(XMLoadFloat3(&m_pVerticesPos[iIndices[2]]), 1.f),
		fDist) ||
		/* ���� �Ʒ� �ﰢ���� �浹�ΰ�? */
		DirectX::TriangleTests::Intersects(vRayPos, vRayDir,
			XMVectorSetW(XMLoadFloat3(&m_pVerticesPos[iIndices[0]]), 1.f),
			XMVectorSetW(XMLoadFloat3(&m_pVerticesPos[iIndices[2]]), 1.f),
			XMVectorSetW(XMLoadFloat3(&m_pVerticesPos[iIndices[3]]), 1.f),
			fDist)) 
	{
		vOut = vRayPos + vRayDir * fDist;
	}

	if (vOutPos != nullptr) {
		XMStoreFloat4(vOutPos, vOut);
	}
	
	return XMVector4Equal(vOut, XMVectorZero());
}

CVIBuffer_Rect * CVIBuffer_Rect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CVIBuffer_Rect*		pInstance = new CVIBuffer_Rect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CVIBuffer_Rect"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_Rect::Clone(void * pArg)
{
	CVIBuffer_Rect*		pInstance = new CVIBuffer_Rect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CVIBuffer_Rect"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Rect::Free()
{
	__super::Free();



}
