#include "stdafx.h"
#include "Chr_Battle_Light_Attack.h"
#include "Chr_Battle_Light.h"

CChr_Battle_Light_Attack::CChr_Battle_Light_Attack(CChr_Battle_Light* pChr_Battle_Light)
{
	m_pChr_Battle_Light = pChr_Battle_Light;
}

void CChr_Battle_Light_Attack::OnStateEnter()
{
	// 스킬이면 그자리에서 쓰도록 처리
	if (0) {		// 조건 추가
		m_eState = SKILL;
		m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::SKILL, false);
	}

	m_eState = RUN;

	Update_LookAt();

	if (abs(m_fDegree) < 45)
		m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::RUN_START, false);
	else if (m_fDegree > 0)
		m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::RUN_START_WITH_TURN_LEFT, false);
	else
		m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::RUN_START_WITH_TURN_RIGHT, false);

	m_testAttack = false;
}

void CChr_Battle_Light_Attack::OnStateUpdate(_float fTimeDelta)
{
	Update_LookAt();

	if (m_eState == ATTACK) {
		if (m_pGameInstance->Get_DIMouseState(DIMKS_LBUTTON)) {
			m_testAttack = true;
		}
	}

	switch (m_eState) {
	case RUN:
		Run(fTimeDelta);
		break;
	case UP:
		Up(fTimeDelta);
		break;
	case DOWN:
		Down(fTimeDelta);
		break;
	case ATTACK:
		Attack(fTimeDelta);
		break;
	case SKILL:
		Skill(fTimeDelta);
		break;
	case FINISH:
		Finish(fTimeDelta);
		break;
	}
}

void CChr_Battle_Light_Attack::OnStateExit()
{
}

void CChr_Battle_Light_Attack::Run(_float fTimeDelta)
{
	_vector vTargetPosition = XMLoadFloat4(&m_pChr_Battle_Light->Get_Target_Position());
	_float fTargetPositionY = vTargetPosition.m128_f32[1];
	vTargetPosition.m128_f32[1] = 0.f;

	_vector vCurrentPosition = m_pChr_Battle_Light->Get_Transform()->Get_State_Vector(CTransform::STATE_POSITION);
	_float fCurrentPositionY = vTargetPosition.m128_f32[1];
	vCurrentPosition.m128_f32[1] = 0.f;

	_float fDist = XMVector3Length(vTargetPosition - vCurrentPosition).m128_f32[0];

	if (abs(m_fDegree) > 5) {
		m_pChr_Battle_Light->Get_Transform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * m_fDegree / abs(m_fDegree));
	}

	m_pChr_Battle_Light->Get_Transform()->Go_Straight(fTimeDelta);

	if (m_pChr_Battle_Light->Is_Animation_Finished()) {
		m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::RUN_IDLE, true);
	}

	if (fDist <= 2) {
		if (fTargetPositionY - fCurrentPositionY >= 3) {
			m_eState = UP;
			m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::JUMP_START, false);
		}
		else {
			m_eState = ATTACK;
			m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::ATTACK_NOR1, false);
		}
	}

}

void CChr_Battle_Light_Attack::Up(_float fTimeDelta)
{
	_float fTargetPositionY = m_pChr_Battle_Light->Get_Target_Position().y;
	_float fCurrentPositionY = m_pChr_Battle_Light->Get_Transform()->Get_State_Float4(CTransform::STATE_POSITION).y;

	if (m_pChr_Battle_Light->Is_Animation_Finished()) {
		m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::JUMP_UP, true);
	}

	if (fTargetPositionY - fCurrentPositionY > 1) {
		m_pChr_Battle_Light->Get_Transform()->Go_Up(fTimeDelta);
	}
	else {
		m_eState = ATTACK;
		m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::ATTACK_AIR, false);
	}
}

void CChr_Battle_Light_Attack::Down(_float fTimeDelta)
{
	_float fCurrentPositionY = m_pChr_Battle_Light->Get_Transform()->Get_State_Float4(CTransform::STATE_POSITION).y;

	if (fCurrentPositionY > 1) {
		m_pChr_Battle_Light->Get_Transform()->Go_Down(fTimeDelta);
	}
	else {
		m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::JUMP_LAND, false);

		m_eState = FINISH;
	}
}

void CChr_Battle_Light_Attack::Attack(_float fTimeDelta)
{
	if (m_testAttack ) {
		if (m_pChr_Battle_Light->Get_Transform()->Get_State_Float4(CTransform::STATE_POSITION).y > 2) {
			if (m_pChr_Battle_Light->Get_CurrentTrackPosition() >= 23) {
				_uint iCurAnimation = m_pChr_Battle_Light->Get_CurrentAnimationIndex();
				_int nextAnimation = iCurAnimation;
				while (nextAnimation == iCurAnimation) {
					nextAnimation = rand() % 3;
				}

				m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::ANIMATION_CHR_BATTLE_LIGHT(nextAnimation), false);
				m_testAttack = false;
			}
		}
		else {
			if (m_pChr_Battle_Light->Get_CurrentTrackPosition() >= 17) {
				m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::ANIMATION_CHR_BATTLE_LIGHT(m_pChr_Battle_Light->Get_CurrentAnimationIndex() + 1), false);
				m_testAttack = false;
			}
			
		}
	}

	// 공격이 끝났을때, 공중에 있다면 Down으로 이동
	_float fCurrentPositionY = m_pChr_Battle_Light->Get_Transform()->Get_State_Float4(CTransform::STATE_POSITION).y;

	if (m_pChr_Battle_Light->Is_Animation_Finished()) {
		if (fCurrentPositionY > 1) {
			m_eState = DOWN;
			m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::JUMP_FALL, true);
		}
		else {
			m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::ATTACK_END, false);
			m_eState = FINISH;
		}
	}
}

void CChr_Battle_Light_Attack::Skill(_float fTimeDelta)
{
	// 이부분 수정 많이 필요함
	switch (m_pChr_Battle_Light->Get_CurrentAnimationIndex()) {
	case CChr_Battle_Light::SKILL:
		//if()
		break;
	case CChr_Battle_Light::SKILL_AIR:
		break;
	}

	if (m_pChr_Battle_Light->Is_Animation_Finished()) {
		m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::ANIM_IDLE, false);
		m_eState = RUN;
	}

}

void CChr_Battle_Light_Attack::Finish(_float fTimeDelta)
{
	if (m_pChr_Battle_Light->Is_Animation_Finished()) {
		switch (m_pChr_Battle_Light->Get_CurrentAnimationIndex()) {
		case CChr_Battle_Light::JUMP_LAND:
			m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::ATTACK_END, false);
			break;
		case CChr_Battle_Light::ATTACK_END:
			m_pChr_Battle_Light->Change_State(CChr_Battle_Light::IDLE);
			break;
		}
	}
	if (m_pChr_Battle_Light->Get_CurrentAnimationIndex() == CChr_Battle_Light::ATTACK_END) {
		if (m_pChr_Battle_Light->Get_CurrentTrackPosition() <= 25.f) {
			m_pChr_Battle_Light->Get_Transform()->Go_Backward(fTimeDelta);
		}
	}
}

void CChr_Battle_Light_Attack::Update_LookAt()
{
	_float4 vTargetLook;
	_vector vCurrent_To_Target_Look = (XMLoadFloat4(&m_pChr_Battle_Light->Get_Target_Position())
		- m_pChr_Battle_Light->Get_Transform()->Get_State_Vector(CTransform::STATE_POSITION));
	vCurrent_To_Target_Look.m128_f32[1] = 0.f;
	XMStoreFloat4(&vTargetLook, XMVectorSetW(XMVector3Normalize(vCurrent_To_Target_Look), 0.f));
	
	_float4 vCurrentLook = m_pChr_Battle_Light->Get_Transform()->Get_State_Float4(CTransform::STATE_LOOK);
	m_fDegree = Cal_Degree_From_Directions_Between_Min180_To_180(vCurrentLook, vTargetLook);
}
CChr_Battle_Light_Attack* CChr_Battle_Light_Attack::Create(CChr_Battle_Light* pChr_Battle_Light)
{
	CChr_Battle_Light_Attack* pInstance = new CChr_Battle_Light_Attack(pChr_Battle_Light);

	return pInstance;
}

void CChr_Battle_Light_Attack::Free()
{
	__super::Free();
}
