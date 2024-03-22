#pragma once
#include "UI_Pnal.h"
#include "Inventory.h"

BEGIN(Client)
class CUI_Pnal_Item final : public CUI_Pnal
{
public:
	// CUI_Pnal�� struct�� �״�� ����� ����
	typedef struct UI_Pnal_Item_Desc : public CUI_Pnal::UI_PNAL_DESC
	{
		CInventory::ITEM eItem = { CInventory::ITEM_END };
	} UI_PNAL_ITEM_DESC;

private:
	CUI_Pnal_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Pnal_Item(const CUI_Pnal_Item& rhs);
	virtual ~CUI_Pnal_Item() = default;

public:
	CInventory::ITEM Get_Item() { return m_eItem; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void	Tick(_float fTimeDelta);
	virtual HRESULT Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void	Start();

private:
	HRESULT Add_Components();
	virtual HRESULT Bind_ShaderResources() override;
	virtual void Move(_float fTimeDelta) override;		// ���� ��ġ���� ��ǥ ��ġ���� �̵�, ���� ����

private:
	CInventory::ITEM m_eItem = { CInventory::ITEM_END };			// �� Pnal�� �����ִ� ������

public:
	static CUI_Pnal_Item* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI_Pnal_Item* Clone(void* pArg);
	virtual void Free() override;
};

END