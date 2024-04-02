#pragma once


#include "Collider.h"
#include "Bounding.h"

BEGIN(Engine)
class CBone;
class ENGINE_DLL CCollider_Bone final : public CCollider
{
public:
	typedef struct Collider_Bone_Desc
	{
		CBone* pSocket = { nullptr };
		CBounding::BOUNDING_DESC* pBounding_Desc = { nullptr };
	} COLLIDER_BONE_DESC;

private:
	CCollider_Bone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollider_Bone(const CCollider_Bone& rhs);
	virtual ~CCollider_Bone() = default;

public:
	virtual HRESULT Initialize_Prototype(TYPE eType);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_fmatrix WorldMatrix);

private:
	CBone* m_pSocket = { nullptr };

public:
	static CCollider_Bone* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

END