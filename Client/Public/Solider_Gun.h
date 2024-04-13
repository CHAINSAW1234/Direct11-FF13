#pragma once
#include "Solider.h"

BEGIN(Client)
class CSolider_Gun final : public CSolider
{
private:
	CSolider_Gun(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSolider_Gun(const CSolider_Gun& rhs);
	virtual ~CSolider_Gun() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual void Start() override;

	void Create_Bullet(); 

private:
	virtual HRESULT Add_Components() override;
	virtual HRESULT Add_Component_FSM() override;
	virtual HRESULT Add_PartObjects() override;

public:
	static CSolider_Gun* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END