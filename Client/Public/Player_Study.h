#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CPartObject;
END

BEGIN(Client)

class CPlayer_Study final : public CGameObject
{
public:
	enum STATE {
		STATE_IDLE = 0x01,
		STATE_RUN = 0x02,
		STATE_ATT = 0x04,
	};
private:
	CPlayer_Study(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_Study(const CPlayer_Study& rhs);
	virtual ~CPlayer_Study() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	map<const wstring, CPartObject*>			m_PartObjects;
	_ubyte										m_eState = {};


private:
	HRESULT Add_Components();
	HRESULT Add_PartObjects();

public:
	static CPlayer_Study* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END