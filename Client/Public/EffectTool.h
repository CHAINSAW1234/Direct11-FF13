#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CModel;
END

BEGIN(Client)
class CEffect;
class CEffect_2D;
class CEffect_3D;
class CEffect_Instance;
class CEffectTool final : public CGameObject
{
private:
	CEffectTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffectTool(const CEffectTool& rhs);
	virtual ~CEffectTool() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual HRESULT	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private: /* For. ImGui */
	class CImGUI_Manager* m_pImGUI_Manager = { nullptr };

private:
	_bool show_demo_window = { true };
	_bool show_Effect_window = { false };
private:
	CEffect* m_pTargetObject = { nullptr };
	class CCamera_Free* m_pCamera = { nullptr };

	vector<CEffect*>		m_Effects;

	_uint m_iCurrent_Effect_Index = { INFINITE };

private:
	void Set_PickingTarget();	
	void Show_Picking_ImGUI();

	void WindowList_Window();
	void Effect_Window();

private:
	HRESULT Add_Components();
	void	Update_KeyInput();			// Key_Input ¹Þ±â

	CEffect_3D* Create_Effect_3D(_int iMeshIndex, const _char* EffectName);
	CEffect_Instance* Create_Effect_Instance(CVIBuffer_Instance::INSTANCE_DESC InstanceDesc, const _char* EffectName);

public:
	static CEffectTool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
