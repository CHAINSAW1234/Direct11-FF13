#pragma once

#include "Base.h"

/* ���������������ο� �ʿ��� ViewMatrix, ProjMatrix�����ϳ�. */
/* ��Ÿ Ŭ���̾�Ʈ ������ �ʿ��� �����͵��� ����� ���夾����. (View, Proj Inv, CamPosition */

BEGIN(Engine)

class CPipeLine final : public CBase
{
public:
	enum PIPELINE { CAMERA, SHADOW, PIPELINE_END};
	enum TRANSFORMSTATE { D3DTS_VIEW, D3DTS_PROJ, D3DTS_END };
private:
	CPipeLine();
	virtual ~CPipeLine() = default;

public:
	void Set_Transform(TRANSFORMSTATE eState, _fmatrix TransformMatrix) {
		XMStoreFloat4x4(&m_TransformMatrices[CAMERA][eState], TransformMatrix);
	}
	void Set_Transform(TRANSFORMSTATE eState, _float4x4 TransformMatrix) {
		m_TransformMatrices[CAMERA][eState] = TransformMatrix;	// ���� �׽�Ʈ�Ұ� 
	}

	void Set_Shadow_Transform(TRANSFORMSTATE eState, _fmatrix TransformMatrix) {
		XMStoreFloat4x4(&m_TransformMatrices[SHADOW][eState], TransformMatrix);
	}
	void Set_Shadow_Transform(TRANSFORMSTATE eState, _float4x4 TransformMatrix) {
		m_TransformMatrices[SHADOW][eState] = TransformMatrix;	// ���� �׽�Ʈ�Ұ� 
	}

public:
	_matrix Get_Transform_Matrix(TRANSFORMSTATE eState) const {
		return XMLoadFloat4x4(&m_TransformMatrices[CAMERA][eState]);
	}
	_float4x4 Get_Transform_Float4x4(TRANSFORMSTATE eState) const {
		return m_TransformMatrices[CAMERA][eState];
	}
	_matrix Get_Transform_Matrix_Inverse(TRANSFORMSTATE eState) const {
		return XMLoadFloat4x4(&m_TransformInverseMatrices[CAMERA][eState]);
	}
	_float4x4 Get_Transform_Float4x4_Inverse(TRANSFORMSTATE eState) const {
		return m_TransformInverseMatrices[CAMERA][eState];
	}

	_vector Get_CamPosition_Vector() const {
		return XMLoadFloat4(&m_vCamPosition[CAMERA]);
	}

	_float4 Get_CamPosition_Float4() const {
		return m_vCamPosition[CAMERA];
	}

	//SHADOW
	_matrix Get_Shadow_Transform_Matrix(TRANSFORMSTATE eState) const {
		return XMLoadFloat4x4(&m_TransformMatrices[SHADOW][eState]);
	}
	_float4x4 Get_Shadow_Transform_Float4x4(TRANSFORMSTATE eState) const {
		return m_TransformMatrices[SHADOW][eState];
	}
	_matrix Get_Shadow_Transform_Matrix_Inverse(TRANSFORMSTATE eState) const {
		return XMLoadFloat4x4(&m_TransformInverseMatrices[SHADOW][eState]);
	}
	_float4x4 Get_Shadow_Transform_Float4x4_Inverse(TRANSFORMSTATE eState) const {
		return m_TransformInverseMatrices[SHADOW][eState];
	}


public:
	HRESULT Initialize();
	void Tick();

private:
	_float4x4			m_TransformMatrices[PIPELINE_END][D3DTS_END];
	_float4x4			m_TransformInverseMatrices[PIPELINE_END][D3DTS_END];

	_float4				m_vCamPosition[PIPELINE_END];

public:
	static CPipeLine* Create();
	virtual void Free() override;
};

END