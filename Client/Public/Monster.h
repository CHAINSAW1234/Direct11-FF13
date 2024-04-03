#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Observer_Handler.h"
BEGIN(Engine)
class CModel;
class CShader;
class CFSM;
class CCollider;
END

BEGIN(Client)
class CChr_Battle;

class CMonster abstract : public CGameObject, public CObserver_Handler
{
protected:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Start() override;

public:
	/*============================GET============================*/
	CTransform* Get_Transform() { return m_pTransformCom; }
	CCollider* Get_Collider() { return m_pColliderCom; }
	//CChr_Battle* Get_TargetObject() { return m_pTargetObject;  }
	_float4 Get_StartPosition() { return m_vStartPosition; }
	_float4 Get_TargetPosition();
	wstring	Get_Name() { return m_strMonsterName; }
	_int	Get_Hp() { return m_iHp; }
	_int	Get_MaxHp() { return m_iMaxHp; }
	_float	Get_AttackTime() { return m_fAttackTimeDelta; }
	_bool	Get_Break() { return m_isBreak; }
	_float  Get_BreakTime() { return m_fBreakTimeDelta; }
	_float  Get_Chain() { return m_fChain; }
	_float  Get_CurrentChain() { return m_fCurChain; }
	_float  Get_Stagger() { return m_fStagger; }
	_bool	Get_isTarget() { return m_isTarget; }

	/*============================SET============================*/
	void Set_Target(CChr_Battle* pTargetObject);
	void Set_isTarget(_bool isTarget); 
	void Reset_AttackTime() { m_fAttackTimeDelta = 0.f; };
	virtual void Set_State_Battle_Start();

	/*============================Model============================*/
	_uint		Get_CurrentAnimationIndex();
	_float		Get_CurrentTrackPosition();
	_bool		Is_Animation_Finished();

	/*============================��Ÿ============================*/
	void Add_Hp(_int iHp);
	void Min_Hp(_int iHp);
	void Update_Attack_Time(_float fTimeDelta);
	void Add_Chain(_float fChain);
	
	void Set_AttackAble(_int iIndex) { m_isAttackable[iIndex] = false; }
	void Reset_Attakable();

	_float Cal_Degree_Start();
	_float Cal_Degree_Target();
	
	virtual void Set_Hit(_int iDamage);
	void   Create_Damage(_int iDamage);
	void   Check_Interact_Chr();		// ������ �浹 üũ -> ��� ��ü -> �÷��̾� ĳ���� �о
	void   Check_Interact_Monster();	// ������ �浹 üũ -> ��� ��ü -> �÷��̾� ĳ���� �о
	void   Check_Interact_Weapon();		// ������ �浹 üũ -> ��� ��ü -> �÷��̾� ĳ���� �ǰ� ���� �ֱ�


protected:
	HRESULT Create_UI_Hp();
	void Update_Chain(_float fTimeDelta);


protected:
	CModel*			m_pModelCom = { nullptr };
	CShader*		m_pShaderCom = { nullptr };
	CFSM*			m_pFSMCom = { nullptr };
	CCollider*		m_pColliderCom = { nullptr };
	CCollider*		m_pCollider_WeaponCom = { nullptr };
	CChr_Battle*	m_pTargetObject = { nullptr };

	wstring		m_strMonsterName = {};
	_int		m_iMaxHp = { 1 };
	_int		m_iHp = { 1 };

	_int		m_iDamage = { 1 };
	_bool		m_isAttackable[3] = { true, true, true };	// ���� ������ �� �� �� ���� ���� �ൿ�� �ѹ��� �ǰ�ó�� �ϱ� ���� �迭

	_float		m_fAttackTimeDelta = { 0.f };				// ���� �ֱ� ���

	_bool		m_isTarget = { false };						// �� ��ü�� ���� Ÿ������ ������ߵǴ� �����̴� -> ü�� ����� ���� ������ ��

	// ü���� ���ӽð��� 20�ʷ� �����սô�
	_bool		m_isBreak = { false };						// �극��ũ ���������� ���� ó��
	_float		m_fBreakTimeDelta = { 0.f };
	_float		m_fMagnification = { 1.f };					// ü�� ���� ����
	_float		m_fChain = { 100.f };
	_float		m_fCurChain = { 100.f };					// �ǽð����� �����ϴ� Chain �� : �� ���� 100�� �Ǹ� m_fChain�� 100���� �ʱ�ȭ
	_float		m_fStagger = { 100.f };						// Chain�� �� ������ �������� Break���·� ��ȯ
	_float4		m_vStartPosition = { 0.f,0.f,0.f,1.f };		// ��



protected:
	virtual HRESULT Add_Components();
	virtual HRESULT Add_Component_FSM();
	virtual HRESULT Bind_ShaderResources();

public:
	virtual void Free() override;
};

END