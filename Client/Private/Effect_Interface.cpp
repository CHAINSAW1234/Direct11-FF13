#include "stdafx.h"
#include "Effect_Interface.h"
#include "Bone.h"

void Target_GameObject::Free()
{
	Safe_Release(m_pTargetObject);
}

_float4 Target_Bone::Get_Update_Location()
{
	const _float4x4* pMatrix = m_pSocket->Get_CombinedTransformationMatrix();

	_vector vScale;
	_vector vRotation;
	_vector vTranspose;

	XMMatrixDecompose(&vScale, &vRotation, &vTranspose, XMLoadFloat4x4(m_pParentMatrix));

	XMMatrixRotationQuaternion(vRotation);

	/*
		_float4x4			m_WorldMatrix;					// 자신의 월드 행렬
	const _float4x4*	m_pParentMatrix = { nullptr };	// 이 파츠를 보유하고 있는 GameObject == Parent의 월드 행렬을 포인터로 보유
	*/
	_matrix vMatrix = XMMatrixRotationQuaternion(vRotation) * XMLoadFloat4x4(pMatrix);
	_float4x4 fMatrix;
	XMStoreFloat4x4(&fMatrix, vMatrix);

	_float4 vPos = { fMatrix._41 + vTranspose.m128_f32[0] ,	fMatrix._42 + vTranspose.m128_f32[1],	fMatrix._43 + vTranspose.m128_f32[2],	1.f };

	return vPos;
}

void Target_Bone::Free()
{
	Safe_Release(m_pSocket);
}

const FRAME Interface_2D::EffectImageFrame[EFFECT_END] = {
	{25, 0.01f, 0, 0, 0},
	{2, 0.2f, 0, 0, 0}

};

void Interface_2D::Update_Frame(_float fTimeDelta)
{
	m_tFrame.fTime += fTimeDelta;

	if (m_tFrame.fTime > m_tFrame.fFrameSpeed)
	{
		m_tFrame.fTime = 0.f;
		++m_tFrame.iCurFrame;

		if (m_tFrame.iCurFrame >= m_tFrame.iMaxFrame) {
			Frame_Finish();
			m_tFrame.iCurFrame = 0;
		}
	}
}

void Interface_2D::Free()
{
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
}

void Interface_3D::Free()
{
	Safe_Release(m_pModelCom);
}

void Interface_Instance::Free()
{
	Safe_Release(m_pVIBufferCom);
}
