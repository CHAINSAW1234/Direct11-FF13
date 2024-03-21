#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CInventory final : public CGameObject
{
public:
	enum ITEM { POTION, PHOENIX_DOWN, SPECIAL_ITEM, ITEM_END };

private:
	const wstring strItemName[ITEM_END] = { TEXT("포션"), TEXT("피닉스의 꼬리"), TEXT("개쩌는 아이템")};

private:
	using CGameObject::CGameObject;
	virtual ~CInventory() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Tick(_float fTimeDelta) override;

public:
	size_t	Get_Inventory_Count() { return m_pItems.size(); }
	HRESULT Insert_Item(ITEM pItem);
	HRESULT	Delete_Item(ITEM pItem, int iNum = 1);
	ITEM	Get_Item_Index(size_t iNum);
	wstring Get_Item_Name(ITEM pItem);
	_uint	Get_Item_Count(ITEM pItem);

	void	Fill_Inventory_At_GameStart();				// 게임 실행시 처음 한번만 수행하는 함수 : 기본 인벤토리 체우기 임

private:
	list<pair<ITEM, _uint>> m_pItems;

public:
	static CInventory* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END