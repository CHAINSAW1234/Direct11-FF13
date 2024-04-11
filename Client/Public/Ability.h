#pragma once
#include "Client_Defines.h"
#include "Base.h"
#include "Role.h"

BEGIN(Client)
class CAbility final : public CBase
{
public:
	enum ROLE { ATTACKER, BLASTER, HEALER, ROLE_END };

private:
	const static wstring strRoleName[ROLE_END];
	const static wstring strRoleFullName[ROLE_END];
	const static _float4 vRoleColor[ROLE_END];
private:
	CAbility();
	~CAbility() = default;

public:
	static wstring Get_RoleName(ROLE eRole) { return strRoleName[eRole]; }
	static wstring Get_RoleFullName(ROLE eRole) { return strRoleFullName[eRole]; }
	static _float4 Get_RoleColor(ROLE eRole) { return vRoleColor[eRole]; }

	size_t	Get_RoleCount() { return m_Role.size(); }
	
	ROLE	Get_CurrentRole() { return m_eCurrentRole; }
	CRole* Get_CurrentSkillSet();
	CRole* Get_Role(ROLE eRole);
	HRESULT Add_Role(ROLE eRole);
	HRESULT Delete_Role(ROLE eRole);
	void	Set_CurrentRole(ROLE eRole);

public:
	// Role¿¡ °ü·ÃµÊ
	_int Get_Skill_Cost(ROLE eRole, CRole::SKILL eSkill);
	CRole::SKILL Get_Skill_Index(ROLE eRole, size_t iNum);
	HRESULT Add_Skill(ROLE eRole, CRole::SKILL eSkill, _uint iCost);
	HRESULT Delete_Skill(ROLE eRole, CRole::SKILL eSkill);

private:
	CRole* Find_Role(ROLE eRole);

private:
	map<ROLE, CRole*>	m_Role;
	ROLE				m_eCurrentRole = {ROLE_END};

public:
	static CAbility* Create();
	void Free();

};

END