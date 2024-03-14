#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CBone;
class CModel;
class CShader;
END

BEGIN(Client)

class CWeapon_Study final : public CPartObject
{
public:
	typedef struct Weapon_Study_Desc: public CPartObject::PARTOBJECT_DESC
	{
		CBone* pSocket = { nullptr };
	}WEAPON_STUDY_DESC;

private:
	CWeapon_Study(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon_Study(const CWeapon_Study& rhs);
	virtual ~CWeapon_Study() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CBone* m_pSocket = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CWeapon_Study* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END