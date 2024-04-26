#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CCorpse final : public CGameObject
{
public:
	typedef struct Corpse_Desc : public CGameObject::GAMEOBJECT_DESC
	{
		CModel*		pModelCom = { nullptr };
		_float4x4	WorldMatrix;
	} CORPSE_DESC;

private:
	CCorpse(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCorpse(const CCorpse& rhs);
	virtual ~CCorpse() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Start() override;

protected:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

protected:
	_float		m_fTimeDelta = { 0.f };
	CModel*		m_pModelCom = { nullptr };
	CShader*	m_pShaderCom = { nullptr };
	CTexture*	m_pTextureCom = { nullptr };

public:
	static CCorpse* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
