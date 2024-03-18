#include "stdafx.h"
#include "Player_Battle.h"

HRESULT CPlayer_Battle::Initialize()
{
	return S_OK;
}

void CPlayer_Battle::Start()
{
}

void CPlayer_Battle::Tick(_float fTimeDelta)
{
}

HRESULT CPlayer_Battle::Render()
{
	return S_OK;
}


CPlayer_Battle* CPlayer_Battle::Create()
{
	CPlayer_Battle* pInstance = new CPlayer_Battle();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayer_Battle"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Battle::Free()
{
	for (auto& pChr_Battle : m_Memebers)
		Safe_Release(pChr_Battle);
	m_Memebers.clear();

}
