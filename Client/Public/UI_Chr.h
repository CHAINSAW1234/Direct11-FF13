#pragma once
#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CChr_Battle;

class CUI_Chr final : public CUI
{
public:
	// �÷��̾� ü���� ���� �����Ѵ�
	typedef struct UI_Chr_Desc : public CUI::UI_DESC
	{
		CChr_Battle* pChr_Battle = { nullptr };
		_float3 vStartPosition;
		_float3 vTargetPosition;
	} UI_CHR_DESC;

protected:
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

protected:
	virtual HRESULT Bind_ShaderResources();
	HRESULT Add_Components();

	void Update_Hp(_float fTimeDelta);
	virtual void Move(_float fTimeDelta);		// ���� ��ġ���� ��ǥ ��ġ���� �̵�, ���� ����

private:
	CTexture* m_pTextureGradCom = { nullptr };		// �����

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

public:
	static CUI_Chr* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI_Chr* Clone(void* pArg);
	virtual void Free() override;
};

END

