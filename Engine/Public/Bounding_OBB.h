#pragma once

#include "Bounding.h"

BEGIN(Engine)

class CBounding_OBB final : public CBounding
{
private:
	CBounding_OBB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding_OBB() = default;

public:
	virtual HRESULT Initialize(CBounding::BOUNDING_DESC* pBoundingDesc) override;
#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch) override;
#endif

private:
	BoundingOrientedBox*			m_pBoundingDesc = { nullptr };

public:
	static CBounding_OBB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CBounding::BOUNDING_DESC* pBoundingDesc);
	virtual void Free() override;
};

END