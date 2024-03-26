#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Ability.h"
#include "Observer_Handler.h"

BEGIN(Engine)
class CFSM;
class CModel;
class CShader;
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
	
	CAbility* Get_Ability() { return m_pAbility; }
	
	CTransform* Get_Transform() { return m_pTransformCom; }
	_float4		Get_Target_Position() { return ((CTransform*)m_pTargetObject->Get_Component(g_strTransformTag))->Get_State_Float4(CTransform::STATE_POSITION); }
	_float4		Get_Start_Position() { return m_vStartPosition; }
	_float4		Get_Look();						// Player�� Look vector�� Y���� ����� ����
	_int		Get_CommandCount() { return m_iNextCommandCount; }

	/*=============��0�� : Body�� ������� ��=============*/
	_uint		Get_CurrentAnimationIndex();
	_float		Get_CurrentTrackPosition();
	_bool		Is_Animation_Finished();

	/*============================SET============================*/

	void Set_ATB_Full() { m_fATB = m_fMaxATB; }

	void Set_TrackPosition(_float fTrackPosition);
	void Set_Target(CGameObject* pTargetObject);

	/*============================��Ÿ============================*/
	void Add_Hp(_int iHp);
	void Min_Hp(_int iHp);
	void Update_ATB(_float fTimeDelta);
	virtual void Change_Role(CAbility::ROLE eRole);
	virtual void Use_Command();
	virtual CRole::SKILL Get_Current_Command();

protected:
	/*===========================���� ����=====================*/
	virtual HRESULT Add_Components();
	virtual HRESULT Add_Component_FSM();
	virtual HRESULT Add_PartObjects();
	virtual HRESULT Add_Ability();
	void			Update_Command();

protected:
	wstring m_strChrName = {};
	_int	m_iMaxHp = { 1 };
	_int	m_iHp = { 1 };
	_float	m_fATB = { 0.f };	// ATB ������
	_float	m_fMaxATB = { 3.f };
	_int	m_iNextCommandCount = { 0 };					// AI�� : �Ź� �������� ����ϴ� Ŀ�ǵ� ������ �������� �ֱ�
	queue<CRole::SKILL> m_Commands;
	_float4	m_vStartPosition = { 0.f,0.f,0.f,0.f };

	CFSM* m_pFSMCom = { nullptr };
	CAbility* m_pAbility = { nullptr };
	vector<CPartObject*> m_PartObjects; // PartObject�� ���� -> vector�� ���ٰ� �Ǵ�, ���� ��ü ���ɼ��� 0�� ����

	CGameObject* m_pTargetObject = { nullptr };

public:
	virtual void Free() override;

};
END