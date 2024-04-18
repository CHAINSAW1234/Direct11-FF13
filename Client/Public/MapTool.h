#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
END

BEGIN(Client)
class CMapObject;

class CMapTool final : public CGameObject
{
private:
	CMapTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMapTool(const CMapTool& rhs);
	virtual ~CMapTool() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual HRESULT	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private: /* For. ImGui */
	class CImGUI_Manager* m_pImGUI_Manager = { nullptr };

private:
	_bool show_demo_window = { false };
	_bool show_ModelCreate_window = { false };
	_bool show_ModelList_window = { false };
	_bool show_MapList_window = { false };

	_bool m_isEnablePicking = { false };

	_uint m_iCurrent_MapObject_Index = { INFINITE };
	_uint m_iCurrent_Mesh_Index = { INFINITE };

private:
	CTexture*				m_PrevTextures = { nullptr };		// �̸� ������ �� ���� �⺻ �̹���
	vector<wstring>			m_strModelTags;		// �̹����� ��Ī�Ǵ� ���� ��ü�� Tag

	vector<CMapObject*>		m_MapObjects;		// ������ ��ü
	CGameObject*			m_pTargetObject = { nullptr };

	vector<string>			m_MapList;			// ���丮���� ���� �� ����Ʈ

	class CCamera_Free*			m_pCamera = { nullptr };

private:
	void Set_PickingTarget();
	void Show_Picking_ImGUI();

private:
	HRESULT Add_Components();

	void Update_KeyInput();			// Key_Input �ޱ�
	HRESULT Update_MapList();			// MapList Update

	void WindowList_Window(_float fTimeDelta);
	void ModelCreate_Window();
	void ModelList_Window();
	void MapList_Window();

	HRESULT Create_MapObject(const wstring& m_strModelTag);

	HRESULT Save_Map(string filepath, string filepathName);
	HRESULT Delete_Map(string strMapTag);
	HRESULT Load_Map(string strMapTag);
public:
	static CMapTool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END