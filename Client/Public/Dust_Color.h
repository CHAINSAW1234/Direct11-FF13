#pragma once
#include "Effect_2D.h"

BEGIN(Client)
class CDust_Color final : public CEffect_2D
{
protected:
	CDust_Color(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDust_Color(const CEffect& rhs);
	virtual ~CDust_Color() = default;

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
	static CDust_Color* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
