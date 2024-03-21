#pragma once
#include "UI_Pnal.h"
#include "Role.h"
BEGIN(Client)
// �� Ŭ������ ���¿� ���� �ٸ� �������� ��������
// CUI_Pnal Ŭ������ �̵��� �پ缺�� ���� ���°� �� Ŭ���������� ������ �����Ͽ� �и���
// �� Ŭ������ �ǽð����� ����, ������ ����
// �� Ŭ������ Observer�� ����� ������� ����
class CUI_Pnal_Attack final : public CUI_Pnal
{
public:
	// CUI_Pnal�� struct�� �״�� ����� ����
	typedef struct UI_Pnal_Attack_Desc : public CUI_Pnal::UI_PNAL_DESC
	{
		CRole::SKILL eSkill = { CRole::SKILL_END };
		_uint		 iSize = { 1 };
	} UI_PNAL_ATTACK_DESC;

private:
	CUI_Pnal_Attack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Pnal_Attack(const CUI_Pnal_Attack& rhs);
	virtual ~CUI_Pnal_Attack() = default;

public:
	_uint Get_Size() { return m_iSize; }
	CRole::SKILL Get_Skill() { return m_eSkill; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void	Tick(_float fTimeDelta);
	virtual HRESULT Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void	Start();

private:
	HRESULT Add_Components();
	virtual HRESULT Bind_ShaderResources() override;
	virtual void Move(_float fTimeDelta) override;		// ���� ��ġ���� ��ǥ ��ġ���� �̵�, ���� ����

private:
	CRole::SKILL	m_eSkill = { CRole::SKILL_END };			// �� Pnal�� �����ִ� ��ų
	_uint			m_iSize = { 1 };					// �� Pnal�� ũ�� : ���

public:
	static CUI_Pnal_Attack* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI_Pnal_Attack* Clone(void* pArg);
	virtual void Free() override;
};

END