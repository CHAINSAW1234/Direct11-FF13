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
	bool show_demo_window = true;
	bool show_ModelList_window = true;
	bool show_Model_window = false;

private:
	CTexture*				m_PrevTextures;		// 미리 저장해 둔 모델의 기본 이미지
	vector<wstring>			m_strModelTags;		// 이미지와 매칭되는 실제 객체의 Tag

	vector<CMapObject*>		m_MapObjects;		// 생성된 객체
	CGameObject*			m_pTargetObject = { nullptr };

private:
	void Set_PickingTarget();
	void Show_Picking_ImGUI();

private:
	HRESULT Add_Components();
	HRESULT Set_RenderState();
	HRESULT Reset_RenderState();

	void WindowList_Window();
	void ModelList_Window();


	HRESULT Create_MapObject(const wstring& m_strModelTag);
public:
	static CMapTool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END