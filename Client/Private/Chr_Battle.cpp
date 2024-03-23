#include "stdafx.h"
#include "Chr_Battle.h"

#include "FSM.h"
#include "Model.h"
#include "Shader.h"

#include "Body.h"
#include "Ability.h"



CChr_Battle::CChr_Battle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CChr_Battle::CChr_Battle(const CChr_Battle& rhs)
	: CGameObject{ rhs }
	, m_strChrName{ rhs.m_strChrName }
	, m_iMaxHp{ rhs.m_iMaxHp }
	, m_iHp{ rhs.m_iMaxHp}
{
}

HRESULT CChr_Battle::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CChr_Battle::Initialize(void* pArg)
{
	m_eLevel = g_Level;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	if (FAILED(Add_PartObjects()))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_Ability()))
		return E_FAIL;


	return S_OK;
}

void CChr_Battle::Tick(_float fTimeDelta)
{
	m_pFSMCom->Update(fTimeDelta);

}

HRESULT CChr_Battle::Late_Tick(_float fTimeDelta)
{
	if (FAILED(__super::Late_Tick(fTimeDelta)))
		return E_FAIL;

	for (auto& Parts : m_PartObjects)
		Parts->Late_Tick(fTimeDelta);

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

HRESULT CChr_Battle::Add_Components()
{
	if (FAILED(Add_Component_FSM()))
		return E_FAIL;

	return S_OK;
}

HRESULT CChr_Battle::Add_Component_FSM()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_FSM"),
		TEXT("Com_FSM"), (CComponent**)&m_pFSMCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CChr_Battle::Add_PartObjects()
{
	return S_OK;
}

_float4 CChr_Battle::Get_Look()
{
	// Player의 Look vector를 Y값을 지우고 리턴
	_float4 vChrLook = m_pTransformCom->Get_State_Float4(CTransform::STATE_LOOK);
	vChrLook.y = 0;
	XMStoreFloat4(&vChrLook, XMVectorSetW(XMVector3Normalize(XMLoadFloat4(&vChrLook)), 0.f));

	return vChrLook;
}

_uint CChr_Battle::Get_CurrentAnimationIndex()
{
	return dynamic_cast<CBody*>(m_PartObjects[0])->Get_CurrentAnimationIndex();
}

_float CChr_Battle::Get_CurrentTrackPosition()
{
	return dynamic_cast<CBody*>(m_PartObjects[0])->Get_CurrentTrackPosition();
}

_bool CChr_Battle::Is_Animation_Finished()
{
	return dynamic_cast<CBody*>(m_PartObjects[0])->Is_Animation_Finished();
}

void CChr_Battle::Add_Hp(_int iHp)
{
	m_iHp += iHp;
	m_iHp = min(m_iHp, m_iMaxHp);
}

void CChr_Battle::Min_Hp(_int iHp)
{
	m_iHp -= iHp;
	m_iHp = max(m_iHp, 0);
}

void CChr_Battle::Set_TrackPosition(_float fTrackPosition)
{
	dynamic_cast<CBody*>(m_PartObjects[0])->Set_TrackPosition(fTrackPosition);
}

void CChr_Battle::Set_Target(CGameObject* pTargetObject)
{
	if (nullptr != m_pTargetObject)
		Safe_Release(m_pTargetObject);

	m_pTargetObject = pTargetObject;
	Safe_AddRef(m_pTargetObject);
}

void CChr_Battle::Update_ATB(_float fTimeDelta)
{
	m_fATB += fTimeDelta * 0.8;
	m_fATB = min(m_fATB, m_fMaxATB);

}

void CChr_Battle::Update_Command()
{
	if (!m_Commands.empty())
		m_iNextCommandCount = (_int)m_Commands.size();
	else {

		m_iNextCommandCount = rand() % 3 + 1;
		CRole* pRole = m_pAbility->Get_CurrentRole();

		CRole::SKILL eSkill = pRole->Get_Skill_Index(rand()% pRole->Get_SkillSet_Count());

		for(int i=0;i<m_iNextCommandCount;++i)
			m_Commands.push(eSkill);
	}

}

void CChr_Battle::Use_Command()
{
	if (m_Commands.empty())
		return;

	m_iNextCommandCount -= 1;
	m_Commands.pop();
	m_fATB -= 1.f;
}

CRole::SKILL CChr_Battle::Get_Current_Command()
{
	if (m_Commands.empty())
		return CRole::SKILL_END;

	return m_Commands.front();
}

void CChr_Battle::Free()
{
	__super::Free();

	for (auto& iter : m_PartObjects)
		Safe_Release(iter);
	m_PartObjects.clear();

	Safe_Release(m_pFSMCom);
	Safe_Release(m_pAbility);
	Safe_Release(m_pTargetObject);
}
