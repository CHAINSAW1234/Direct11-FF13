#include "stdafx.h"
#include "Ability.h"

const wstring CAbility::strRollName[ROLE_END] = { TEXT("ATK"), TEXT("BLA"), TEXT("HLR") };

CAbility::CAbility()
	: CBase{}
{
}


CRole* CAbility::Get_Role(ROLE eRole)
{
	CRole* pRole = Find_Role(eRole);

	return pRole;
}

CRole* CAbility::Get_CurrentRole()
{
	return Find_Role(m_eCurrentRole);
}

HRESULT CAbility::Add_Role(ROLE eRole)
{
	CRole* pRole = Find_Role(eRole);

	if (nullptr != pRole) {
		return E_FAIL;
	}

	pRole = CRole::Create();
		
	m_Role.emplace(eRole, pRole);

	return S_OK;
}

HRESULT CAbility::Delete_Role(ROLE eRole)
{
	CRole* pRole = Find_Role(eRole);

	if (nullptr == pRole) {
		return E_FAIL;
	}

	m_Role.erase(eRole);

	return S_OK;
}

void CAbility::Set_CurrentRole(ROLE eRole) {
	CRole* pRole = Find_Role(eRole);
	if (nullptr == pRole) {
		return ;
	}

	m_eCurrentRole = eRole;
}

_int CAbility::Get_Skill_Cost(ROLE eRole, CRole::SKILL eSkill)
{
	CRole* pRole = Find_Role(eRole);

	if (nullptr == pRole) {
		return INFINITE;
	}

	return pRole->Get_Skill_Cost(eSkill);
}

CRole::SKILL CAbility::Get_Skill_Index(ROLE eRole, size_t iNum)
{
	CRole* pRole = Find_Role(eRole);

	if (nullptr == pRole) {
		return CRole::SKILL_END;
	}

	return pRole->Get_Skill_Index(iNum);
}

HRESULT CAbility::Add_Skill(ROLE eRole, CRole::SKILL eSkill, _uint iCost)
{
	CRole* pRole = Find_Role(eRole);

	if (nullptr == pRole) {
		return E_FAIL;
	}


	return pRole->Add_Skill(eSkill, iCost);
}

HRESULT CAbility::Delete_Skill(ROLE eRole, CRole::SKILL eSkill)
{
	CRole* pRole = Find_Role(eRole);

	if (nullptr == pRole) {
		return E_FAIL;
	}

	return pRole->Delete_Skill(eSkill);
}

CRole* CAbility::Find_Role(ROLE eRole)
{
	if (eRole >= ROLE_END)
		return nullptr;

	auto iter = m_Role.find(eRole);

	if (iter == m_Role.end())
		return nullptr;

	return iter->second;
}


CAbility* CAbility::Create()
{
	CAbility* pInstance = new CAbility();

	return pInstance;
}

void CAbility::Free()
{
	__super::Free();

	for (auto& iter : m_Role)
		Safe_Release(iter.second);
	m_Role.clear();

}
