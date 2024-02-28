#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CCamera_MapTool final : public CCamera
{
public:
	typedef struct Camera_MapTool_Desc : public CCamera::CAMERA_DESC
	{
		_float		fMouseSensor = { 0.0f };
	}CAMERA_MAPTOOL_DESC;

private:
	CCamera_MapTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_MapTool(const CCamera_MapTool& rhs);
	virtual ~CCamera_MapTool() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_float			m_fMouseSensor = { 0.0f };

public:
	static CCamera_MapTool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END