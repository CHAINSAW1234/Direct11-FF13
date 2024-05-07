#include "VIBuffer_Trail.h"
#include "GameInstance.h"
#include "Transform.h"

CVIBuffer_Trail::CVIBuffer_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer{ pDevice, pContext }
{
}

CVIBuffer_Trail::CVIBuffer_Trail(const CVIBuffer_Trail& rhs)
	: CVIBuffer{ rhs }
{
}

HRESULT CVIBuffer_Trail::Initialize_Prototype()
{
	m_iNumVertices = 1;
	m_pVerticesPos = new _float3[m_iNumVertices];
	m_iVertexStride = sizeof(VTXPOS);
	m_iNumIndices = 1;
	m_iIndexStride = sizeof(_ushort);
	m_iNumVertexBuffers = 1;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

#pragma region VERTEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	/* ���������� byteũ�� */
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	VTXPOS* pVertices = new VTXPOS[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOS) * m_iNumVertices);

	pVertices[0].vPosition = m_pVerticesPos[0] = _float3(0.f, 0.f, 0.f);

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

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	pIndices[0] = 0;

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Trail::Initialize(void* pArg)
{
	return S_OK;
}

CVIBuffer_Trail* CVIBuffer_Trail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Trail* pInstance = new CVIBuffer_Trail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CVIBuffer_Trail"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Trail::Clone(void* pArg)
{
	CVIBuffer_Trail* pInstance = new CVIBuffer_Trail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CVIBuffer_Trail"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Trail::Free()
{
	__super::Free();
}
