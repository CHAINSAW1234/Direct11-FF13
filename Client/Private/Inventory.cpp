#include "stdafx.h"
#include "Inventory.h"

const wstring CInventory::strItemName[ITEM_END] = {TEXT("포션"), TEXT("피닉스의 꼬리"), TEXT("개쩌는 아이템")};

HRESULT CInventory::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CInventory::Initialize(void* pArg)
{

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;
   
    Fill_Inventory_At_GameStart();

    return S_OK;
}

HRESULT CInventory::Late_Tick(_float fTimeDelta)
{
    if (FAILED(__super::Late_Tick(fTimeDelta)))
        return E_FAIL;
    return S_OK;
}


HRESULT CInventory::Insert_Item(ITEM eItem)
{
    auto item = std::find_if(m_pItems.begin(), m_pItems.end(),
        [&eItem](pair<ITEM, _uint>& item) {
            return item.first == eItem;
        });

    if (item == m_pItems.end()) {
        m_pItems.push_back({ eItem, 1 });
    }
    else {
        item->second += 1;
    }

    return S_OK;
}

HRESULT CInventory::Delete_Item(ITEM eItem, int iNum)
{
    auto item = std::find_if(m_pItems.begin(), m_pItems.end(),
        [&eItem](pair<ITEM, _uint>& item) {
            return item.first == eItem;
        });

    if (item == m_pItems.end()) {
        return E_FAIL;
    }
    else {
        item->second -= iNum;
        if (item->second <= 0) {
            m_pItems.erase(item);
        }
    }

    return S_OK;
}

CInventory::ITEM CInventory::Get_Item_Index(size_t iNum)
{
    auto item = m_pItems.begin();

    for (int i = 0; i < iNum; ++i)
        ++item;

    return item->first;
}

wstring CInventory::Get_Item_Name(ITEM pItem)
{
    if (pItem >= ITEM_END)
        return L"";

    return strItemName[pItem];
}

_uint CInventory::Get_Item_Count(ITEM eItem)
{
    auto item = std::find_if(m_pItems.begin(), m_pItems.end(),
        [&eItem](pair<ITEM, _uint>& item) {
            return item.first == eItem;
        });

    if (item == m_pItems.end())
        return 0;
    else
        return item->second;
}

void CInventory::Fill_Inventory_At_GameStart()
{
    for (int i = 0; i < 8; ++i) {
        Insert_Item(POTION);
    }

    for (int i = 0; i < 2; ++i) {
        Insert_Item(PHOENIX_DOWN);
    }
    Insert_Item(SPECIAL_ITEM);
}

CInventory* CInventory::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CInventory* pInstance = new CInventory(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CInventory"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CInventory::Clone(void* pArg)
{
    CInventory* pInstance = new CInventory(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Created : CInventory"));

        Safe_Release(pInstance);
    }

    return pInstance;

}

void CInventory::Free()
{
    __super::Free();
    m_pItems.erase(m_pItems.begin(), m_pItems.end());
    m_pItems.clear();

}
