#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_MapTool final : public CLevel
{
private:
	CLevel_MapTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_MapTool() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Layer_Camera(const wstring& strLayerTag);
	HRESULT Ready_Layer_BackGround(const wstring& strLayerTag);
	HRESULT Ready_Layer_IMGUI(const wstring& strLayerTag);
	//private:
	//	void	LoadData(const wstring& strLayerTag);

private:
	// Parsing �޸�

public:
	static CLevel_MapTool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END