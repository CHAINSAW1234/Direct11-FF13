#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Ability.h"
#include "Observer_Handler.h"

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
	_int		Get_Attack() { return m_iDamage; }
	CAbility*	Get_Ability() { return m_pAbility; }
	
	CTransform* Get_Transform() { return m_pTransformCom; }
	_float4		Get_Target_Position();
	_float4		Get_Start_Position() { return m_vStartPosition; }
	_float4		Get_Look();						// Player�� Look vector�� Y���� ����� ����
	_int		Get_CommandCount() { return m_iNextCommandCount; }

	/*=============��0�� : Body�� ������� ��=============*/
	_uint		Get_CurrentAnimationIndex();
	_float		Get_CurrentTrackPosition();
	_bool		Is_Animation_Finished();

	CCollider* Get_Collider() { return m_pColliderCom; }
	CCollider* Get_Collider_Weapon();

	/*============================SET============================*/
	void Set_ATB_Full() { m_fATB = m_fMaxATB; }
	void Set_TrackPosition(_float fTrackPosition);
	void Set_Target(CGameObject* pTargetObject);
	void Set_AttackAble(_int iIndex) { m_isAttackable[iIndex] = false; }
	void Reset_Attakable();

	virtual void Set_Hit(_int iDamage);								// �ǰ� ���·� ����
	void Create_Damage(_int iDamage);
	/*============================��Ÿ============================*/
	void Add_Hp(_int iHp);
	void Min_Hp(_int iHp);
	void Update_ATB(_float fTimeDelta);
	void Update_Target();
	virtual void Change_Role(CAbility::ROLE eRole);
	virtual void Use_Command();
	virtual CRole::SKILL Get_Current_Command();

	void   Check_Interact_Chr();		// ������ �浹 üũ -> �ٸ� �÷��̾� ��ü -> �÷��̾� ĳ���� �о
	void   Check_Interact_Monster();	// ������ �浹 üũ -> ��� ��ü -> ���� �о

protected:
	/*===========================���� ����=====================*/
	virtual HRESULT Add_Components();
	virtual HRESULT Add_Component_FSM();
	virtual HRESULT Add_PartObjects();
	virtual HRESULT Add_Ability();
	void			Update_Command();

protected:
	CFSM* m_pFSMCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
	CAbility* m_pAbility = { nullptr };

	vector<CPartObject*> m_PartObjects; // PartObject�� ���� -> vector�� ���ٰ� �Ǵ�, ���� ��ü ���ɼ��� 0�� ����

	wstring m_strChrName = {};
	_int	m_iMaxHp = { 1 };
	_int	m_iHp = { 1 };
	_int	m_iDamage = { 1 };

	vector<int> m_isAttackable;

	_float	m_fATB = { 0.f };	// ATB ������
	_float	m_fMaxATB = { 3.f };
	_int	m_iNextCommandCount = { 0 };					// AI�� : �Ź� �������� ����ϴ� Ŀ�ǵ� ������ �������� �ֱ�
	queue<CRole::SKILL> m_Commands;
	_float4	m_vStartPosition = { 0.f,0.f,0.f,0.f };

	CGameObject* m_pTargetObject = { nullptr };

public:
	virtual void Free() override;

};
END