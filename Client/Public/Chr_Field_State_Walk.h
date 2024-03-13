#pragma once
#include "Client_Defines.h"
#include "FSM_State.h"

BEGIN(Client)

class CChr_Field_State_Walk final : public CFSM_State
{
private:
	CChr_Field_State_Walk(class CChr_Field* pChr_Field);
	virtual ~CChr_Field_State_Walk() = default;

public:
	// 상태 진입했을 때 처음만 호출
	virtual void OnStateEnter() override;
	// 상태 진입되어 있는 상태에서 매 tick마다 호출
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// 상태가 변경될 때 호출
	virtual void OnStateExit() override;

private:
	_float4 Cal_Target_Direction();
	_float	Cal_Degree_From_Directions_Between_Min180_To_180(_float4 vDir1, _float4 vDir2);
	void	Update_Animation();
private:
	class CChr_Field* m_pChr_Field = { nullptr };
	_float4			  vTargetDirection;
public:
	static	CChr_Field_State_Walk* Create(class CChr_Field* pChr_Field);
	virtual void				  Free() override;

};

END
