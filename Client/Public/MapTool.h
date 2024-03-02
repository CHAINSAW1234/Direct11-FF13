#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
END

BEGIN(Client)
class CPrevMapObject;
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
	_bool show_demo_window = false;
	_bool show_ModelCreate_window = false;
	_bool show_ModelList_window = false;

	_bool m_isEnablePicking = false;

private:
	CTexture*				m_PrevTextures;		// �̸� ������ �� ���� �⺻ �̹���
	vector<wstring>			m_strModelTags;		// �̹����� ��Ī�Ǵ� ���� ��ü�� Tag

	vector<CMapObject*>		m_MapObjects;		// ������ ��ü
	CGameObject*			m_pTargetObject = { nullptr };

	class CCamera_Free*			m_pCamera = { nullptr };

private:
	void Set_PickingTarget();
	void Show_Picking_ImGUI();

private:
	HRESULT Add_Components();
	HRESULT Set_RenderState();
	HRESULT Reset_RenderState();


	void Update_Ket_Input();			// Key_Input �ޱ�
	void WindowList_Window(_float fTimeDelta);
	void ModelCreate_Window();
	void ModelList_Window();

	HRESULT Create_MapObject(const wstring& m_strModelTag);
public:
	static CMapTool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END