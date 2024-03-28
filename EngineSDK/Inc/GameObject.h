#pragma once

#include "Transform.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	typedef struct GameObject_Desc : public CTransform::TRANSFORM_DESC
	{		
	}GAMEOBJECT_DESC;

protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	class CComponent*	Get_Component(const wstring& strComTag);
	_bool Get_Dead() { return m_isDead; }
	void Set_Dead(_bool isDead) { m_isDead = isDead; }
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Start();
	virtual void Tick(_float fTimeDelta);
	virtual HRESULT Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

	class CGameInstance*		m_pGameInstance = { nullptr };
	CTransform*					m_pTransformCom = { nullptr };

	_uint						m_eLevel = { INT_MAX };
	_bool						m_isDead = { false };

protected:
	map<const wstring, class CComponent*>		m_Components;

protected:
	HRESULT Add_Component(_uint iLevelIndex, const wstring& strPrototypeTag, const wstring& strComponentTag, class CComponent** ppOut, void* pArg = nullptr);
	HRESULT	Compute_ViewZ();
	void	SetUp_BillBoard();

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END