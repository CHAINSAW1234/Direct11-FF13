#include "stdafx.h"
#include "Chr_Battle_Light_State_Attack.h"
#include "Chr_Battle_Light.h"

CChr_Battle_Light_State_Attack::CChr_Battle_Light_State_Attack(CChr_Battle_Light* pChr_Battle_Light)
{
	m_pChr_Battle_Light = pChr_Battle_Light;
}

void CChr_Battle_Light_State_Attack::OnStateEnter()
{
	CRole::SKILL eSkill = m_pChr_Battle_Light->Get_Current_Command();
	m_fDegree = m_pChr_Battle_Light->Cal_Degree_Target();
	m_isCommandFinish = false;
	m_isCommandUse = false;

	if (eSkill == CRole::ATTACK || eSkill == CRole::AREA_BLAST) {	// 공격이면 달려가야함
		m_eState = RUN;

		if (abs(m_fDegree) < 45)
			m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::RUN_START, false);
		else if (m_fDegree > 0)
			m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::RUN_START_WITH_TURN_LEFT, false);
		else
			m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::RUN_START_WITH_TURN_RIGHT, false);

	}
	else if (eSkill != CRole::SKILL_END) {	// 스킬이면 그자리에서 쓰도록 처리
		m_eState = SKILL;
		m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::SKILL, false);
	}

}

void CChr_Battle_Light_State_Attack::OnStateUpdate(_float fTimeDelta)
{
	m_fDegree = m_pChr_Battle_Light->Cal_Degree_Target();

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

void CChr_Battle_Light_State_Attack::OnStateExit()
{
	m_pChr_Battle_Light->Reset_Attakable();
}

void CChr_Battle_Light_State_Attack::Run(_float fTimeDelta)
{

	_float fDist = m_pChr_Battle_Light->Cal_Dist_Target();

	if (abs(m_fDegree) > 5) {
		m_pChr_Battle_Light->Get_Transform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta*2 * m_fDegree / abs(m_fDegree));
	}

	m_pChr_Battle_Light->Get_Transform()->Go_Straight(fTimeDelta * 3, m_pChr_Battle_Light->Get_Navigation());

	if (m_pChr_Battle_Light->Is_Animation_Finished()) {
		m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::RUN_IDLE, true);
	}

	if (fDist <= 0.5f) {
		if (m_pChr_Battle_Light->Get_Current_Command() == CRole::AREA_BLAST) {
			m_eState = ATTACK;
			m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::ATTACK_AREABLAST, false);
		}
		else {
			if (Get_Dist_Y() >= 2.f) {
				m_eState = UP;
				m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::JUMP_START, false);
			}
			else {
				m_eState = ATTACK;
				m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::ATTACK_NOR1, false);

			}
		}
	}
}

void CChr_Battle_Light_State_Attack::Up(_float fTimeDelta)
{
	_float fTargetPositionY = m_pChr_Battle_Light->Get_Target_Position().y;
	_float fCurrentPositionY = m_pChr_Battle_Light->Get_Transform()->Get_State_Float4(CTransform::STATE_POSITION).y;

	if (m_pChr_Battle_Light->Is_Animation_Finished()) {
		m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::JUMP_UP, true);
	}

	if (Get_Dist_Y() > 1) {
		m_pChr_Battle_Light->Get_Transform()->Go_Up(fTimeDelta * 8);
	}
	else {
		m_eState = ATTACK;
		m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::ATTACK_AIR, false);
	}
}

void CChr_Battle_Light_State_Attack::Down(_float fTimeDelta)
{
	_float fCurrentPositionY = m_pChr_Battle_Light->Get_Transform()->Get_State_Float4(CTransform::STATE_POSITION).y;

	if (fCurrentPositionY >= 0.1) {
		m_pChr_Battle_Light->Get_Transform()->Go_Down(fTimeDelta * 8);
	}
	else {
		CRole::SKILL eSkill = m_pChr_Battle_Light->Get_Current_Command();

		if (eSkill == CRole::ATTACK || eSkill == CRole::AREA_BLAST) {	// 공격이면 달려가야함
			m_eState = RUN;
			m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::RUN_START, false);
		}
		else if (eSkill != CRole::SKILL_END) {	// 스킬이면 그자리에서 쓰도록 처리
			m_eState = SKILL;
			m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::SKILL, false);
		}
		else {
			m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::JUMP_LAND, false);
			m_eState = FINISH;
		}

	}
}

void CChr_Battle_Light_State_Attack::Attack(_float fTimeDelta)
{
	// if문 순서 : 
	// 1. 공중과 지상 구분
	// 2. 다음 공격과의 연계 체크
	// 3. 공격 종료시 다음 애니메이션 체크
	
	if(m_pChr_Battle_Light->Get_Current_Command() == CRole::AREA_BLAST)
		m_pChr_Battle_Light->Check_Interact_Weapon_Multi();
	else
		m_pChr_Battle_Light->Check_Interact_Weapon();

	// 1. 공중에서
	if (m_pChr_Battle_Light->Get_Transform()->Get_State_Float4(CTransform::STATE_POSITION).y > 1) {
		m_pChr_Battle_Light->Check_Interact_Weapon();
		if (!m_isCommandFinish) {
			if (m_pChr_Battle_Light->Get_CurrentTrackPosition() >= 23) {
				m_pChr_Battle_Light->Use_Command();
				CRole::SKILL eSkill = m_pChr_Battle_Light->Get_Current_Command();


				switch (eSkill) {
				case CRole::ATTACK: 
				{
					m_pChr_Battle_Light->Reset_Attakable();

					if (m_pChr_Battle_Light->Cal_Dist_Target() > 2.f) {
						m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::JUMP_FALL, false);
						m_eState = DOWN;
						return;
					}

					_uint iCurAnimation = m_pChr_Battle_Light->Get_CurrentAnimationIndex();
					_int nextAnimation = iCurAnimation;

					while (nextAnimation == iCurAnimation)
						nextAnimation = rand() % 3;

					m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::ANIMATION_CHR_BATTLE_LIGHT(nextAnimation), false);
				}
				break;
				case CRole::AREA_BLAST:
					m_pChr_Battle_Light->Reset_Attakable();
					m_eState = DOWN;
					m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::JUMP_FALL, true);
					break;
				case CRole::SKILL_END:
					m_isCommandFinish = true;
					break;
				default:
					m_pChr_Battle_Light->Reset_Attakable();
					m_eState = SKILL;
					m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::SKILL_AIR, false);
					break;
				}
			}
		}
		else {
			if (m_pChr_Battle_Light->Is_Animation_Finished()) {
				m_eState = DOWN;
				m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::JUMP_FALL, true);
			}
		}
	}
	else  { // 2. 지상에서 
		if (!m_isCommandFinish) {
			if (m_pChr_Battle_Light->Get_CurrentTrackPosition() >= 17) {
				m_pChr_Battle_Light->Use_Command();
				CRole::SKILL eSkill = m_pChr_Battle_Light->Get_Current_Command();

				switch (eSkill) {
				case CRole::ATTACK:
				{
					m_pChr_Battle_Light->Reset_Attakable();
					// 1. 지상에서의 거리 비교, 거리가 짧으면 다시 쫒아가야함
					if (m_pChr_Battle_Light->Cal_Dist_Target() > 2.f) {
						m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::RUN_START, false);
						m_eState = RUN;
						return;
					}

					// 2. Y축 거리 비교, 몬스터가 더 높이 있다면 점프해야함
					if (Get_Dist_Y() >= 3) {
						m_eState = UP;
						m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::JUMP_START, false);
						return;
					}

					// 3. 그 이외의 경우, 그 자리에서 즉시 다음 공격
					_uint iCurAnimation = m_pChr_Battle_Light->Get_CurrentAnimationIndex();
					_int nextAnimation = iCurAnimation;
					while (nextAnimation == iCurAnimation)
						nextAnimation = rand() % 4 + CChr_Battle_Light::ATTACK_NOR1;

					m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::ANIMATION_CHR_BATTLE_LIGHT(nextAnimation), false);
				}
					break;
				case CRole::AREA_BLAST:
					m_pChr_Battle_Light->Reset_Attakable();
					m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::ATTACK_AREABLAST, false);
					break;
				case CRole::SKILL_END:
					m_isCommandFinish = true;
					break;
				default:
					m_pChr_Battle_Light->Reset_Attakable();
					m_eState = SKILL;
					m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::SKILL, false);
					break;
				}
			}
		}
		else {
			if (m_pChr_Battle_Light->Is_Animation_Finished()) {
				CRole::SKILL eSkill = m_pChr_Battle_Light->Get_Current_Command();

				if (eSkill == CRole::SKILL_END) {
					m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::ATTACK_END, false);
					m_eState = FINISH;
				}
				else {
					m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::SKILL, false);
					m_eState = SKILL;
					m_isCommandFinish = false;
				}
			}
		}
	}
}

void CChr_Battle_Light_State_Attack::Skill(_float fTimeDelta)
{
	m_pChr_Battle_Light->Get_Transform()->Look_At_ForLandObject(XMLoadFloat4(&m_pChr_Battle_Light->Get_Target_Position()));

	if (m_pChr_Battle_Light->Get_Transform()->Get_State_Float4(CTransform::STATE_POSITION).y > 1) {
		if (!m_isCommandFinish) {
			if (!m_isCommandUse && m_pChr_Battle_Light->Get_CurrentTrackPosition() >= 16) {
				m_isCommandUse = true;
				m_pChr_Battle_Light->Use_Command();
				m_pChr_Battle_Light->Create_Sphere(m_pChr_Battle_Light->Get_Attack_Magic());
			}

			if (m_pChr_Battle_Light->Is_Animation_Finished()) {
				CRole::SKILL eSkill = m_pChr_Battle_Light->Get_Current_Command();

				switch (eSkill) {
				case CRole::ATTACK:
					m_eState = ATTACK;
					m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::ATTACK_AIR, false);
					m_isCommandUse = false;
					break;
				case CRole::AREA_BLAST:
					m_eState = DOWN;
					m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::JUMP_FALL, true);
					m_isCommandUse = false;
					break;
				case CRole::SKILL_END:
					m_isCommandFinish = true;
					break;
				default:
					m_pChr_Battle_Light->Set_TrackPosition(0.f);
					m_isCommandUse = false;
					break;
				}
			}
		}
		else {
			if (m_pChr_Battle_Light->Is_Animation_Finished()) {
				m_eState = DOWN;
				m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::JUMP_FALL, true);
			}
		}
	}
	else {
		if (!m_isCommandFinish) {
			if (!m_isCommandUse && m_pChr_Battle_Light->Get_CurrentTrackPosition() >= 14) {
				m_isCommandUse = true;
				m_pChr_Battle_Light->Use_Command();
				m_pChr_Battle_Light->Create_Sphere(m_pChr_Battle_Light->Get_Attack_Magic());
			}

			if (m_pChr_Battle_Light->Get_CurrentTrackPosition() >= 30) {
				CRole::SKILL eSkill = m_pChr_Battle_Light->Get_Current_Command();

				switch (eSkill) {
				case CRole::ATTACK:
				case CRole::AREA_BLAST:
				case CRole::SKILL_END:
					m_isCommandFinish = true;
					break;
				default:
					if (m_pChr_Battle_Light->Get_CurrentAnimationIndex() == CChr_Battle_Light::SKILL) {
						m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::SKILL_2, false);
						m_pChr_Battle_Light->Set_TrackPosition(15.f);
						m_isCommandUse = false;
					}
					else {
						m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::SKILL, false);
						m_pChr_Battle_Light->Set_TrackPosition(15.f);
						m_isCommandUse = false;
					}
					break;
				}
			}
		}
		else {
			if (m_pChr_Battle_Light->Is_Animation_Finished()) {
				CRole::SKILL eSkill = m_pChr_Battle_Light->Get_Current_Command();

				if (eSkill == CRole::SKILL_END) {
					m_pChr_Battle_Light->Change_State(CChr_Battle_Light::IDLE);
				}
				else {
					m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::RUN_START, false);
					m_eState = RUN;
					m_isCommandFinish = false;
				}
			}
		}
	}

}

void CChr_Battle_Light_State_Attack::Finish(_float fTimeDelta)
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
		m_pChr_Battle_Light->Update_ATB(fTimeDelta);
		if (m_pChr_Battle_Light->Get_CurrentTrackPosition() <= 25.f) {
			m_pChr_Battle_Light->Get_Transform()->Go_Backward(fTimeDelta * 2.f, m_pChr_Battle_Light->Get_Navigation());
		}
	}
}

_float CChr_Battle_Light_State_Attack::Get_Dist_Y()
{
	// Target과 현재 위치사이의 Y값 차이를 구한다 

	_vector vTargetPosition = XMLoadFloat4(&m_pChr_Battle_Light->Get_Target_Position());
	_float fTargetPositionY = vTargetPosition.m128_f32[1];

	_vector vCurrentPosition = m_pChr_Battle_Light->Get_Transform()->Get_State_Vector(CTransform::STATE_POSITION);
	_float fCurrentPositionY = vCurrentPosition.m128_f32[1];

	return fTargetPositionY - fCurrentPositionY;
}

CChr_Battle_Light_State_Attack* CChr_Battle_Light_State_Attack::Create(CChr_Battle_Light* pChr_Battle_Light)
{
	CChr_Battle_Light_State_Attack* pInstance = new CChr_Battle_Light_State_Attack(pChr_Battle_Light);

	return pInstance;
}

void CChr_Battle_Light_State_Attack::Free()
{
	__super::Free();
}
