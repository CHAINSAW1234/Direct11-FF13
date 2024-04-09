#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CCamera_Field final : public CCamera
{
	const _float fInertiaTime = 0.5;

private:
	CCamera_Field(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Field(const CCamera_Field& rhs);
	virtual ~CCamera_Field() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_Target(CGameObject* pGameObject); 

private:
	CGameObject*	m_pTargetObject = { nullptr };
	_float			m_fMouseSensor = { 0.1f };
	_float			m_fDist = { 2.f };
	_float			m_fYOffset = { 1.5f };

	_long			m_MouseMoveX = { 0 };
	_long			m_MouseMoveY = { 0 };
	_float			m_fMouseMoveXAxis = { 0.f };
	_float			m_fMouseMoveYAxis = { 0.f };

private:
	void Update_With_Mouse(_float fTimeDelta);
	void Set_CursorPos();

public:
	static CCamera_Field* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END