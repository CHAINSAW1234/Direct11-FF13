#include "stdafx.h"
#include "Chr_Battle.h"
#include "Ability.h"

CChr_Battle::CChr_Battle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CChr_Battle::CChr_Battle(const CChr_Battle& rhs)
	: CGameObject{ rhs}
{
}

HRESULT CChr_Battle::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CChr_Battle::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	if(FAILED(Add_Ability()))
		return E_FAIL;

	return S_OK;
}

void CChr_Battle::Tick(_float fTimeDelta)
{

}

HRESULT CChr_Battle::Late_Tick(_float fTimeDelta)
{
	if (FAILED(__super::Late_Tick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CChr_Battle::Render()
{
	return S_OK;
}


HRESULT CChr_Battle::Add_Ability()
{
	return S_OK;
}

void CChr_Battle::Update_ATB(_float fTimeDelta)
{
	if (!m_isUpdateATB)
		return;

	m_fATB += fTimeDelta;
	m_fATB = min(m_fATB, m_fMaxATB);

}

void CChr_Battle::Free()
{
	__super::Free();

	Safe_Release(m_pAbility);
}
