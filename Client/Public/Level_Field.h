#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)
class CTroup;
class CLevel_Field final : public CLevel
{
	const string strMapTag = "../Bin/Resources/Map/Level_Field.dat";
	const string strMonsterTag = "asdfasdf";
private:
	CLevel_Field(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Field() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Initialize_Parsed();
	HRESULT Read_Map();	
	HRESULT Create_MapObject(const wstring strModelTag, _float4x4 WorldMatrix);
	HRESULT Ready_Layer_Camera(const wstring& strLayerTag);
	HRESULT Ready_Layer_Chr(const wstring& strLayerTag);
	
	HRESULT Ready_Layer_Monster(const wstring& strLayerTag);
private:
	list<CTroup*> m_Troups;

public:
	static CLevel_Field* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);	// 기존의 방식
	static CLevel_Field* Create_Parsed(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);	// 데이터 파싱을 통한 생성 
	virtual void Free() override;
};

END