#include "..\Public\VIBuffer_Instance.h"
#include "GameInstance.h"

CVIBuffer_Instance::CVIBuffer_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer{ pDevice, pContext }
{

}

CVIBuffer_Instance::CVIBuffer_Instance(const CVIBuffer_Instance& rhs)
	: CVIBuffer{ rhs }
	, m_pVBInstance{ rhs.m_pVBInstance }
	, m_iInstanceStride{ rhs.m_iInstanceStride }
	, m_iNumInstance{ rhs.m_iNumInstance }
	, m_iIndexCountPerInstance{ rhs.m_iIndexCountPerInstance }
	, m_pInstanceVertices{ rhs.m_pInstanceVertices }
	, m_InstanceBufferDesc{ rhs.m_InstanceBufferDesc }
	, m_InstanceSubResourceData{ rhs.m_InstanceSubResourceData }
	, m_pSpeeds{ rhs.m_pSpeeds }
	, m_InstanceDesc{ rhs.m_InstanceDesc }
{
	m_pLifeTimes = new _float2[m_iNumInstance];
	memcpy(m_pLifeTimes, rhs.m_pLifeTimes, sizeof(_float2) * m_iNumInstance);

	Safe_AddRef(m_pVBInstance);
}

HRESULT CVIBuffer_Instance::Initialize_Prototype(const CVIBuffer_Instance::INSTANCE_DESC& InstanceDesc)
{
	m_RandomNumber = mt19937_64(m_RandomDevice());
	m_InstanceDesc = InstanceDesc;

	return S_OK;
}

HRESULT CVIBuffer_Instance::Initialize(void* pArg)
{
	if (FAILED(m_pDevice->CreateBuffer(&m_InstanceBufferDesc, &m_InstanceSubResourceData, &m_pVBInstance)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Instance::Bind_Buffers()
{
	ID3D11Buffer* pVertexBuffers[] = {
		m_pVB,
		m_pVBInstance,
	};

	_uint				iVertexStrides[] = {
		m_iVertexStride,
		m_iInstanceStride
	};

	_uint				iOffsets[] = {
		0,
		0
	};

	/* 정점버퍼들을 장치에 바인딩한다. */
	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);

	/* 인덱스버퍼들을 장치에 바인딩한다. */
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);

	/* 그릴때 어떤 형태로 정점들을 이어 그릴건지. */
	m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

	return S_OK;
}

HRESULT CVIBuffer_Instance::Render()
{
	if (nullptr == m_pContext)
		return E_FAIL;

	/* 인덱스버퍼의 값을 이용하여 정점 버퍼의 정점들을 그려낸다. */
	m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_iNumInstance, 0, 0, 0);

	return S_OK;
}

_bool CVIBuffer_Instance::Compute_Picking(const CTransform* pTransform, _float4* vOutPos)
{
	return false;
}

void CVIBuffer_Instance::Begin()
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	m_pVertices = ((VTXMATRIX*)SubResource.pData);
}

void CVIBuffer_Instance::End()
{
	m_pContext->Unmap(m_pVBInstance, 0);
	m_pVertices = nullptr;
}

void CVIBuffer_Instance::Drop(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		m_pVertices[i].vPosition.y -= m_pSpeeds[i] * fTimeDelta;

	}
}

void CVIBuffer_Instance::Spread(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_vector		vDir = XMVectorSetW(XMLoadFloat4(&m_pVertices[i].vPosition) - XMLoadFloat3(&m_InstanceDesc.vPivot), 0.f);

		XMStoreFloat4(&m_pVertices[i].vPosition,
			XMLoadFloat4(&m_pVertices[i].vPosition) + XMVector3Normalize(vDir) * m_pSpeeds[i] * fTimeDelta);
	}
}

void CVIBuffer_Instance::Gather(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_vector		vDir = XMVectorSetW(XMLoadFloat4(&m_pVertices[i].vPosition) - XMLoadFloat3(&m_InstanceDesc.vPivot), 0.f);

		if (XMVectorGetX(XMVector3Length(vDir)) < 0.1f) {
			m_pVertices[i].vPosition = Compute_RandPosition();
		}

		XMStoreFloat4(&m_pVertices[i].vPosition,
			XMLoadFloat4(&m_pVertices[i].vPosition) - XMVector3Normalize(vDir) * m_pSpeeds[i] * fTimeDelta);
	}
}

void CVIBuffer_Instance::Set_Direction_To_Pivot_Up()
{
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_vector		vDir = XMVectorSetW(XMLoadFloat4(&m_pVertices[i].vPosition) - XMLoadFloat3(&m_InstanceDesc.vPivot), 0.f);
		// 내적 -> 값이 나온다 -:Look에 곱한다 
		_vector vLook = XMLoadFloat4(&m_pVertices[i].vLook);
		vLook = vLook * XMVectorGetX(XMVector3Dot(vLook, vDir));
		vDir = XMVector3Normalize(vDir - vLook);

		XMStoreFloat4(&m_pVertices[i].vUp, vDir);
		_vector vRight = XMVector3Cross(XMLoadFloat4(&m_pVertices[i].vUp), XMLoadFloat4(&m_pVertices[i].vLook));
		XMStoreFloat4(&m_pVertices[i].vRight, XMVectorSetW(XMVector3Normalize(vRight), 0.f));
	}
}

void CVIBuffer_Instance::Set_Direction_To_Pivot_Up_Reverse()
{
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_vector		vDir = XMVectorSetW(XMLoadFloat4(&m_pVertices[i].vPosition) - XMLoadFloat3(&m_InstanceDesc.vPivot), 0.f);
		// 내적 -> 값이 나온다 -:Look에 곱한다 
		_vector vLook = XMLoadFloat4(&m_pVertices[i].vLook);
		vLook = vLook * XMVectorGetX(XMVector3Dot(vLook, vDir));
		vDir = XMVector3Normalize(vDir - vLook);

		XMStoreFloat4(&m_pVertices[i].vUp, -vDir);
		_vector vRight = XMVector3Cross(XMLoadFloat4(&m_pVertices[i].vUp), XMLoadFloat4(&m_pVertices[i].vLook));
		XMStoreFloat4(&m_pVertices[i].vRight, XMVectorSetW(XMVector3Normalize(vRight), 0.f));
	}
}

void CVIBuffer_Instance::Set_Direction_To_Pivot_Right()
{
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_vector		vDir = XMVectorSetW(XMLoadFloat4(&m_pVertices[i].vPosition) - XMLoadFloat3(&m_InstanceDesc.vPivot), 0.f);
		// 내적 -> 값이 나온다 -:Look에 곱한다 
		_vector vLook = XMLoadFloat4(&m_pVertices[i].vLook);
		vLook = vLook * XMVectorGetX(XMVector3Dot(vLook, vDir));
		vDir = XMVector3Normalize(vDir - vLook);

		XMStoreFloat4(&m_pVertices[i].vRight, vDir);
		_vector vUp = XMVector3Cross(XMLoadFloat4(&m_pVertices[i].vLook), XMLoadFloat4(&m_pVertices[i].vRight));
		XMStoreFloat4(&m_pVertices[i].vUp, XMVectorSetW(XMVector3Normalize(vUp), 0.f));
	}
}

void CVIBuffer_Instance::Set_Direction_To_Pivot_Right_Reverse()
{
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_vector		vDir = XMVectorSetW(XMLoadFloat4(&m_pVertices[i].vPosition) - XMLoadFloat3(&m_InstanceDesc.vPivot), 0.f);
		// 내적 -> 값이 나온다 -:Look에 곱한다 
		_vector vLook = XMLoadFloat4(&m_pVertices[i].vLook);
		vLook = vLook * XMVectorGetX(XMVector3Dot(vLook, vDir));
		vDir = XMVector3Normalize(vDir - vLook);

		XMStoreFloat4(&m_pVertices[i].vRight, -vDir);
		_vector vUp = XMVector3Cross(XMLoadFloat4(&m_pVertices[i].vLook), XMLoadFloat4(&m_pVertices[i].vRight));
		XMStoreFloat4(&m_pVertices[i].vUp, XMVectorSetW(XMVector3Normalize(vUp), 0.f));
	}
}

void CVIBuffer_Instance::Set_Direction_To_Pivot_None()
{
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		m_pVertices[i].vRight = { 1.f,0.f,0.f,0.f };
		m_pVertices[i].vUp = { 0.f,1.f,0.f,0.f };
		m_pVertices[i].vLook = { 0.f,0.f,1.f,0.f };
	}
}

void CVIBuffer_Instance::Sin()
{
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		XMStoreFloat4(&m_pVertices[i].vPosition, XMVectorSetW(XMLoadFloat4(&m_pVertices[i].vRight) * sin(XMConvertToRadians(m_pLifeTimes[i].x * 360)), 1.f));
	}
}

void CVIBuffer_Instance::Set_Up_Camera(_fmatrix CamMatrix)
{
	_vector vRight = CamMatrix.r[0];
	_vector vUp = CamMatrix.r[1];
	_vector vLook = CamMatrix.r[2];

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		XMStoreFloat4(&m_pVertices[i].vRight, vRight);
		XMStoreFloat4(&m_pVertices[i].vUp, vUp);
		XMStoreFloat4(&m_pVertices[i].vLook, vLook);
	}
}

void CVIBuffer_Instance::Compute_LifeTime(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		m_pLifeTimes[i].x += fTimeDelta;

		if (m_pLifeTimes[i].x > m_pLifeTimes[i].y)
		{
			if (false == m_InstanceDesc.isLoop)
				m_pVertices[i].isLived = false;
			else
			{
				m_pLifeTimes[i].x = 0.f;
				m_pVertices[i].vPosition = Compute_RandPosition();
			}
		}
	}
}

void CVIBuffer_Instance::Free()
{
	__super::Free();

	if (false == m_isCloned)
	{
		Safe_Delete_Array(m_pSpeeds);
	}

	Safe_Delete_Array(m_pLifeTimes);

	Safe_Release(m_pVBInstance);
}
