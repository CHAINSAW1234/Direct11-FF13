#pragma once
#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Client)

class CLeopard final : public CMonster
{
public:
	enum STATE { STATE_IDLE, STATE_ATTACK, STATE_HIT, STATE_END };
private:
	CLeopard(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLeopard(const CMonster& rhs);
	virtual ~CLeopard() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Start() override;

public:
	HRESULT Change_State(STATE eState);

private:
	STATE eState = { STATE_END };


private:
	virtual HRESULT Add_Components() override;
	virtual HRESULT Add_Component_FSM() override;

public:
	static CLeopard* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END