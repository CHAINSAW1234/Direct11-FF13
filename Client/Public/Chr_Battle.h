#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Ability.h"
#include "Observer_Handler.h"
#include "UI_Number.h"

BEGIN(Engine)
class CFSM;
class CModel;
class CShader;
class CCollider;
class CPartObject;
END

BEGIN(Client)

class CChr_Battle abstract : public CGameObject, public CObserver_Handler
{
protected:
	CChr_Battle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CChr_Battle(const CChr_Battle& rhs);
	virtual ~CChr_Battle() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	/*============================GET============================*/
	wstring		Get_Name() { return m_strChrName; }
	_int		Get_Hp() { return m_iHp; }
	_int		Get_MaxHp() { return m_iMaxHp; }
 	_float		Get_ATB() { return m_fATB; }
	_float		Get_MaxATB() { return m_fMaxATB; }
	_int		Get_Attack_Physic() { return m_iAttack_Physic; }
	_int		Get_Attack_Magic() { return m_iAttack_Magic; }
	CAbility*	Get_Ability() { return m_pAbility; }
	
	CTransform* Get_Transform() { return m_pTransformCom; }
	_float4		Get_Target_Position();
	_float4		Get_Start_Position() { return m_vStartPosition; }
	_float4		Get_Look();						// Player의 Look vector를 Y값을 지우고 리턴
	_int		Get_CommandCount() { return m_iNextCommandCount; }
	_float3		Get_ColliderSize() { return m_vColliderSize; }

	/*=============모델0번 : Body를 대상으로 함=============*/
	_uint		Get_CurrentAnimationIndex();
	_float		Get_CurrentTrackPosition();
	_bool		Is_Animation_Finished();

	CCollider* Get_Collider() { return m_pColliderCom; }
	CCollider* Get_Collider_Weapon();
	CCollider* Get_Collider_Push() { return m_pCollider_PushCom; }
	CNavigation* Get_Navigation() { return m_pNavigationCom; }

	/*============================SET============================*/
	void Set_ATB_Full() { m_fATB = m_fMaxATB; }
	void Set_TrackPosition(_float fTrackPosition);
	virtual void Set_Target(CGameObject* pTargetObject);
	void Set_AttackAble(_int iIndex) { m_isAttackable[iIndex] = false; }
	void Reset_Attakable();
	void Set_StartPosition(_float4 vStartPosition);
	virtual void Set_State_Battle_Finish();

	virtual void Set_Hit(_int iDamage);								// 피격 상태로 변경
	void Create_UI_Number(CUI_Number::TYPE eType, _int iNum);
	class CUI_Skill* Create_UI_Skill(CRole::SKILL eSkill);
	virtual void Create_Sphere(_int iDamage, _int iWeaponNum = 0);
	/*============================기타============================*/
	void Add_Hp(_int iHp);
	void Min_Hp(_int iHp);
	void Revive();
	void Update_ATB(_float fTimeDelta);

	void Change_Target();									// 공격시 일정 확률로 변경 -> AI전용
	virtual void Change_Role(CAbility::ROLE eRole);
	virtual void Use_Command();
	virtual CRole::SKILL Get_Current_Command();

	_float Cal_Degree_Start();
	_float Cal_Dist_Start();
	_float Cal_Degree_Target();
	_float Cal_Dist_Target();

	void   Check_Interact_Chr(_float fTimeDelta);		// 몸통의 충돌 체크 -> 다른 플레이어 객체 -> 플레이어 캐릭터 밀어냄
	void   Check_Interact_Monster(_float fTimeDelta);	// 몸통의 충돌 체크 -> 상대 객체 -> 몬스터 밀어냄
	virtual void Update_Target();									// 타겟 사망시 변경
	void   Update_Collider();
	void	Update_Command();
protected:
	/*===========================생성 관련=====================*/
	virtual HRESULT Add_Components();
	virtual HRESULT Add_Component_FSM();
	virtual HRESULT Add_PartObjects();
	virtual HRESULT Add_Ability();


protected:
	CFSM*			m_pFSMCom = { nullptr };
	CCollider*		m_pColliderCom = { nullptr };
	CCollider*		m_pCollider_PushCom = { nullptr };		// 밀기 전용 구 충돌체
	CNavigation*	m_pNavigationCom = { nullptr };
	CAbility*		m_pAbility = { nullptr };
	CGameObject*	m_pTargetObject = { nullptr };
	vector<CPartObject*> m_PartObjects; // PartObject를 보관 -> vector가 낫다고 판단, 무기 교체 가능성이 0에 수렴
	vector<int> m_isAttackable;

	wstring m_strChrName = {};
	_int	m_iMaxHp = { 1 };
	_int	m_iHp = { 1 };
	_int	m_iAttack_Physic = { 1 };
	_int	m_iAttack_Magic = { 1 };
	_float	m_fATB = { 0.f };	// ATB 게이지
	_float	m_fMaxATB = { 3.f };
	_int	m_iNextCommandCount = { 0 };					// AI용 : 매번 연속으로 사용하는 커맨드 개수를 랜덤으로 주기

	_float3		m_vColliderSize = { 0.f, 0.f,0.f };
	queue<CRole::SKILL> m_Commands;
	
	_float4	m_vStartPosition = { 0.f,0.f,0.f,0.f };

public:
	virtual void Free() override;

};
END