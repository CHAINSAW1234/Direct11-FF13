#pragma once
#include "UI.h"

BEGIN(Client)
class CUI_Skill final : public CUI
{
public:
	typedef struct UI_Skill_Desc {
		CGameObject* pTargetObject = { nullptr };
		wstring strSkillName;
		_float4	vColor;

	} UI_SKILL_DESC;

private:
	CUI_Skill(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Skill(const CUI_Skill& rhs);
	virtual ~CUI_Skill() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void	Tick(_float fTimeDelta);
	virtual HRESULT Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void	Start();

private:
	virtual HRESULT Bind_ShaderResources();
	HRESULT Add_Components();
	void	Update_Position();

	HRESULT Render_Font();

private:
	CGameObject* m_pTargetObject = { nullptr };
	CTransform* m_pTransformBorderCom = { nullptr };
	wstring m_strSkillName = {};

	_float2 m_vFontPosition = { 0.f,0.f };
	_float4 m_vColor = { 0.f,0.f,1.f,.1f };
public:
	static CUI_Skill* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI_Skill* Clone(void* pArg);
	virtual void Free() override;
};

END

