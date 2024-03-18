#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_Battle final : public CUI
{
private:
	CUI_Battle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Battle(const CUI_Battle& rhs);
	virtual ~CUI_Battle() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Bind_ShaderResources() override;
	virtual void OnNotify()					 	 override;

private:
	HRESULT Add_Components();

public:
	static CUI_Battle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg) override;
	virtual void Free() override;
};

END