#include "stdafx.h"
#include "Chr_Battle_Vanila.h"
#include "Chr_Battle_Vanila_State_Idle.h"
#include "Chr_Battle_Vanila_State_Skill.h"
#include "Chr_Battle_Vanila_State_Heal.h"
#include "Chr_Battle_Vanila_State_Hit.h"
#include "Chr_Battle_Vanila_State_Dead.h"
#include "Chr_Battle_Vanila_State_Optima.h"
#include "Chr_Battle_Vanila_State_Finish.h"

#include "FSM.h"
#include "Model.h"
#include "Shader.h"

#include "Body.h"
#include "Weapon_Anim.h"
#include "Ability.h"
#include "Sphere_Heal.h"

#include "ImGUI_Manager.h"

CChr_Battle_Vanila::CChr_Battle_Vanila(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CChr_Battle{ pDevice, pContext }
{
}

CChr_Battle_Vanila::CChr_Battle_Vanila(const CChr_Battle_Vanila& rhs)
	: CChr_Battle{ rhs }
{
}

HRESULT CChr_Battle_Vanila::Initialize_Prototype()
{
	m_strChrName = TEXT("바닐라");
	m_iMaxHp = m_iHp = 350;
	m_iAttack_Physic = 25;
	m_iAttack_Magic = 26;
	m_vColliderSize = _float3(.6f, 1.8f, .6f);
	return S_OK;
}

HRESULT CChr_Battle_Vanila::Initialize(void* pArg)
{
	GAMEOBJECT_DESC		GameObjectDesc{};

	GameObjectDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(360.f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;


	//m_pImGUI_Manager = CImGUI_Manager::Get_Instance(m_pDevice, m_pContext);
	//Safe_AddRef(m_pImGUI_Manager);
	return S_OK;
}

void CChr_Battle_Vanila::Tick(_float fTimeDelta)
{
		__super::Tick(fTimeDelta);
}

HRESULT CChr_Battle_Vanila::Late_Tick(_float fTimeDelta)
{
	if (FAILED(__super::Late_Tick(fTimeDelta)))
		return E_FAIL;

	// 임시
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
	return S_OK;
}

HRESULT CChr_Battle_Vanila::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CChr_Battle_Vanila::Start()
{
	Set_Target(m_pGameInstance->Get_GameObject(g_Level, g_strMonsterLayerTag, 0));
	m_pTransformCom->Look_At_ForLandObject(((CTransform*)m_pTargetObject->Get_Component(g_strTransformTag))->Get_State_Vector(CTransform::STATE_POSITION));
	m_pNavigationCom->Set_Index(m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION));
	Update_Command();
	Change_Animation_Weapon(WEAPON_IDLE, true);
	
}

void CChr_Battle_Vanila::Set_Hit(_int iDamage)
{
	if (m_eState == DEAD)
		return;

	__super::Set_Hit(iDamage);

	if (m_iHp <= 0) {
		Change_State(DEAD);
		m_pGameInstance->PlaySoundW(TEXT("Vanila_Dead.wav"), CSound_Manager::CHR3, SOUND_DEFAULT);
	}
	else {
		Change_State(HIT);
		_int irand = rand() % 3;
		wstring strSoundTag = L"Vanila_Damage_" + to_wstring(irand) + L".wav";
		const TCHAR* pChar = strSoundTag.c_str();
		m_pGameInstance->PlaySoundW(const_cast<TCHAR*>(strSoundTag.c_str()), CSound_Manager::CHR3, SOUND_DEFAULT);
	}
}

void CChr_Battle_Vanila::Add_Hp(_int iHp)
{
	__super::Add_Hp(iHp);

	_int iRand = rand() % 4;

	if (!iRand) {
		_int irand = rand() % 2;
		wstring strSoundTag = L"Vanila_Heal_" + to_wstring(irand) + L".wav";
		const TCHAR* pChar = strSoundTag.c_str();
		m_pGameInstance->PlaySoundW(const_cast<TCHAR*>(strSoundTag.c_str()), CSound_Manager::CHR3, SOUND_DEFAULT);
	}
}

void CChr_Battle_Vanila::Update_Target()
{
	if (m_pAbility->Get_CurrentRole() != CAbility::HEALER) {
		__super::Update_Target();
	}
	else {
		Update_HealTarget();
	}
}

void CChr_Battle_Vanila::Create_Sphere(_int iDamage, _int iWeaponNum)
{
	_float4 vPos = ((CBody*)m_PartObjects[iWeaponNum])->Get_BonePosition("L_weapon");

	CSphere::Sphere_Desc Sphere_Desc = {};
	Sphere_Desc.pTargetObject = m_pTargetObject;
	Sphere_Desc.vStartPosition = vPos;
	Sphere_Desc.isTargetMonster = true;
	Sphere_Desc.iDamage = iDamage;
	Sphere_Desc.fChain = 10.f;
	Sphere_Desc.eSkill = m_Commands.front();

	if (FAILED(m_pGameInstance->Add_Clone(g_Level, TEXT("Layer_Bullet"), TEXT("Prototype_GameObject_Sphere"), &Sphere_Desc))) {
		int i = 0;
		return;
	}

	return;
}

void CChr_Battle_Vanila::Create_Sphere_Heal()
{
	_float4 vPos = ((CBody*)m_PartObjects[0])->Get_BonePosition("L_weapon");

	CSphere::Sphere_Desc Sphere_Desc = {};
	Sphere_Desc.pTargetObject = m_pTargetObject;
	Sphere_Desc.vStartPosition = vPos;
	Sphere_Desc.isTargetMonster = false;
	Sphere_Desc.iDamage = m_iAttack_Magic * 3;
	Sphere_Desc.fChain = 0.f;
	Sphere_Desc.eSkill = CRole::CURE;

	if (FAILED(m_pGameInstance->Add_Clone(g_Level, TEXT("Layer_Bullet"), TEXT("Prototype_GameObject_Sphere_Heal"), &Sphere_Desc))) {
		int i = 0;
		return;
	}

	return;
}

HRESULT CChr_Battle_Vanila::Change_State(STATE eState)
{
	m_eState = eState;
	m_pFSMCom->Change_State(eState);

	return S_OK;
}

void CChr_Battle_Vanila::Change_Animation(ANIMATION_CHR_BATTLE_VANILA iAnimationIndex, _bool isLoop)
{
	dynamic_cast<CBody*>(m_PartObjects[0])->Change_Animation(iAnimationIndex, isLoop);
}

void CChr_Battle_Vanila::Change_Animation_Weapon(ANIMATION_CHR_BATTLE_VANILA_WEAPON iAnimationIndex, _bool isLoop)
{
	dynamic_cast<CWeapon_Anim*>(m_PartObjects[1])->Change_Animation(iAnimationIndex, isLoop);
}

void CChr_Battle_Vanila::Change_Role(CAbility::ROLE eRole)
{
	__super::Change_Role(eRole);
	Change_State(OPTIMA);
}

void CChr_Battle_Vanila::Set_State_Battle_Finish()
{
	Change_State(FINISH);
}

void CChr_Battle_Vanila::Set_Target(CGameObject* pTargetObject)
{
	if (m_pAbility->Get_CurrentRole() == CAbility::HEALER)
		return;

	__super::Set_Target(pTargetObject);
}

HRESULT CChr_Battle_Vanila::Add_Components()
{
	if (FAILED(__super::Add_Components()))
		return E_FAIL;

	/* Com_Collider_Body */
	CBounding_OBB::BOUNDING_OBB_DESC		ColliderOBBDesc{};

	/* 로컬상의 정보를 셋팅한다. */
	ColliderOBBDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColliderOBBDesc.vSize = m_vColliderSize;
	ColliderOBBDesc.vCenter = _float3(0.f, ColliderOBBDesc.vSize.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderOBBDesc)))
		return E_FAIL;

	CBounding_Sphere::BOUNDING_SPHERE_DESC ColliderSphereDesc = {};
	ColliderSphereDesc.fRadius = .5f;
	ColliderSphereDesc.vCenter = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Push"), (CComponent**)&m_pCollider_PushCom, &ColliderSphereDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CChr_Battle_Vanila::Add_Component_FSM()
{
	if (FAILED(__super::Add_Component_FSM()))
		return E_FAIL;

	m_pFSMCom->Add_State(IDLE, CChr_Battle_Vanila_State_Idle::Create(this));
	m_pFSMCom->Add_State(HEAL, CChr_Battle_Vanila_State_Heal::Create(this));
	m_pFSMCom->Add_State(SKILL, CChr_Battle_Vanila_State_Skill::Create(this));
	m_pFSMCom->Add_State(HIT, CChr_Battle_Vanila_State_Hit::Create(this));
	m_pFSMCom->Add_State(DEAD, CChr_Battle_Vanila_State_Dead::Create(this));
	m_pFSMCom->Add_State(OPTIMA, CChr_Battle_Vanila_State_Optima::Create(this));
	m_pFSMCom->Add_State(FINISH, CChr_Battle_Vanila_State_Finish::Create(this));

	m_pFSMCom->Change_State(IDLE);

	return S_OK;
}

HRESULT CChr_Battle_Vanila::Add_PartObjects()
{
	/* For.Part_Body */
	CPartObject* pBodyObject = { nullptr };
	CBody::BODY_DESC BodyDesc{};

	BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4_Ptr();
	BodyDesc.strModelTag = TEXT("Prototype_Component_Model_Vanila_Battle");

	pBodyObject = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Part_Body"), &BodyDesc));
	if (nullptr == pBodyObject)
		return E_FAIL;

	m_PartObjects.push_back(pBodyObject);

	/* For.Part_Weapon */
	CPartObject* pWeaponObject = { nullptr };
	CWeapon_Anim::WEAPON_ANIM_DESC	WeaponDesc{};

	CModel* pModel = (CModel*)pBodyObject->Get_Component(TEXT("Com_Model"));

	WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4_Ptr();
	WeaponDesc.pSocket = pModel->Get_BonePtr("R_weapon");
	WeaponDesc.strModelTag = TEXT("Prototype_Component_Model_Vanila_Weapon");
	WeaponDesc.isAddCollider = false;
	pWeaponObject = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Part_Weapon_Anim"), &WeaponDesc));
	if (nullptr == pWeaponObject)
		return E_FAIL;

	m_PartObjects.push_back(pWeaponObject);

	return S_OK;
}

HRESULT CChr_Battle_Vanila::Add_Ability()
{
	m_pAbility = CAbility::Create();

	m_pAbility->Add_Role(CAbility::BLASTER);
	m_pAbility->Add_Skill(CAbility::BLASTER, CRole::WATER, 1);
	m_pAbility->Add_Skill(CAbility::BLASTER, CRole::AERO, 1);
	m_pAbility->Add_Skill(CAbility::BLASTER, CRole::FIRE, 1);

	m_pAbility->Add_Role(CAbility::HEALER);
	m_pAbility->Add_Skill(CAbility::HEALER, CRole::CURE, 1);

	m_pAbility->Set_CurrentRole(CAbility::BLASTER);

	return S_OK;
}

void CChr_Battle_Vanila::Update_HealTarget()
{
	if (m_pAbility->Get_CurrentRole() != CAbility::HEALER) {
		return;
	}
	_int iMinHp = 999999999;
	size_t iIndex = 99999999;
	size_t iNumCnt = m_pGameInstance->Get_LayerCnt(g_Level, g_strChrLayerTag);
	for (size_t i = 0; i < iNumCnt; ++i) {
		CChr_Battle* pChr_Battle = (CChr_Battle*)m_pGameInstance->Get_GameObject(g_Level, g_strChrLayerTag, (_int)i);
		if (pChr_Battle->Get_Hp() != 0 && pChr_Battle->Get_Hp() < iMinHp) {
			iIndex = i;
			iMinHp = pChr_Battle->Get_Hp();
		}
	}

	CChr_Battle::Set_Target(m_pGameInstance->Get_GameObject(g_Level, g_strChrLayerTag, (_uint)iIndex));
}


CChr_Battle_Vanila* CChr_Battle_Vanila::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CChr_Battle_Vanila* pInstance = new CChr_Battle_Vanila(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CChr_Battle_Vanila"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CChr_Battle_Vanila::Clone(void* pArg)
{
	CChr_Battle_Vanila* pInstance = new CChr_Battle_Vanila(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CChr_Battle_Vanila"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChr_Battle_Vanila::Free()
{
	__super::Free();
}
