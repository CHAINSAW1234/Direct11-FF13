#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Role.h"

BEGIN(Client)
class CEffect;
class CEffect_Camera_Look final : public CGameObject
{
public:
	typedef struct Effect_Camera_Look_Desc : public CGameObject::GAMEOBJECT_DESC
	{
		_float4 vPosition;
		_float fEffectTime = { 2.f };
		CRole::SKILL eSkill;
	}EFFECT_CAMERA_LOOK_DESC;

private:
	CEffect_Camera_Look(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Camera_Look(const CEffect_Camera_Look& rhs);
	virtual ~CEffect_Camera_Look() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Start() override;

private:
	vector<CEffect*>	m_Effects;
	_float				m_fTimeDelta = { 0.f };
	_float				m_fEffectTime = { 2.f };

public:
	static CEffect_Camera_Look* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
