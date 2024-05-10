#pragma once
#include "UI.h"

BEGIN(Client)

class CUI_Number final : public CUI
{
	enum STATE { LERP, IDLE, DISAPPEAR, STATE_END };
public:
	enum TYPE { DAMAGE, CRITICAL, HIT, HEAL, BREAK, TYPE_END };
	typedef struct UI_Number_Desc {
		TYPE eType;
		_int iNumber;
		_float4 vPosition;
	}UI_NUMBER_DESC;

private:
	static const _float4 vTypeColor[TYPE_END];		// Ÿ�Կ� ���� �ٸ� ���� �־��ֱ�

private:
	CUI_Number(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Number(const CUI_Number& rhs);
	virtual ~CUI_Number() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void	Tick(_float fTimeDelta);
	virtual HRESULT Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();
	//virtual void	Start();

private:
	static _float4 Get_Color(TYPE eType);
	void	Update_Position();
	void	Change_State(STATE eState);

	void	Lerp();			// ���� �ö󰡰� ó��
	void	Idle();			// 0.5��? ���
	void	Disappear(_float fTimeDelta);	// �ö󰡸鼭 ���� ó��

private:
	TYPE	m_eType = { TYPE_END };
	STATE	m_eState = { STATE_END };
	_float	m_fTimeDelta = { 0.f };
	_int	m_iNumber = { 0 };
	_int	m_iStartNumber = { 0 };
	_int	m_iCurNumber = { 0 };
	_float2 m_vFontPosition = { 0.f,0.f };

public:
	static CUI_Number* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI_Number* Clone(void* pArg);
	virtual void Free() override;
};

END

