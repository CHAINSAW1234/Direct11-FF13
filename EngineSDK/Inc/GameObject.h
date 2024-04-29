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
	ID3D11Device* Get_Device() { return m_pDevice; }
	ID3D11DeviceContext* Get_Context() { return m_pContext; }
	class CComponent*	Get_Component(const wstring& strComTag);
	_bool Get_Dead() { return m_isDead; }
	_float Get_ViewZ() { Compute_ViewZ(); return  m_fViewZ; }
	void Set_Dead(_bool isDead) { m_isDead = isDead; }
	void Set_RotationPerSec(_float fRotationPerSec) { m_pTransformCom->Set_RotationPerSec(fRotationPerSec); }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Start();
	virtual void Tick(_float fTimeDelta);
	virtual HRESULT Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_LightDepth() { return S_OK; }
	virtual HRESULT Render_Bright() { return S_OK; }

protected:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

	class CGameInstance*		m_pGameInstance = { nullptr };
	CTransform*					m_pTransformCom = { nullptr };

	_uint						m_eLevel = { INT_MAX };
	_bool						m_isDead = { false };

	_float						m_fViewZ = { 0.0f };

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