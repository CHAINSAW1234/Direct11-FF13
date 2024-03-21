#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Battle final : public CLevel
{
	const string strMapTag = "../Bin/Resources/Map/Level_Battle.dat";
private:
	CLevel_Battle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Battle() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Read_Map();
	HRESULT Ready_UI(const wstring& strLayerTag);
	HRESULT Create_MapObject(const wstring strModelTag, _float4x4 WorldMatrix);
	HRESULT Ready_Layer_Camera(const wstring& strLayerTag);
	HRESULT Ready_Layer_BackGround(const wstring& strLayerTag);
	HRESULT Ready_Layer_Chr(const wstring& strLayerTag);
	HRESULT Ready_Layer_Monster(const wstring& strLayerTag);

private:
	class CPlayer_Battle* m_pPlayer = { nullptr };

public:
	static CLevel_Battle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;

};

END