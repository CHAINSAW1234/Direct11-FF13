#pragma once
#include "UI.h"

BEGIN(Client)

class CUI_Optima_Change final : public CUI
{
	enum STATE { EASEOUT, EASEIN, STATE_END};
private:
	CUI_Optima_Change(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Optima_Change(const CUI_Optima_Change& rhs);
	virtual ~CUI_Optima_Change() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void	Tick(_float fTimeDelta);
	virtual HRESULT Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void	Start();

public:
	void Set_OptimaName(wstring strOptimaName) { m_strOptimaName = strOptimaName; }
	void Reset_Position();
	_bool isFinish() { return !m_isRender; }
private:
	virtual HRESULT Bind_ShaderResources();

	void Move1(_float fTimeDelta);
	void Move2(_float fTimeDelta);
	HRESULT Add_Components();
	void	Update_FontPosition();


	HRESULT Render_Font();

private:
	CTransform* m_pTransformNameCom = { nullptr };
	wstring m_strOptimaName = {};
	_float m_fTimeDelta = { 0.f };

	STATE m_eState = { STATE_END };

	_float3 m_vStartPosition = { 0.f,0.f,0.f };			// 이 Pnal의 애니메이션 시작 위치 
	_float3 m_vMiddlePosition = { 0.f,0.f,0.f };		// 이 Pnal이 이동시의 중간 위치
	_float3 m_vTargetPosition = { 0.f,0.f,0.f };		// 이 Pnal이 이동시의 도착 위치

	_float2 m_vFontUpperPosition = { 0.f,0.f };
	_float2 m_vFontDownPosition = { 0.f,0.f };

public:
	static CUI_Optima_Change* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI_Optima_Change* Clone(void* pArg);
	virtual void Free() override;
};

END

