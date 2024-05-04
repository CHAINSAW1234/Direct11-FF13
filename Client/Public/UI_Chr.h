#pragma once
#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CChr_Battle;
class CPlayer_Battle;

class CUI_Chr final : public CUI
{
public:
	// 플레이어 체력은 직교 투영한다
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
	virtual void Move(_float fTimeDelta);		// 현재 위치에서 목표 위치로의 이동, 선형 보간
	void Reset_Position();

private:
	CTexture* m_pTextureGradCom = { nullptr };		// 점멸용

	CPlayer_Battle* m_pPlayer_Battle = { nullptr };
	CChr_Battle* m_pChr_Battle = { nullptr };

	_bool	m_isMoved = { true };					// 이 애니메이션이 이동을 하는지에 대한 처리? -> 등장시에 처리될 예정

	_float4 m_vColor = { 0.f,1.f,0.f,1.f };			// 색깔 빨강 -> 노랑 -> 초록
	_int	m_iMaxHp = { 1 };						// 플레이어의 전체 체력 
	_int	m_iHp = { 0 };							// 플레이어의 체력
	_int	m_iStartHp = { 0 };						// 선형 보간을 위한, 체력 변화시의 UI 체력
	_int	m_iCurHp = { 0 };						// 현제 표시되고 있는 체력
	_float	m_fRatio = { 0.f };						// 비율 저장

	_float  m_fMoveTimeDelta = { 0.f };
	_float	m_fHpLerpTimeDelta = { 0.f };				// 체력에 변화가 있을 경우, 선형 보간을 위한 시간 계산 
	_float	m_fGradMovement = { 0.f };					// 체력이 노란색일때 처리
	
	_float3 m_vStartPosition = { 0.f,0.f,0.f };			// 이 Pnal의 애니메이션 시작 위치 
	_float3 m_vTargetPosition = { 0.f,0.f,0.f };		// 이 Pnal이 이동시의 도착 위치
	
	_float2 m_vFont_NamePosition = { 0.f,0.f };
	_float2 m_vFont_RolePosition = { 0.f,0.f };
	_float2 m_vFont_HpPosition = { 0.f,0.f };

public:
	static CUI_Chr* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI_Chr* Clone(void* pArg);
	virtual void Free() override;
};

END

