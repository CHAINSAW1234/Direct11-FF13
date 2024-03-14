#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTransform final: public CComponent
{
public:
	typedef struct Transform_Desc
	{
		_float		fSpeedPerSec = { 0.0f };
		_float		fRotationPerSec = { 0.0f };
	}TRANSFORM_DESC;

public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };

public:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public:
	void Set_State(STATE eState, _fvector vState) {
		/*
			_matrix WorldMatrix;
			WorldMatrix = XMLoadFloat4x4(&m_WorldMatrix);
			WorldMatrix.r[eState] = vState;
			XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);
		*/

		_float4 vTemp;
		XMStoreFloat4(&vTemp, vState);
		memcpy(&m_WorldMatrix.m[eState], &vTemp, sizeof(_float4));
	}

	void Set_State(STATE eState, const _float4& vState) {
		memcpy(&m_WorldMatrix.m[eState], &vState, sizeof(_float4));
	}

	void Set_WorldMatrix(_float4x4 WorldMatrix) {
		memcpy(&m_WorldMatrix, &WorldMatrix, sizeof(_float4x4));
	}

	void Set_WorldMatrix(_matrix WorldMatrix) {
		XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);
	}

	void Set_Scaled(_float fScaleX, _float fScaleY, _float fScaleZ);

	_vector Get_State_Vector(STATE eState) {
		return XMLoadFloat4x4(&m_WorldMatrix).r[eState];
	}

	_float4 Get_State_Float4(STATE eState) {
		_float4 vState;
		XMStoreFloat4(&vState, XMLoadFloat4x4(&m_WorldMatrix).r[eState]);
		return vState;
	}

	_float3 Get_Scaled() const {
		_matrix WorldMatrix = XMLoadFloat4x4(&m_WorldMatrix);
		return _float3{
			XMVectorGetX(XMVector3Length(WorldMatrix.r[STATE_RIGHT])),
			XMVectorGetX(XMVector3Length(WorldMatrix.r[STATE_UP])),
			XMVectorGetX(XMVector3Length(WorldMatrix.r[STATE_LOOK]))
		};
	}

	_float4x4 Get_WorldFloat4x4() const {
		return m_WorldMatrix;
	}

	_float4x4& Get_WorldFloat4x4() {
		return m_WorldMatrix;
	}

	_float4x4* Get_WorldFloat4x4_Ptr() {
		return &m_WorldMatrix;
	}

	_matrix Get_WorldMatrix() const {
		return XMLoadFloat4x4(&m_WorldMatrix);
	}

	_float4x4 Get_WorldFloat4x4_Inverse() const {
		_float4x4	WorldMatrixInverse;
		XMStoreFloat4x4(&WorldMatrixInverse, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));
		return WorldMatrixInverse;
	}
	_matrix Get_WorldMatrix_Inverse() const {
		return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix));
	}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName);

public:
	void Go_Straight(_float fTimeDelta);
	void Go_Backward(_float fTimeDelta);
	void Go_Left(_float fTimeDelta);
	void Go_Right(_float fTimeDelta);
	void Go_Up(_float fTimeDelta);
	void Go_Down(_float fTimeDelta);
	void Look_At(_fvector vTargetPosition);			// LookAt _ Target
	void Look_At_ForLandObject(_fvector vTargetPosition);
	void Move_To_Target(_fvector vTargetPos, _float fTimeDelta, _float fMinDistance = 0.f);
	void Move_To_Direction(_fvector vDirection, _float fTimeDelta);
	void Turn(_fvector vAxis, _float fTimeDelta);
	void Rotation(_fvector vAxis, _float fRadian);			// �ð� ���� ��� ����

	void Turn_With_Look_At(_fvector vAxis, _fvector vTargetPosition, _float fDest, _float fTimeDelta, _int iMaxDegree = 0);

private:
	_float4x4				m_WorldMatrix;
	_float					m_fSpeedPerSec = { 0.0f };
	_float					m_fRotationPerSec = { 0.0f };

public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;

};

END