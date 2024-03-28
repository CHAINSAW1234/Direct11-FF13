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
	_int	m_iMaxHp = { 1 };						// 플레이어의 전체 체력 
	_int	m_iStartHp = { 0 };						// 선형 보간을 위한, 체력 변화시의 UI 체력
	_int	m_iCurHp = { 0 };
	_int	m_iHp = { 0 };							// 플레이어의 체력
	_float	m_fRatio = { 0.f };						// 비율 저장

	_float	m_fHpLerpTimeDelta = { 0.f };				// 체력에 변화가 있을 경우, 선형 보간을 위한 시간 계산 
	_float2 m_vFontPosition = { 0.f,0.f };
	CMonster* m_pTargetMonster = { nullptr };
public:
	static CUI_Monster_Hp* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI_Monster_Hp* Clone(void* pArg);
	virtual void Free() override;
};

END

