#include "stdafx.h"
#include "Role.h"

const wstring CRole::strSkillName[SKILL_END] = {TEXT("�ο��") , TEXT("������ ����Ʈ") , TEXT("����") ,
                    TEXT("����"), TEXT("����"), TEXT("�����"), TEXT("���̾�"), TEXT("�÷��� ��ο�"),
                    TEXT("�ɾ�") };

CRole::CRole()
    : CBase{}
{
}

wstring CRole::Get_SkillName(SKILL eSkill)
{
    if (eSkill >= SKILL_END)
        return L"";

    return strSkillName[eSkill];
}

_int CRole::Get_Skill_Cost(SKILL eSkill)
{
    auto iter = std::find_if(m_Skill.begin(), m_Skill.end(),
        [&eSkill](pair<SKILL, _uint>& skill) {
            return skill.first == eSkill;
        });
    return iter->second;
}

CRole::SKILL CRole::Get_Skill_Index(size_t iNum)
{
    auto item = m_Skill.begin();

    for (int i = 0; i < iNum; ++i)
        ++item;

    return item->first;
}

HRESULT CRole::Add_Skill(SKILL eSkill, _uint iCost)
{
    auto iter = std::find_if(m_Skill.begin(), m_Skill.end(),
        [&eSkill](pair<SKILL, _uint>& skill) {
            return skill.first == eSkill;
        });

    if (iter == m_Skill.end()) {
        m_Skill.push_back({ eSkill, iCost });
        return S_OK;
    }

    return E_FAIL;
}

HRESULT CRole::Delete_Skill(SKILL eSkill)
{
    auto iter = std::find_if(m_Skill.begin(), m_Skill.end(),
        [&eSkill](pair<SKILL, _uint>& skill) {
            return skill.first == eSkill;
        });

    if (iter == m_Skill.end()) {
        return E_FAIL;
    }
    m_Skill.erase(iter);
    return S_OK;
}

CRole* CRole::Create()
{
    CRole* pInstance = new CRole();
	return pInstance;
}

void CRole::Free()
{
    __super::Free();
    m_Skill.erase(m_Skill.begin(), m_Skill.end());
    m_Skill.clear();
}
