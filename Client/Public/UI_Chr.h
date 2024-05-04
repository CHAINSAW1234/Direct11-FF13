#pragma once
#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CChr_Battle;
class CPlayer_Battle;

class CUI_Chr final : public CUI
{
public:
	// �÷��̾� ü���� ���� �����Ѵ�
	typedef struct UI_Chr_Desc : public CUI::UI_DESC
	{
		CPlayer_Battle* pPlayer_Battle = { nullptr };
		CChr_Battle* pChr_Battle = { nullptr };
		_float3 vStartPosition;
		_float3 vTargetPosition;
	} UI_CHR_DESC;

private:
	CUI_Chr(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Chr(const CUI& rhs);
	virtual ~CUI_Chr() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void	Tick(_float fTimeDelta);
	virtual HRESULT Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void	Start();
	virtual void	OnNotify() override;

private:
	virtual HRESULT Bind_ShaderResources();
	HRESULT Add_Components();

	HRESULT Render_Name();
	HRESULT Render_Role();
	HRESULT Render_Hp();

	void Update_Hp(_float fTimeDelta);
	virtual void Move(_float fTimeDelta);		// ���� ��ġ���� ��ǥ ��ġ���� �̵�, ���� ����
	void Reset_Position();

private:
	CTexture* m_pTextureGradCom = { nullptr };		// �����

	CPlayer_Battle* m_pPlayer_Battle = { nullptr };
	CChr_Battle* m_pChr_Battle = { nullptr };

	_bool	m_isMoved = { true };					// �� �ִϸ��̼��� �̵��� �ϴ����� ���� ó��? -> ����ÿ� ó���� ����

	_float4 m_vColor = { 0.f,1.f,0.f,1.f };			// ���� ���� -> ��� -> �ʷ�
	_int	m_iMaxHp = { 1 };						// �÷��̾��� ��ü ü�� 
	_int	m_iHp = { 0 };							// �÷��̾��� ü��
	_int	m_iStartHp = { 0 };						// ���� ������ ����, ü�� ��ȭ���� UI ü��
	_int	m_iCurHp = { 0 };						// ���� ǥ�õǰ� �ִ� ü��
	_float	m_fRatio = { 0.f };						// ���� ����

	_float  m_fMoveTimeDelta = { 0.f };
	_float	m_fHpLerpTimeDelta = { 0.f };				// ü�¿� ��ȭ�� ���� ���, ���� ������ ���� �ð� ��� 
	_float	m_fGradMovement = { 0.f };					// ü���� ������϶� ó��
	
	_float3 m_vStartPosition = { 0.f,0.f,0.f };			// �� Pnal�� �ִϸ��̼� ���� ��ġ 
	_float3 m_vTargetPosition = { 0.f,0.f,0.f };		// �� Pnal�� �̵����� ���� ��ġ
	
	_float2 m_vFont_NamePosition = { 0.f,0.f };
	_float2 m_vFont_RolePosition = { 0.f,0.f };
	_float2 m_vFont_HpPosition = { 0.f,0.f };

public:
	static CUI_Chr* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI_Chr* Clone(void* pArg);
	virtual void Free() override;
};

END

