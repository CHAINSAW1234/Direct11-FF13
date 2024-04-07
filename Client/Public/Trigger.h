#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CTrigger final : public CGameObject
{
private:
	CTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrigger(const CTrigger& rhs);
	virtual ~CTrigger() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Start() override;
private:
	CCollider*		m_pColliderCom = { nullptr };
	CNavigation*	m_pNavigationCom = { nullptr };

private:
	HRESULT Add_Components();

public:
	static CTrigger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END