#include "stdafx.h"
#include "Chr_Field_State_Walk.h"
#include "Chr_Field.h"

CChr_Field_State_Walk::CChr_Field_State_Walk(CChr_Field* pChr_Field)
{
	m_pChr_Field = pChr_Field;
}

void CChr_Field_State_Walk::OnStateEnter()
{
	// 1. Chr_Field의 Look벡터구하고, y값 없애서 정규화
	_float4 vChrLook = m_pChr_Field->m_pTransformCom->Get_State_Float4(CTransform::STATE_LOOK);
	vChrLook.y = 0;
	XMStoreFloat4(&vChrLook, XMVectorSetW(XMVector3Normalize(XMLoadFloat4(&vChrLook)), 0.f));

	// 2. 키 입력 값으로 방향 결정, 그 방향과 Look백터로 각도 구하기
	_float fDegree = Cal_Degree_From_Directions_Between_Min180_To_180(Cal_Target_Direction(), vChrLook);
	


	// 1. -30 ~ 30도는 단순 회전으로 처리
	if (abs(fDegree) <= 30) {
		m_pChr_Field->Change_Animation(CChr_Field::WALK_IDLE, true);
	}
	else {
		// 각도가 0보다 크면 왼쪽이다
		if(fDegree >= 0)
			m_pChr_Field->Change_Animation(CChr_Field::WALK_START_WITH_TURN_LEFT, false);
		else {
			m_pChr_Field->Change_Animation(CChr_Field::WALK_START_WITH_TURN_RIGHT, false);
		}

	}

}

void CChr_Field_State_Walk::OnStateUpdate(_float fTimeDelta)
{
	m_pChr_Field->m_pTransformCom->Turn()

	Update_Animation();	// 사실상 Turn 끝내는 용도임
}

void CChr_Field_State_Walk::OnStateExit()
{
}

_float4 CChr_Field_State_Walk::Cal_Target_Direction()
{
	_vector vTargetDir = { 0.f,0.f,0.f,0.f };

	// 1. 카메라의 Look, Right 벡터 구한다
	 
	_vector vCamRight, vCamLook;
	vCamRight = m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_VIEW).r[0];
	vCamLook = m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_VIEW).r[2];

	// 2. 키 입력을 기준으로 이동 방향 결정 
	if (m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_W))
		vTargetDir += vCamLook;
	if (m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_S))
		vTargetDir -= vCamLook;

	if (m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_A))
		vTargetDir -= vCamRight;
	if (m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_D))
		vTargetDir += vCamRight;

	// 3. y값 지우기
	vTargetDir.m128_f32[1] = 0;
	
	// 4. 정규화된 이동 방향 결정 
	vTargetDir = XMVector3Normalize(vTargetDir);

	_float4 vOutputDir = { 0.f,0.f,0.f,0.f };

	XMStoreFloat4(&vOutputDir, vTargetDir);

	return vOutputDir;
}

_float CChr_Field_State_Walk::Cal_Degree_From_Directions_Between_Min180_To_180(_float4 vDir1, _float4 vDir2)
{
	// 1. 정규화 한 두 벡터를 가지고 각도를 계산 하기
	_float fRadian = XMVector3AngleBetweenNormals(XMLoadFloat4(&vDir1), XMLoadFloat4(&vDir2)).m128_f32[0];

	// 외적으로 방향 찾기
	_float  vCross = XMVector3Cross(XMLoadFloat4(&vDir1), XMLoadFloat4(&vDir2)).m128_f32[1];

	// 각도를 -180 ~ 180도 사이로 세팅
	_float fDegree = XMConvertToDegrees(fRadian * vCross / abs(vCross)); // 0 ~ 180 사이의 값 
	// 각도가 0보다 크면 왼쪽이다
	return fDegree;
}

void CChr_Field_State_Walk::Update_Animation()
{
	if (!m_pChr_Field->m_pModelCom->isFinished())
		return;

	switch (m_pChr_Field->m_pModelCom->Get_CurrentAnimationIndex()) {
	case CChr_Field::WALK_START_WITH_TURN_LEFT:
	case CChr_Field::WALK_START_WITH_TURN_RIGHT:
		m_pChr_Field->m_pModelCom->Set_Animation(CChr_Field::WALK_IDLE, true);
		break;
	}
}


CChr_Field_State_Walk* CChr_Field_State_Walk::Create(CChr_Field* pChr_Field)
{
	CChr_Field_State_Walk* pInstance = new CChr_Field_State_Walk(pChr_Field);
	//m_pPlayer = pGameObject;

	return pInstance;
}

void CChr_Field_State_Walk::Free()
{
	__super::Free();
}
