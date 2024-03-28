#pragma once
#include "UI.h"

BEGIN(Client)
class CMonster;

class CUI_Monster_Hp final : public CUI
{
private:
	CUI_Monster_Hp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Monster_Hp(const CUI_Monster_Hp& rhs);
	virtual ~CUI_Monster_Hp() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void	Tick(_float fTimeDelta);
	virtual HRESULT Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void	Start();
	virtual void	OnNotify();

private:
	virtual HRESULT Bind_ShaderResources();
	HRESULT Add_Components();
	void	Update_Hp(_float fTimeDelta);
	void	Update_Position();

	HRESULT Render_Font();

private:
	_int	m_iMaxHp = { 1 };						// �÷��̾��� ��ü ü�� 
	_int	m_iStartHp = { 0 };						// ���� ������ ����, ü�� ��ȭ���� UI ü��
	_int	m_iCurHp = { 0 };
	_int	m_iHp = { 0 };							// �÷��̾��� ü��
	_float	m_fRatio = { 0.f };						// ���� ����

	_float	m_fHpLerpTimeDelta = { 0.f };				// ü�¿� ��ȭ�� ���� ���, ���� ������ ���� �ð� ��� 
	_float2 m_vFontPosition = { 0.f,0.f };
	CMonster* m_pTargetMonster = { nullptr };
public:
	static CUI_Monster_Hp* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI_Monster_Hp* Clone(void* pArg);
	virtual void Free() override;
};

END

