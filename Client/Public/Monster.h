#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CFSM;
END

BEGIN(Client)
class CMonster abstract : public CGameObject
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
	wstring	Get_Name() { return m_strMonsterName; }
	_int	Get_Hp() { return m_iHp; }
	_int	Get_MaxHp() { return m_iMaxHp; }
	_bool	Get_Break() { return m_isBreak; }
	_float  Get_BreakTime() { return m_fBreakTimeDelta; }
	_float  Get_Chain() { return m_fChain; }
	_float  Get_CurrentChain() { return m_fCurChain; }
	_float  Get_Stagger() { return m_fStagger; }

	/*============================SET============================*/
	void Set_Target(CGameObject* pTargetObject);

	/*============================Model============================*/
	_uint		Get_CurrentAnimationIndex();
	_float		Get_CurrentTrackPosition();
	_bool		Is_Animation_Finished();

	/*============================��Ÿ============================*/
	void Add_Hp(_int iHp);
	void Min_Hp(_int iHp);
	void Add_Chain(_float fChain);

protected:
	void Update_Chain(_float fTimeDelta);

protected:
	CModel*		m_pModelCom = { nullptr };
	CShader*	m_pShaderCom = { nullptr };
	CFSM*		m_pFSMCom = { nullptr };

	wstring		m_strMonsterName = {};
	_int		m_iMaxHp = { 1 };
	_int		m_iHp = { 1 };

	// ü���� ���ӽð��� 20�ʷ� �����սô�
	_bool		m_isBreak = { false };						// �극��ũ ���������� ���� ó��
	_float		m_fBreakTimeDelta = { 0.f };
	_float		m_fMagnification = { 1.f };					// ü�� ���� ����
	_float		m_fChain = { 100.f };
	_float		m_fCurChain = { 100.f };					// �ǽð����� �����ϴ� Chain �� : �� ���� 100�� �Ǹ� m_fChain�� 100���� �ʱ�ȭ
	_float		m_fStagger = { 100.f };						// Chain�� �� ������ �������� Break���·� ��ȯ
	_float4		m_vStartPosition = { 0.f,0.f,0.f,1.f };		// ��

	CGameObject* m_pTargetObject = { nullptr };

protected:
	virtual HRESULT Add_Components();
	virtual HRESULT Add_Component_FSM();
	virtual HRESULT Bind_ShaderResources();

public:
	virtual void Free() override;
};

END