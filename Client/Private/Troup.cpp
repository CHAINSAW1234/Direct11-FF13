#include "stdafx.h"
#include "Troup.h"

#include "Monster.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Level_Battle.h"
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

_bool CTroup::Tick()
{
	if (Check_Collision())
		if (m_pGameInstance->Get_KeyState(KEY_DOWN, DIK_RETURN)) {
			Save_Troup_For_Battle();
			m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_BATTLE));
			return true;
			//m_pGameInstance->Open_Level(LEVEL_BATTLE, CLevel_Battle::Create(m_pDevice, m_pContext));
		}

	return false;
}

void CTroup::Add_Monster(wstring strPrototypeTag, CMonster* pMonster)
{
	++m_iNumMonsters;
	m_Monsters.push_back({ strPrototypeTag, pMonster });
	Safe_AddRef(pMonster);
}

void CTroup::Save_Troup(ofstream& OFS)
{
	OFS.write(reinterpret_cast<const char*>(&m_iNumMonsters), sizeof(_int));
	for (_int i = 0; i < m_iNumMonsters; ++i) {
		size_t iStringNum = wcslen(m_Monsters[i].first.c_str());

		OFS.write(reinterpret_cast<const char*>(&iStringNum), sizeof(size_t));
		OFS.write(reinterpret_cast<const char*>(m_Monsters[i].first.c_str()), sizeof(_tchar) * iStringNum);
		OFS.write(reinterpret_cast<const char*>(&m_Monsters[i].second->Get_StartPosition()), sizeof(_float4));
	}
}

HRESULT CTroup::Load_Troup(ifstream& IFS)
{
	IFS.read(reinterpret_cast<char*>(&m_iNumMonsters), sizeof(_int));

	for (_int i = 0; i < m_iNumMonsters; ++i) {
		size_t iStringNum = { 0 };
		_tchar strPrototypeTag[MAX_PATH] = {};
		_float4 vStartPosition = { 0.f,0.f,0.f,1.f };

		IFS.read(reinterpret_cast<char*>(&iStringNum), sizeof(size_t));
		IFS.read(reinterpret_cast<char*>(&strPrototypeTag), sizeof(_tchar) * iStringNum);
		IFS.read(reinterpret_cast<char*>(&vStartPosition), sizeof(_float4));
		CMonster* pMonster = (CMonster*)(m_pGameInstance->Add_Clone_With_Object(g_Level, g_strMonsterLayerTag, strPrototypeTag));
		pMonster->Set_StartPosition(vStartPosition);
		m_Monsters.push_back({ strPrototypeTag,pMonster });
		Safe_AddRef(pMonster);
	}

	return S_OK;
}

_bool CTroup::Check_Collision()
{
	CCollider* pTargetCollider = dynamic_cast<CChr_Field*>(m_pGameInstance->Get_GameObject(g_Level, g_strChrLayerTag, 0))->Get_Collider();
	if (nullptr == pTargetCollider)
		return false;

	for (auto& pMonster : m_Monsters) {
		if (pMonster.second->Get_Collider()->Intersect(pTargetCollider))
			return true;
	}

	return false;
}

void CTroup::Save_Troup_For_Battle()
{
	string filepath = "../Bin/DataFiles/Troup.dat";
	ofstream OFS{filepath, ios::out | ios::binary};

	Save_Troup(OFS);

	OFS.close();
}

CTroup* CTroup::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTroup* pInstance = new CTroup(pDevice, pContext);

	return pInstance;
}

CTroup* CTroup::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ifstream& IFS)
{
	CTroup* pInstance = new CTroup(pDevice, pContext);
	
	if (FAILED(pInstance->Load_Troup(IFS))) {
		Safe_Release(pInstance);
	}

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
