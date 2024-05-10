#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CCamera_Battle final : public CCamera
{
	const _float fInertiaTime = 0.5;

private:
	CCamera_Battle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Battle(const CCamera_Battle& rhs);
	virtual ~CCamera_Battle() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_YOffset(_float fYOffSet) { m_fYOffset = fYOffSet; }
private:
	_float			m_fMouseSensor = { 0.1f };

	_float4			m_vTargetPosition = { 0.f,0.f,0.f,1.f };
	_float4			m_vCurrentPosition = { 0.f,0.f,0.f,1.f };

	_float			m_fDist = { 5.f };

	_float			m_fYOffset = { 2.f };

	_long			m_MouseMoveX = { 0 };
	_long			m_MouseMoveY = { 0 };
	_float			m_fMouseMoveXAxis = { 0.f };
	_float			m_fMouseMoveYAxis = { 0.f };

private:
	void Update_TargetPosition(_float fTimeDelta);	//  객체 전체 순회를 하면서 위치를 조정
	void Update_Dist(_float fTimeDelta);	//  객체 전체 순회를 하면서 위치를 조정
	void Update_With_Mouse(_float fTimeDelta);
	void Set_CursorPos();

public:
	static CCamera_Battle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END