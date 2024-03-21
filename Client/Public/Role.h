#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CRole final : public CBase
{
public:
	enum SKILL { ATTACK, AREA_BLAST, RUIN,
				 THUNDER, WATER, AERO, FIRE, FLAMEBLOW,
				 CURE, SKILL_END};
private:
	const static wstring strSkillName[SKILL_END];
public:
	size_t Get_SkillSet_Count() { return m_Skill.size(); }

private:
	CRole();
	~CRole() = default;

public:
	static wstring Get_SkillName(SKILL eSkill); 
	_int Get_Skill_Cost(SKILL eSkill);
	SKILL Get_Skill_Index(size_t iNum);

	HRESULT Add_Skill(SKILL eSkill, _uint iCost);
	HRESULT Delete_Skill(SKILL eSkill);
private:
	list<pair<SKILL, _uint>> m_Skill;
	
public:
	static CRole* Create();
	void Free();

};

END