#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CLoadingBlur final : public CGameObject
{
private:
	CLoadingBlur(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLoadingBlur(const CLoadingBlur& rhs);
	virtual ~CLoadingBlur() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Start() override;

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	CVIBuffer*	m_pVIBufferCom = { nullptr };
	CShader*	m_pShaderCom = { nullptr };
	CTexture*	m_pTextureCom = { nullptr };
	_float		m_fTimeDelta = { 0.f };

	_float		m_fSizeX = { g_iWinSizeX };
	_float		m_fSizeY = { g_iWinSizeY };
	_float4x4	m_ViewMatrix = {};
	_float4x4	m_ProjMatrix = {};

public:
	static CLoadingBlur* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
