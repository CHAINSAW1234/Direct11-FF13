#pragma once
#include "UI_Pnal.h"
#include "Role.h"
BEGIN(Client)
// 이 클래스는 상태에 따라 다른 움직임을 가져야함
// CUI_Pnal 클래스에 이동의 다양성을 가진 상태가 이 클래스이지만 동작이 복잡하여 분리됨
// 이 클래스는 실시간으로 생성, 삭제될 예정
// 이 클래스는 Observer의 기능을 사용하지 않음
class CUI_Pnal_Attack final : public CUI_Pnal
{
public:
	// CUI_Pnal의 struct를 그대로 사용할 예정
	typedef struct UI_Pnal_Attack_Desc : public CUI_Pnal::UI_PNAL_DESC
	{
		CRole::SKILL eSkill = { CRole::SKILL_END };
		_uint		 iSize = { 1 };
	} UI_PNAL_ATTACK_DESC;

private:
	CUI_Pnal_Attack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Pnal_Attack(const CUI_Pnal_Attack& rhs);
	virtual ~CUI_Pnal_Attack() = default;

public:
	_uint Get_Size() { return m_iSize; }
	CRole::SKILL Get_Skill() { return m_eSkill; }

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
	virtual void Move(_float fTimeDelta) override;		// 현재 위치에서 목표 위치로의 이동, 선형 보간

private:
	CRole::SKILL	m_eSkill = { CRole::SKILL_END };			// 이 Pnal이 갖고있는 스킬
	_uint			m_iSize = { 1 };					// 이 Pnal의 크기 : 비용

public:
	static CUI_Pnal_Attack* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI_Pnal_Attack* Clone(void* pArg);
	virtual void Free() override;
};

END