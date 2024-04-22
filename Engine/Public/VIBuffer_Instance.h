#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Instance abstract : public CVIBuffer
{
public:
	typedef struct Instance_Desc
	{
		_float3		vPivot;
		_float3		vCenter;
		_float3		vRange;
		_float3		vMinScale, vMaxScale;
		_float2		vLifeTime;
		_bool		isLoop;
		_float2		vSpeed;
		_uint		iNumInstance;
	}INSTANCE_DESC;

protected:
	CVIBuffer_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Instance(const CVIBuffer_Instance& rhs);
	virtual ~CVIBuffer_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype(const CVIBuffer_Instance::INSTANCE_DESC& InstanceDesc);
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Bind_Buffers();
	virtual HRESULT Render();

public:
	virtual _bool	Compute_Picking(const CTransform* pTransform, _Out_  _float4* vOutPos = nullptr);
	virtual _float4 Compute_RandPosition() = 0;
	virtual void	Reset_Instance();
	void Begin();
	void End();
	virtual void Drop(_float fTimeDelta);
	virtual void Spread(_float fTimeDelta);
	virtual void Gather(_float fTimeDelta);
	virtual void Move_Dir(_fvector vDir, _float fTimeDelta);

	virtual void Set_Direction_To_Pivot_Up();
	virtual void Set_Direction_To_Pivot_Up_Reverse();
	virtual void Set_Direction_To_Pivot_Right();
	virtual void Set_Direction_To_Pivot_Right_Reverse();
	virtual void Set_Direction_To_Pivot_None();
	virtual void Sin();				//sin 파 태우기 
	virtual void Set_Up_Camera(_fmatrix CamMatrix);


	void Compute_LifeTime(_float fTimeDelta);

protected:
	ID3D11Buffer*			m_pVBInstance = { nullptr };
	_uint					m_iInstanceStride = { 0 };
	_uint					m_iNumInstance = { 0 };
	_uint					m_iIndexCountPerInstance = { 0 };
	VTXMATRIX*				m_pInstanceVertices = { nullptr };
	INSTANCE_DESC			m_InstanceDesc{};

protected:
	D3D11_BUFFER_DESC		m_InstanceBufferDesc{};
	D3D11_SUBRESOURCE_DATA	m_InstanceSubResourceData{};

protected:
	random_device				m_RandomDevice;
	mt19937_64					m_RandomNumber;
	_float2*					m_pLifeTimes = { nullptr };
	_float*						m_pSpeeds = { nullptr };
	VTXMATRIX*					m_pVertices = { nullptr };
public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END