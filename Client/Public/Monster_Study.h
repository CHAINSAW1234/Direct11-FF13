#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
END

BEGIN(Client)

class CMonster_Study final : public CGameObject
{
public:
	enum COLLIDERTYPE { COLLIDER_HEAD, COLLIDER_BODY, COLLIDER_END };
private:
	CMonster_Study(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster_Study(const CMonster_Study& rhs);
	virtual ~CMonster_Study() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	const wstring Get_Name() { return m_strName; }

private:
	CModel*			m_pModelCom = { nullptr };
	CShader*		m_pShaderCom = { nullptr };
	CCollider*		m_pColliderCom[COLLIDER_END] = { nullptr };
	wstring			m_strName = {};

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CMonster_Study* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END