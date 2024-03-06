#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CParsingTool final : public CGameObject
{
private:
	CParsingTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParsingTool(const CParsingTool& rhs);
	virtual ~CParsingTool() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual HRESULT	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private: /* For. ImGui */
	class CImGUI_Manager* m_pImGUI_Manager = { nullptr };

private:
	_bool show_ModelList_window = { true };
	_bool show_MapList_window = { false };

	_uint m_iCurrent_Model_Index = { INFINITE };
private:
	vector<class CModel*>	m_Models;		// 저장된 모델 객체
	_float4x4				m_TransformMatrix;	// 생성시 적용하는 TransformMatrix

private:
	void ModelList_Window();

	void Parse_All();
public:
	static CParsingTool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END