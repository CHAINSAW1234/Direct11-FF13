#include "stdafx.h"
#include "Chr_Battle.h"

#include "FSM.h"
#include "Model.h"
#include "Shader.h"

#include "Body.h"
#include "Ability.h"
#include "Weapon_Anim.h"
#include "UI_Number.h"
#include "Monster.h"

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

	Update_Target();
	Check_Interact_Chr();
	Check_Interact_Monster();

	if(nullptr != m_pColliderCom)
		m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	for (auto& pPartObject : m_PartObjects)
		pPartObject->Tick(fTimeDelta);
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

#ifdef _DEBUG
	if (nullptr != m_pColliderCom)
		m_pColliderCom->Render();
#endif 

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

_float4 CChr_Battle::Get_Target_Position()
{
	if (nullptr == m_pTargetObject)
		return _float4(0.f, 0.f, 0.f, 1.f);

	return ((CTransform*)m_pTargetObject->Get_Component(g_strTransformTag))->Get_State_Float4(CTransform::STATE_POSITION);
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

CCollider* CChr_Battle::Get_Collider_Weapon()
{
	return dynamic_cast<CWeapon_Anim*>(m_PartObjects[1])->Get_Collider();
}



void CChr_Battle::Add_Hp(_int iHp)
{
	m_iHp += iHp;
	m_iHp = min(m_iHp, m_iMaxHp);
	NotifyObserver();
}

void CChr_Battle::Min_Hp(_int iHp)
{
	m_iHp -= iHp;
	m_iHp = max(m_iHp, 0);
	NotifyObserver();
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

void CChr_Battle::Reset_Attakable()
{
	for (auto& i : m_isAttackable)
		i = true;
}

void CChr_Battle::Set_Hit(_int iDamage)
{
}

void CChr_Battle::Create_Damage(_int iDamage)
{
	CUI_Number::UI_NUMBER_DESC UI_Number_desc = {};
	UI_Number_desc.eType = CUI_Number::HIT;
	UI_Number_desc.iNumber = iDamage;
	UI_Number_desc.vPosition = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

	if (FAILED(m_pGameInstance->Add_Clone(g_Level, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Number"), &UI_Number_desc)))
		return;
}

void CChr_Battle::Update_ATB(_float fTimeDelta)
{
	m_fATB += fTimeDelta * 0.8f;
	m_fATB = min(m_fATB, m_fMaxATB);

}

void CChr_Battle::Update_Target()
{
	if (nullptr == m_pTargetObject) {
		return;
	}

	if (m_pTargetObject->Get_Dead()) {
		Safe_Release(m_pTargetObject);
		m_pTargetObject = m_pGameInstance->Get_GameObject(g_Level, g_strMonsterLayerTag, 0);

		if (nullptr != m_pTargetObject) {
			Safe_AddRef(m_pTargetObject);
			((CMonster*)m_pTargetObject)->Set_isTarget(true);
		}
	}

}

void CChr_Battle::Change_Role(CAbility::ROLE eRole)
{
	m_pAbility->Set_CurrentRole(eRole);
}


void CChr_Battle::Update_Command()
{
	if (!m_Commands.empty())
		m_iNextCommandCount = (_int)m_Commands.size();
	else {

		m_iNextCommandCount = rand() % 3 + 1;
		CRole* pRole = m_pAbility->Get_CurrentSkillSet();

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

_float CChr_Battle::Cal_Degree_Start()
{
	_float4 vLook = m_pTransformCom->Get_State_Float4(CTransform::STATE_LOOK);
	_vector vDir_to_Start = XMLoadFloat4(&m_vStartPosition) - m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);
	vDir_to_Start.m128_f32[1] = 0.f;
	vDir_to_Start = XMVector3Normalize(vDir_to_Start);

	_float4 vTargetLook;
	XMStoreFloat4(&vTargetLook, vDir_to_Start);
	return Cal_Degree_From_Directions_Between_Min180_To_180(vLook, vTargetLook);
}

_float CChr_Battle::Cal_Dist_Start()
{
	_vector vDist = XMLoadFloat4(&m_vStartPosition) - m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);
	vDist.m128_f32[1] = 0.f;

	return XMVector3Length(vDist).m128_f32[0];
}

_float CChr_Battle::Cal_Degree_Target()
{
	if (nullptr == m_pTargetObject) {
		return Cal_Degree_Start();
	}

	_float4 vLook = m_pTransformCom->Get_State_Float4(CTransform::STATE_LOOK);
	_vector vDir_to_Target = dynamic_cast<CTransform*>(m_pTargetObject->Get_Component(g_strTransformTag))->Get_State_Vector(CTransform::STATE_POSITION) - m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);
	vDir_to_Target.m128_f32[1] = 0.f;
	vDir_to_Target = XMVector3Normalize(vDir_to_Target);

	_float4 vTargetLook;
	XMStoreFloat4(&vTargetLook, vDir_to_Target);
	return Cal_Degree_From_Directions_Between_Min180_To_180(vLook, vTargetLook);
}

_float CChr_Battle::Cal_Dist_Target()
{
	if (nullptr == m_pTargetObject) {
		return Cal_Dist_Start();
	}
	CMonster* pMonster = dynamic_cast<CMonster*>(m_pTargetObject);
	if (nullptr == pMonster)
		return Cal_Dist_Start();

	_float fDist = INFINITY;

	_vector vPos = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);
	vPos.m128_f32[1] = pMonster->Get_Transform()->Get_State_Float4(CTransform::STATE_POSITION).y;
	_vector vLook = m_pTransformCom->Get_State_Vector(CTransform::STATE_LOOK);
	
	if(pMonster->Get_Collider()->IntersectRay(vPos, vLook, fDist))
		return fDist - m_fColliderSizeZ;
	
	return INFINITY;
}

void CChr_Battle::Check_Interact_Chr()
{
	_int iSizeChr = (_int)m_pGameInstance->Get_LayerCnt(g_Level, g_strChrLayerTag);
	for (int i = 0; i < iSizeChr; ++i) {
		CChr_Battle* pChr_Battle = dynamic_cast<CChr_Battle*>(m_pGameInstance->Get_GameObject(g_Level, g_strChrLayerTag, i));

		if (nullptr == pChr_Battle)
			continue;
		if (this == pChr_Battle)
			continue;

		if (m_pColliderCom->Intersect(pChr_Battle->Get_Collider())) {
			pChr_Battle->Get_Transform()->Set_State(CTransform::STATE_POSITION,
				pChr_Battle->Get_Transform()->Get_State_Vector(CTransform::STATE_POSITION) + m_pTransformCom->Get_LastMovement_Vector());
		}

		// 중점 간의 방향으로 밀기
		if (m_pColliderCom->Intersect(pChr_Battle->Get_Collider())) {
			_vector VectorDir = pChr_Battle->Get_Transform()->Get_State_Vector(CTransform::STATE_POSITION) - m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);
			VectorDir.m128_f32[1] = 0.f;
			VectorDir = XMVector3Normalize(VectorDir);
			// 미는 힘은 힘으로 처리함
			pChr_Battle->Get_Transform()->Move_To_Direction(VectorDir, (XMVector3Length(m_pTransformCom->Get_LastMovement_Vector()) * 2.f / pChr_Battle->Get_Transform()->Get_SpeedPerSec()).m128_f32[0]);
		}
	}
}

void CChr_Battle::Check_Interact_Monster()
{
	_int iSizeMonster = (_int)m_pGameInstance->Get_LayerCnt(g_Level, g_strMonsterLayerTag);
	for (int i = 0; i < iSizeMonster; ++i) {
		CMonster* pMonster = dynamic_cast<CMonster*>(m_pGameInstance->Get_GameObject(g_Level, g_strMonsterLayerTag, i));

		if (nullptr == pMonster)
			continue;

		// 중점 간의 방향으로 밀기
		if (m_pColliderCom->Intersect(pMonster->Get_Collider())) {
			_vector VectorDir = pMonster->Get_Transform()->Get_State_Vector(CTransform::STATE_POSITION) - m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);
			VectorDir.m128_f32[1] = 0.f;
			VectorDir = XMVector3Normalize(VectorDir);
			// 미는 힘은 힘으로 처리함
			pMonster->Get_Transform()->Move_To_Direction(VectorDir, (XMVector3Length(m_pTransformCom->Get_LastMovement_Vector()) * 2.f / pMonster->Get_Transform()->Get_SpeedPerSec()).m128_f32[0]);
		}
	}
}

void CChr_Battle::Free()
{
	__super::Free();

	for (auto& iter : m_PartObjects)
		Safe_Release(iter);
	m_PartObjects.clear();

	Safe_Release(m_pFSMCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pAbility);
	Safe_Release(m_pTargetObject);
}
