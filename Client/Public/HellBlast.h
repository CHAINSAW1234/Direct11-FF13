#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CEffect;
class CHellBlast final : public CGameObject
{
public:
	typedef struct HellBlast_Desc : public CGameObject::GAMEOBJECT_DESC
	{
		_float4 vPosition;
	}HELLBLAST_DESC;

private:
	CHellBlast(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHellBlast(const CHellBlast& rhs);
	virtual ~CHellBlast() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Start() override;

public:
	void Set_Move(_bool isMove) { m_isMove = isMove; }

private:
	void Move(_float fTimeDelta);
	void Set_Damage();			// 플레이어 데미지 주기

private:
	vector<CEffect*> m_Effects;

	_bool			m_isMove = { false };

public:
	static CHellBlast* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
