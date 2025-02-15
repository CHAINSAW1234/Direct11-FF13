#pragma once

#include "Base.h"

BEGIN(Engine)

class CCell final : public CBase
{
public:
	enum POINT { POINT_A, POINT_B, POINT_C, POINT_END };
	enum LINE { LINE_AB, LINE_BC, LINE_CA, LINE_END };
private:
	CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCell() = default;

public:
	_vector Get_Point(POINT ePoint) {
		return XMLoadFloat3(&m_vPoints[ePoint]);
	}

	void SetUp_Neighbor(LINE eLine, CCell* pNeighbor) {
		m_iNeighborIndices[eLine] = (_int)pNeighbor->m_iIndex;
	}

public:
	HRESULT Initialize(const _float3* pPoints, size_t iIndex);
	_bool isIn(_fvector vPosition, _fmatrix TerrainWorldMatrix, _int* pNeighborIndex);
	_bool Compare_Points(_fvector vSourPoint, _fvector vDestPoint);
	void  Take_Y(class CTransform* pTransform, const _float fOffset, const _float4x4 vWorldMatrix);
	_float4 Sliding(_fvector vPosition, _fvector vMovement, const _float4x4 vWorldMatrix);

#ifdef _DEBUG
public:
	virtual HRESULT Render();
#endif

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

private:
	_float3			m_vPoints[POINT_END] = {};
	size_t			m_iIndex = { 0 };

	_int			m_iNeighborIndices[LINE_END] = { -1, -1, -1 };

#ifdef _DEBUG

private:
	class CVIBuffer_Cell* m_pVIBuffer = { nullptr };

#endif

public:
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* vPoints, size_t iIndex);
	virtual void Free() override;
};

END