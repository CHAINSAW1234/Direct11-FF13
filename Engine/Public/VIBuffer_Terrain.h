#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
private:
	CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs);
	virtual ~CVIBuffer_Terrain() = default;

public:
	virtual HRESULT Initialize_Prototype(const wstring& strHeightMapFilePath);
	virtual HRESULT Initialize(void* pArg);
	virtual _bool	Compute_Picking(const CTransform* pTransform, _Out_  _float4* vOutPos = nullptr);

private:
	_uint				m_iNumVerticesX = { 0 };
	_uint				m_iNumVerticesZ = { 0 };

public:
	static CVIBuffer_Terrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strHeightMapFilePath);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END