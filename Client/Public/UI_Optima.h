#pragma once
#include "UI.h"

BEGIN(Client)
class CPlayer_Battle;
class CUI_Optima final : public CUI
{
private:
	CUI_Optima(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Optima(const CUI_Optima& rhs);
	virtual ~CUI_Optima() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void	Tick(_float fTimeDelta);
	virtual HRESULT Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void	Start();
	virtual void	OnNotify() override;

public:
	void Set_OptimaName(wstring strOptimaName) { m_strOptimaName = strOptimaName; }
	void Reset_Position();

private:
	virtual HRESULT Bind_ShaderResources();
	HRESULT Add_Components();

	HRESULT Render_Font();

	virtual void Move(_float fTimeDelta);		// ���� ��ġ���� ��ǥ ��ġ���� �̵�, ���� ����

private:
	CPlayer_Battle* m_pPlayerInfo = { nullptr };	// �÷��̾��� ������
	CTransform*		m_pTransformTabCom = { nullptr };

	wstring m_strOptimaName = {};

	_float  m_fTimeDelta = { 0.f };

	_float3 m_vStartPosition = { 0.f,0.f,0.f };			// �� Pnal�� �ִϸ��̼� ���� ��ġ 
	_float3 m_vTargetPosition = { 0.f,0.f,0.f };		// �� Pnal�� �̵����� ���� ��ġ

	_float2 m_vFont_OptimaPosition = { 0.f,0.f };		// ���� ��Ƽ�� ���
	_float2 m_vFont_StrPosition = { 0.f,0.f };			// Optima change

public:
	static CUI_Optima* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI_Optima* Clone(void* pArg);
	virtual void Free() override;
};

END

