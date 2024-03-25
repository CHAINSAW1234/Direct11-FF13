#include "stdafx.h"
#include "Chr_Battle_Sazh.h"
#include "Chr_Battle_Sazh_State_Idle.h"
#include "Chr_Battle_Sazh_State_Attack.h"
#include "Chr_Battle_Sazh_State_Skill.h"
#include "Chr_Battle_Sazh_State_Hit.h"
#include "Chr_Battle_Sazh_State_Dead.h"
#include "Chr_Battle_Sazh_State_Optima.h"
#include "Chr_Battle_Sazh_State_Finish.h"

#include "FSM.h"
#include "Model.h"
#include "Shader.h"

#include "Body.h"
#include "Weapon_Anim.h"
#include "Ability.h"

#include "ImGUI_Manager.h"


CChr_Battle_Sazh::CChr_Battle_Sazh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CChr_Battle{ pDevice, pContext }
{
}

CChr_Battle_Sazh::CChr_Battle_Sazh(const CChr_Battle_Sazh& rhs)
	: CChr_Battle{ rhs }
{
}

HRESULT CChr_Battle_Sazh::Initialize_Prototype()
{
	m_strChrName = TEXT("»ñÁî");
	return S_OK;
}

HRESULT CChr_Battle_Sazh::Initialize(void* pArg)
{
	GAMEOBJECT_DESC		GameObjectDesc{};

	GameObjectDesc.fSpeedPerSec = 10.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(360.f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;


	m_pImGUI_Manager = CImGUI_Manager::Get_Instance(m_pDevice, m_pContext);

	return S_OK;
}

void CChr_Battle_Sazh::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	Update_Command();
	if (m_pGameInstance->Get_KeyState(KEY_DOWN, DIK_6))
		Change_State(HIT);

}

HRESULT CChr_Battle_Sazh::Late_Tick(_float fTimeDelta)
{
	if (FAILED(__super::Late_Tick(fTimeDelta)))
		return E_FAIL;

	// ÀÓ½Ã
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this);
	return S_OK;
}

HRESULT CChr_Battle_Sazh::Render()
{
	m_pImGUI_Manager->Tick(0);
	Show_ImGUI();
	m_pImGUI_Manager->Render();

	return S_OK;
}

void CChr_Battle_Sazh::Start()
{
	Set_Target(m_pGameInstance->Get_GameObject(g_Level, g_strMonsterLayerTag, 0));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(_float(rand() % 20), 0.f, _float(rand() % 20), 1.f));
	m_pTransformCom->Look_At_ForLandObject(((CTransform*)m_pTargetObject->Get_Component(g_strTransformTag))->Get_State_Vector(CTransform::STATE_POSITION));
	
	Update_Command();
}

HRESULT CChr_Battle_Sazh::Change_State(STATE eState)
{
	m_eState = eState;
	m_pFSMCom->Change_State(eState);

	return S_OK;
}

void CChr_Battle_Sazh::Change_Animation(ANIMATION_CHR_BATTLE_SAZH iAnimationIndex, _bool isLoop)
{
	dynamic_cast<CBody*>(m_PartObjects[0])->Change_Animation(iAnimationIndex, isLoop);
}

void CChr_Battle_Sazh::Change_Role(CAbility::ROLE eRole)
{
	__super::Change_Role(eRole);
	Change_State(OPTIMA);
}

HRESULT CChr_Battle_Sazh::Add_Components()
{
	if (FAILED(__super::Add_Components()))
		return E_FAIL;

	return S_OK;
}
HRESULT CChr_Battle_Sazh::Add_Component_FSM()
{
	if (FAILED(__super::Add_Component_FSM()))
		return E_FAIL;

	m_pFSMCom->Add_State(IDLE, CChr_Battle_Sazh_State_Idle::Create(this));
	m_pFSMCom->Add_State(ATTACK, CChr_Battle_Sazh_State_Attack::Create(this));
	m_pFSMCom->Add_State(SKILL, CChr_Battle_Sazh_State_Skill::Create(this));
	m_pFSMCom->Add_State(HIT, CChr_Battle_Sazh_State_Hit::Create(this));
	m_pFSMCom->Add_State(DEAD, CChr_Battle_Sazh_State_Dead::Create(this));
	m_pFSMCom->Add_State(OPTIMA, CChr_Battle_Sazh_State_Optima::Create(this));
	m_pFSMCom->Add_State(FINISH, CChr_Battle_Sazh_State_Finish::Create(this));

	m_pFSMCom->Change_State(IDLE);

	return S_OK;
}

HRESULT CChr_Battle_Sazh::Add_PartObjects()
{
	/* For.Part_Body */
	CPartObject* pBodyObject = { nullptr };
	CBody::BODY_DESC BodyDesc{};

	BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4_Ptr();
	BodyDesc.strModelTag = TEXT("Prototype_Component_Model_Sazh_Battle");


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
	WeaponDesc.strModelTag = TEXT("Prototype_Component_Model_Sazh_Weapon");

	pWeaponObject = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Part_Weapon_Anim"), &WeaponDesc));
	if (nullptr == pWeaponObject)
		return E_FAIL;

	m_PartObjects.push_back(pWeaponObject);

	WeaponDesc.pSocket = pModel->Get_BonePtr("L_weapon");
	pWeaponObject = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Part_Weapon_Anim"), &WeaponDesc));
	if (nullptr == pWeaponObject)
		return E_FAIL;

	m_PartObjects.push_back(pWeaponObject);


	return S_OK;
}

HRESULT CChr_Battle_Sazh::Add_Ability()
{
	m_pAbility = CAbility::Create();

	m_pAbility->Add_Role(CAbility::BLASTER);
	m_pAbility->Add_Skill(CAbility::BLASTER, CRole::FIRE, 1);
	m_pAbility->Add_Skill(CAbility::BLASTER, CRole::AERO, 1);
	m_pAbility->Add_Skill(CAbility::BLASTER, CRole::FLAMEBLOW, 1);

	m_pAbility->Set_CurrentRole(CAbility::BLASTER);

	return S_OK;
}

void CChr_Battle_Sazh::Show_ImGUI()
{
	_float4x4 worldmatrix = m_pTransformCom->Get_WorldFloat4x4();
	_float vRight[4] = { worldmatrix.m[0][0], worldmatrix.m[0][1] , worldmatrix.m[0][2] , worldmatrix.m[0][3] };
	_float vUp[4] = { worldmatrix.m[1][0], worldmatrix.m[1][1] , worldmatrix.m[1][2] , worldmatrix.m[1][3] };
	_float vLook[4] = { worldmatrix.m[2][0], worldmatrix.m[2][1] , worldmatrix.m[2][2] , worldmatrix.m[2][3] };
	_float vPos[4] = { worldmatrix.m[3][0], worldmatrix.m[3][1] , worldmatrix.m[3][2] , worldmatrix.m[3][3] };

	string str;
	switch (m_eState) {
	case IDLE:
		str = "IDLE";
		break;
	case ATTACK:
		str = "ATTACK";
		break;
	case SKILL:
		str = "SKILL";
		break;
	case HIT:
		str = "HIT";
		break;
	case DEAD:
		str = "DEAD";
		break;
	}

	ImGui::Begin("Chr_Battle Tool");
	if (ImGui::TreeNode("Transform")) {
		ImGui::InputFloat4("Right", vRight);
		ImGui::InputFloat4("Up", vUp);
		ImGui::InputFloat4("Look", vLook);
		ImGui::InputFloat4("Pos", vPos);

		ImGui::TreePop();
	}
	ImGui::Text("Current State : ");
	ImGui::SameLine();
	ImGui::Text(str.c_str());
	ImGui::Text("Animation Index : %d", Get_CurrentAnimationIndex());
	ImGui::Text("Animation Frame : %f", Get_CurrentTrackPosition());
	ImGui::Text("ATB : %f", m_fATB);
	if ( !m_Commands.empty()) {
		ImGui::Text("Queue size : %d", m_Commands.size());
		ImGui::Text("Current SKILL : ");
		//ImGui::Text(string(CRole::Get_SkillName(m_Commands.front()).cbegin(), CRole::Get_SkillName(m_Commands.front()).cend()).c_str());
	}

	ImGui::End();
}

CChr_Battle_Sazh* CChr_Battle_Sazh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CChr_Battle_Sazh* pInstance = new CChr_Battle_Sazh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CChr_Battle_Sazh"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CChr_Battle_Sazh::Clone(void* pArg)
{
	CChr_Battle_Sazh* pInstance = new CChr_Battle_Sazh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CChr_Battle_Sazh"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChr_Battle_Sazh::Free()
{
	__super::Free();

	//CImGUI_Manager::Destroy_Instance();
}
