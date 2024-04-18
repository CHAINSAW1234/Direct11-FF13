#pragma once
#include "Effect.h"
BEGIN(Client)
class CEffect_2D : public CEffect, public Interface_2D, public Target_Position
{
public:
	typedef struct Effect_2D_Desc : public CEffect::Effect_Desc, public Interface_2D::INTERFACE_2D_DESC, public Target_Position::TARGET_POSITION_DESC
	{} EFFECT_2D_DESC;

protected:
	CEffect_2D(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_2D(const CEffect& rhs);
	virtual ~CEffect_2D() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Start() override;

protected:
	virtual HRESULT Add_Components() override;
	HRESULT Bind_ShaderResources();
	virtual void Frame_Finish() override;

public:
	static CEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
