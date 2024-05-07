#pragma once
#include "UI.h"

BEGIN(Client)

class CUI_Warning final : public CUI
{
private:
	CUI_Warning(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Warning(const CUI_Warning& rhs);
	virtual ~CUI_Warning() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual HRESULT Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void	Start() override;

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	void Check_Hp();

private:
	CVIBuffer*	m_pVIBufferCom = { nullptr };
	CShader*	m_pShaderCom = { nullptr };
	CTexture*	m_pTextureCom = { nullptr };
	_float		m_fTimeDelta = { 0.f };
	_float4		m_fColor = { 1.f,0.f,0.f,1.f };
public:
	static CUI_Warning* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg) override;
	virtual void Free() override;
};

END
