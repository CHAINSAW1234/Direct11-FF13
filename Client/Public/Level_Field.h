#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Field final : public CLevel
{
	const string strMapTag = "../Bin/Resources/Map/Level_Field.dat";
private:
	CLevel_Field(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Field() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Read_Map();	
	HRESULT Create_MapObject(const wstring strModelTag, _float4x4 WorldMatrix);
	HRESULT Ready_Layer_Camera(const wstring& strLayerTag);
	//HRESULT Ready_LandObject();

public:
	static CLevel_Field* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END