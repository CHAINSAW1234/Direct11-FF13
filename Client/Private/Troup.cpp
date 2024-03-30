#include "stdafx.h"
#include "Troup.h"

#include "Monster.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Chr_Field.h"

CTroup::CTroup(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBase{}
{
	m_pDevice = pDevice;
	m_pContext = pContext;
	m_pGameInstance = CGameInstance::Get_Instance();

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

void CTroup::Tick()
{
	if (Check_Collision())
		if (m_pGameInstance->Get_KeyState(KEY_DOWN, DIK_RETURN)) {
			Write_Troup();
			m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_BATTLE));
		}

}

void CTroup::Add_Monster(wstring strPrototypeTag, CMonster* pMonster)
{
	++m_iNumMonsters;
	m_Monsters.push_back({ strPrototypeTag, pMonster });
	Safe_AddRef(pMonster);
}

_bool CTroup::Check_Collision()
{
	CCollider* pTargetCollider = dynamic_cast<CChr_Field*>(m_pGameInstance->Get_GameObject(g_Level, TEXT("Layer_Chr"), 0))->Get_Collider();
	if (nullptr == pTargetCollider)
		return false;

	for (auto& pMonster : m_Monsters) {
		if (pMonster.second->Get_Collider()->Intersect(pTargetCollider))
			return true;
	}

	return false;
}

void CTroup::Write_Troup()
{
	string filepath = "../Bin/DataFiles/Troup.dat";
	ofstream OFS{filepath, ios::out | ios::binary};

	OFS.write(reinterpret_cast<const char*>(&m_iNumMonsters), sizeof(_int));
	for (_int i = 0; i < m_iNumMonsters; ++i) {
		size_t iStringNum = wcslen(m_Monsters[i].first.c_str());

		OFS.write(reinterpret_cast<const char*>(&iStringNum), sizeof(size_t));
		OFS.write(reinterpret_cast<const char*>(m_Monsters[i].first.c_str()), sizeof(_tchar) * iStringNum);
	}

	OFS.close();
}

CTroup* CTroup::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTroup* pInstance = new CTroup(pDevice, pContext);

	return pInstance;
}

void CTroup::Free()
{
	for (auto& pMonster : m_Monsters)
		Safe_Release(pMonster.second);
	m_Monsters.clear();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
}
