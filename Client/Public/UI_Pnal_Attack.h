#pragma once
#include "UI_Pnal.h"
BEGIN(Client)
// 이 클래스는 상태에 따라 다른 움직임을 가져야함
// CUI_Pnal 클래스에 이동의 다양성을 가진 상태가 이 클래스이지만 동작이 복잡하여 분리됨
// 이 클래스는 실시간으로 생성, 삭제될 예정
// 이 클래스는 Observer의 기능을 사용하지 않음
class CUI_Pnal_Attack final : public CUI
{
public:
	// CUI_Pnal의 struct를 그대로 사용할 예정
	typedef struct UI_Pnal_Attack_Desc : public CUI_Pnal::UI_PNAL_DESC
	{
		_uint iSize = { 1 };
	} UI_PNAL_ATTACK_DESC;

private:
	CUI_Pnal_Attack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Pnal_Attack(const CUI_Pnal_Attack& rhs);
	virtual ~CUI_Pnal_Attack() = default;

public:
	_float3 Get_TargetPosition() { return m_vTargetPosition; }
	_uint Get_Size() { return m_iSize; }
	void Set_TargetPosition(_bool isAnimation, _float3 vTargetPosition);
	void Set_Color(_float4 vColor) { m_vColor = vColor; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void	Tick(_float fTimeDelta);
	virtual HRESULT Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void	Start();

private:
	HRESULT Add_Components();
	virtual HRESULT Bind_ShaderResources();

	void Update_Frame(_float fTimeDelta);
	void Move(_float fTimeDelta);		// 현재 위치에서 목표 위치로의 이동, 선형 보간

private:
	class CPlayer_Battle* m_pPlayerInfo = { nullptr };

	CTexture* m_pTextureAnimationCom = { nullptr };
	CTexture* m_pTextureBorderCom = { nullptr };
	CTexture* m_pTextureMaskCom = { nullptr };

	_bool m_isAnimated = { true };
	_float m_fMoveTimeDelta = { 0.f };
	_float m_fMaskMovement = { 0.f };					// 나중에 렌덤으로 변경할 것
	_float4 m_vColor = { 1.f,1.f,1.f,1.f };			// 이 Pnal이 렌더링될 색깔 : ATB게이지가 충분해질 때 까지 색이 부여되지 않음
	wstring m_strName = {};							// 이 Pnal이 가리키는 대상의 이름 ex) 몬스터 이름, 아이템 이름
	FRAME m_tFrame = {};
	_float3 m_vStartPosition = { 0.f,0.f,0.f };			// 이 Pnal의 애니메이션 시작 위치 : 선형 보간을 위해 기억
	_float3 m_vTargetPosition = { 0.f,0.f,0.f };		// 이 Pnal이 이동시의 도착 위치

	_uint m_iSize = { 1 };								// 이 Pnal의 크기 : 비용

public:
	static CUI_Pnal_Attack* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI_Pnal_Attack* Clone(void* pArg);
	virtual void Free() override;
};

END