#pragma once
#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)
class CTroup;
class CLevel_Field_Boss final : public CLevel
{
	const string strMapTag = "../Bin/Resources/Map/Level_Boss_Battle.dat";
private:
	CLevel_Field_Boss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Field_Boss() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Read_Map();
	HRESULT Create_MapObject(const wstring strModelTag, _float4x4 WorldMatrix);
	HRESULT Ready_Layer_Camera(const wstring& strLayerTag);
	HRESULT Ready_Layer_Chr(const wstring& strLayerTag);
	HRESULT Ready_Layer_Monster(const wstring& strLayerTag);
	HRESULT Ready_Layer_Sky(const wstring& strLayerTag);

private:
	CTroup* m_pTroup = { nullptr };
	_bool m_isBattleStart = { false };
	_float m_fTimeDelta = 0.f;

public:
	static CLevel_Field_Boss* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END