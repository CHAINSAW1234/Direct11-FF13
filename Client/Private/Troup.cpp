#include "stdafx.h"
#include "Troup.h"

#include "Monster.h"
#include "GameInstance.h"
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
	if (!m_isCollision && Check_Collision()){
		m_isCollision = true;
		Start_Battle();	
		Save_Troup_For_Battle();
		return true;
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

void CTroup::Start_Battle()
{
	CChr_Field* pChr_Field = (CChr_Field*)(m_pGameInstance->Get_GameObject(g_Level, g_strChrLayerTag, 0));
	pChr_Field->Set_State_Battle_Start();

	for (auto& pMonsters : m_Monsters) {
		pMonsters.second->Set_State_Battle_Start();
	}
}

HRESULT CTroup::Add_Collider()
{
	/* Com_Collider_Body */
	CBounding_OBB::BOUNDING_OBB_DESC		ColliderOBBDesc{};

	/* 로컬상의 정보를 셋팅한다. */
	ColliderOBBDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColliderOBBDesc.vSize = _float3(6.f, 3.f, 3.f);
	ColliderOBBDesc.vCenter = _float3(0.f, 0.f, 26.f);

	CComponent* pComponent = m_pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), &ColliderOBBDesc);
	if (nullptr == pComponent)
		return E_FAIL;

	m_pColliderCom = (CCollider*)pComponent;

	return S_OK;


}

_bool CTroup::Check_Collision()
{
	CCollider* pTargetCollider = dynamic_cast<CChr_Field*>(m_pGameInstance->Get_GameObject(g_Level, g_strChrLayerTag, 0))->Get_Collider();
	if (nullptr == pTargetCollider)
		return false;

	if (nullptr != m_pColliderCom) {
		if (m_pColliderCom->Intersect(pTargetCollider))
			return true;

		m_pColliderCom->Render();
	}

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

	if (g_Level == LEVEL_FIELD_BOSS) {
		if(FAILED(pInstance->Add_Collider()))
			Safe_Release(pInstance);
	}

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


	Safe_Release(m_pColliderCom);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
}
