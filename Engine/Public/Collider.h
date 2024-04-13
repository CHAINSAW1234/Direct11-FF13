#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CCollider : public CComponent
{
public:
	enum TYPE { TYPE_AABB, TYPE_OBB, TYPE_SPHERE, TYPE_END };
protected:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;

public:
	virtual HRESULT Initialize_Prototype(TYPE eType);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_fmatrix WorldMatrix);
	virtual _bool Intersect(CCollider* pTargetCollider);
	virtual _bool IntersectRay(_fvector Origin, _fvector Direction, _Out_ float& Dist);
	virtual _float IntersectDist(CCollider* pTargetCollider);
#ifdef _DEBUG
public:
	HRESULT Render() override;

#endif

protected:
	TYPE				m_eType = { TYPE_END };
	_bool				m_isTrigger = { false };	// 충돌시 밀기?
	class CBounding*	m_pBounding = { nullptr };

#ifdef _DEBUG
protected:
	PrimitiveBatch<VertexPositionColor>* m_pBatch = { nullptr };
	BasicEffect* m_pEffect = { nullptr };
	ID3D11InputLayout* m_pInputLayout = { nullptr };
#endif



public:
	static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

END