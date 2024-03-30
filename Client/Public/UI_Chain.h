#pragma once
#include "UI.h"

BEGIN(Client)
class CPlayer_Battle;
class CMonster;

class CUI_Chain final : public CUI
{
private:
	CUI_Chain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Chain(const CUI_Chain& rhs);
	virtual ~CUI_Chain() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual HRESULT Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Start() override;
	virtual void OnNotify() override;

public:
	void Change_Target(CGameObject* pGameObject);
	void Set_Movement();

private:
	virtual HRESULT Bind_ShaderResources() override;
	HRESULT Add_Components();

	HRESULT Render_Name();
	HRESULT Render_BreakChain();
	HRESULT Render_Chain();

	void	Parse_World_to_ViewPort();
	void	Update_Position(_float fTimeDelta);

	void	Move(_float fTimeDelta);
	void	Update_Ratio();
	void	Check_Target();

private:
	// 이미지가 3장 또는 그 이상 필요할 수도 있음
	CTexture* m_pTextureMaskCom = { nullptr };		// 이미지가 움직이는거 처럼 보이기 위한 마스크 텍스처
	CTexture* m_pTextureBorderCom = { nullptr };	// 테두리를 저장하는 텍스처임
	// UI에서 같고있는 텍스처는 내부로 설정해서 RGB Blue와 더해질 예정

	//CPlayer_Battle* m_pPlayerInfo = { nullptr };
	CGameObject* m_pTarget = { nullptr };

	wstring m_strName = {};

	_bool m_isChasing = { false };						// true이면 직교 투영, false이면 원근 투영 할 것 
	_float m_fMaskMovement = { 0.f };
	_float	m_fMoveTimeDelta = { 0.f };
	_float3 m_vTargetPosition = { 0.f,0.f,0.f };		// 이 Pnal이 이동시의 도착 위치
	_float2 m_vFontPosition = {};
	// 이하 변수들은 몬스터의 Chain과 관련된 데이터이다
	_bool  m_isBreak = { false };
	_float m_fBreakTimeDelta = { 0.f };
	_float m_fChain = { 100.f };
	_float m_fCurChain = { 100.f };
	_float m_fStagger = { 100.f };
	_float m_fRatio = { 0.f };						// Chain / Stagger
	_float m_fCurRatio = { 0.f };					// CurChain / Stagger

public:
	static CUI_Chain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg) override;
	virtual void Free() override;
};

END