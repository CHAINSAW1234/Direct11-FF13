#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Line final : public CVIBuffer
{
public: 
	typedef struct VIBuffer_Line_Desc
	{
		_uint iNumVerticesX;
		_uint iNumVerticesZ;
		_uint iPadding = { 1 };
	} VIBUFFER_LINE_DESC;
private:
	CVIBuffer_Line(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Line(const CVIBuffer_Line& rhs);
	virtual ~CVIBuffer_Line() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _bool	Compute_Picking(const CTransform* pTransform, _Out_  _float4* vOutPos = nullptr);
public:
	void Set_Padding(_uint iPadding) { m_iPadding = iPadding; }
private:
	_uint				m_iNumVerticesX = { 0 };
	_uint				m_iNumVerticesZ = { 0 };
	_uint				m_iPadding = { 1 };

public:
	static CVIBuffer_Line* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END