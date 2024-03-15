#pragma once

#include "Bounding.h"

BEGIN(Engine)

class CBounding_AABB final : public CBounding
{
public:
	typedef struct : public CBounding::BOUNDING_DESC
	{
		_float3		vSize;
	}BOUNDING_AABB_DESC;
private:
	CBounding_AABB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding_AABB() = default;

public:
	virtual HRESULT Initialize(CBounding::BOUNDING_DESC* pBoundingDesc) override;
#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch) override;
#endif

private:
	BoundingBox*			m_pBoundingDesc = { nullptr };

public:
	static CBounding_AABB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CBounding::BOUNDING_DESC* pBoundingDesc);
	virtual void Free() override;
};

END