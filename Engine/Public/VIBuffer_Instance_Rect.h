#pragma once

#include "VIBuffer_Instance.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Instance_Rect final : public CVIBuffer_Instance
{

private:
	CVIBuffer_Instance_Rect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Instance_Rect(const CVIBuffer_Instance_Rect& rhs);
	virtual ~CVIBuffer_Instance_Rect() = default;

public:
	virtual HRESULT Initialize_Prototype(const CVIBuffer_Instance::INSTANCE_DESC& InstanceDesc);
	virtual HRESULT Initialize(void* pArg) override;
	virtual _bool	Compute_Picking(const CTransform* pTransform, _Out_  _float4* vOutPos = nullptr);

public:
	static CVIBuffer_Instance_Rect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CVIBuffer_Instance::INSTANCE_DESC& InstanceDesc);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END