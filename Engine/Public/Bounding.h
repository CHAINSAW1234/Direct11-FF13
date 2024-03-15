#pragma once

#include "Base.h"

BEGIN(Engine)

class CBounding abstract : public CBase
{
public:
	typedef struct
	{
		_float3		vCenter;
	}BOUNDING_DESC;

protected:
	CBounding(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding() = default;

public:
	virtual HRESULT Initialize(CBounding::BOUNDING_DESC* pBoundingDesc);

#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch);
#endif

protected:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

public:
	virtual void Free() override;
};

END