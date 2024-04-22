#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)
class CLevel_EffectTool final : public CLevel
{
private:
	CLevel_EffectTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_EffectTool() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Read_Map();
	HRESULT Create_MapObject(const wstring strModelTag, _float4x4 WorldMatrix);
	HRESULT Ready_Layer_Camera(const wstring& strLayerTag);
	HRESULT Ready_Layer_BackGround(const wstring& strLayerTag);
	HRESULT Ready_Layer_Monster(const wstring& strLayerTag);

	HRESULT Ready_Layer_IMGUI(const wstring& strLayerTag);
	//private:
	//	void	LoadData(const wstring& strLayerTag);

private:
	// Parsing ¸Þ¸ð¸®

public:
	static CLevel_EffectTool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END