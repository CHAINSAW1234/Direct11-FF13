#pragma once
#include "UI.h"

BEGIN(Client)
// 이 클래스는 애니메이션을 갖고 등장하는 pnal로 한정됨
// 애니메이션이 없는 몬스터의 break 게이지, ATB게이지 pnal는 이 클래스를 사용하지 않음
// 이 클래스는 Observer의 기능을 사용하지 않음
class CUI_Pnal : public CUI
{
public:
	typedef struct UI_Pnal_Desc : public CUI::UI_DESC
	{
		_float3 vStartPosition;
		_float3 vTargetPosition;
		wstring strName;
	} UI_PNAL_DESC;

protected:
	CUI_Pnal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Pnal(const CUI_Pnal& rhs);
	virtual ~CUI_Pnal() = default;

public:
	_float3 Get_TargetPosition() { return m_vTargetPosition; }
	void Set_TargetPosition(_bool isAnimated, _float3 vTargetPosition);
	void Reset_Position();
	void Set_Color(_float4 vColor) { m_vColor = vColor; }
	void Set_Name(wstring strName) { m_strName = strName; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void	Tick(_float fTimeDelta);
	virtual HRESULT Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void	Start();

protected:
	HRESULT Add_Components();
	virtual HRESULT Bind_ShaderResources();

	void Update_Frame(_float fTimeDelta);
	virtual void Move(_float fTimeDelta);		// 현재 위치에서 목표 위치로의 이동, 선형 보간

protected:
	class CPlayer_Battle* m_pPlayerInfo = { nullptr };

	CTexture* m_pTextureAnimationCom = { nullptr };
	CTexture* m_pTextureBorderCom = { nullptr };
	CTexture* m_pTextureMaskCom = { nullptr };

	_bool m_isAnimated = { true };
	
	_float m_fMoveTimeDelta = { 0.f };
	_float m_fMaskMovement = { 0.f };					// 나중에 렌덤으로 변경할 것
	_float4 m_vColor = { 0.f,1.f,1.f,1.f };			// 이 Pnal이 렌더링될 색깔
	wstring m_strName = {};							// 이 Pnal이 가리키는 대상의 이름 ex) 몬스터 이름, 아이템 이름
	FRAME m_tFrame = {};
	_float3 m_vStartPosition = { 0.f,0.f,0.f };			// 이 Pnal의 애니메이션 시작 위치 
	_float3 m_vTargetPosition = { 0.f,0.f,0.f };		// 이 Pnal이 이동시의 도착 위치

public:
	static CUI_Pnal* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI_Pnal* Clone(void* pArg);
	virtual void Free() override;
};

END