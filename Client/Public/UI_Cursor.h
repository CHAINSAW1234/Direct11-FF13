#pragma once
#include "UI.h"

BEGIN(Client)

class CUI_Cursor final : public CUI
{
private:
	CUI_Cursor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Cursor(const CUI_Cursor& rhs);
	virtual ~CUI_Cursor() = default;

public:
	void Set_Position(_float4 vPosition);

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void	Tick(_float fTimeDelta);
	virtual HRESULT Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void	Start();
	virtual void OnNotify();

private:
	HRESULT Add_Components();
	virtual HRESULT Bind_ShaderResources();

private:
	class CPlayer_Battle* m_pPlayerInfo = { nullptr };
	_int		m_fDegree = { 0 };							// �¿� �̵��� ���� ���� 
	_float4		m_vOriginPosition = { 0.f,0.f,0.f,1.f };	// �̵����� ������� ���� ��ġ
public:
	static CUI_Cursor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI_Cursor* Clone(void* pArg);
	virtual void Free() override;
};

END