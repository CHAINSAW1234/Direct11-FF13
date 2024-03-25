#include "stdafx.h"
#include "Optima.h"

COptima::COptima()
	:CBase{}
{
}

COptima::Optima* COptima::Get_Optima(_int iOptimaIndex)
{
	if (iOptimaIndex >= m_Optimas.size())
		return nullptr;


	return m_Optimas[iOptimaIndex].second;
}

wstring COptima::Get_Optima_Name(_int iOptimaIndex)
{
	if (iOptimaIndex >= m_Optimas.size())
		return TEXT("");


	return m_Optimas[iOptimaIndex].first;
}

HRESULT COptima::Change_Optima(_int iOptimaIndex)
{
	if (iOptimaIndex >= m_Optimas.size())
		return E_FAIL;

	m_iCurrentOptima = iOptimaIndex;

	return S_OK;
}

HRESULT COptima::Add_Optima(wstring strOptimaName)
{
	auto iter = std::find_if(m_Optimas.begin(), m_Optimas.end(),
		[&strOptimaName](pair<wstring, Optima*>& iter) {
			return iter.first == strOptimaName;
		});


	if (iter == m_Optimas.end()) {
		Optima* pOptima = new Optima[3];
		*pOptima[0] = *pOptima[1] = *pOptima[2] = { CAbility::ROLE_END };
		m_Optimas.push_back({ strOptimaName, pOptima });
		return S_OK;
	}

	return E_FAIL;
}

HRESULT COptima::Delete_Optima(wstring strOptimaName)
{
	auto iter = std::find_if(m_Optimas.begin(), m_Optimas.end(),
		[&strOptimaName](pair<wstring, Optima*>& iter) {
			return iter.first == strOptimaName;
		});

	if (iter == m_Optimas.end()) {
		return E_FAIL;
	}

	Safe_Delete_Array(iter->second);
	m_Optimas.erase(iter);

	return S_OK;
}

HRESULT COptima::Delete_Optima(_int iOptimaIndex)
{
	if (iOptimaIndex >= m_Optimas.size())
		return E_FAIL;


	Safe_Delete_Array(m_Optimas[iOptimaIndex].second);
	m_Optimas.erase(m_Optimas.begin() + iOptimaIndex);

	return S_OK;
}

HRESULT COptima::Change_Role(wstring strOptimaName, _int iOptimaTargetIndex, CAbility::ROLE eRole)
{
	auto iter = std::find_if(m_Optimas.begin(), m_Optimas.end(),
		[&strOptimaName](pair<wstring, Optima*>& iter) {
			return iter.first == strOptimaName;
		});

	if (iter == m_Optimas.end()) {
		return E_FAIL;
	}

	*iter->second[iOptimaTargetIndex] = eRole;


	return S_OK;
}

HRESULT COptima::Change_Role(_int iOptimaIndex, _int iOptimaTargetIndex, CAbility::ROLE eRole)
{
	if (iOptimaIndex >= m_Optimas.size())
		return E_FAIL;

	if (iOptimaTargetIndex >= 3)
		return E_FAIL;


	(*m_Optimas[iOptimaIndex].second)[iOptimaTargetIndex] = eRole;

	return S_OK;
}

COptima* COptima::Create()
{
	COptima* pInstance = new COptima;

	return pInstance;
}

void COptima::Free()
{
	__super::Free();
	for (auto& iter : m_Optimas)
		Safe_Delete_Array(iter.second);
	m_Optimas.clear();



}
