#pragma once
#include "UI.h"
#include "Ability.h"

BEGIN(Client)
class CPlayer_Battle;
class CUI_Optima_Info final : public CUI
{
public:
	typedef struct UI_Optima_Info_Desc : public CUI::UI_DESC
	{
		_float3 vStartPosition;
		_float3 vTargetPosition;
		_int	iChrNum;
	} UI_OPTIMA_INFO_DESC;
private:
	CUI_Optima_Info(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Optima_Info(const CUI_Optima_Info& rhs);
	virtual ~CUI_Optima_Info() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void	Tick(_float fTimeDelta);
	virtual HRESULT Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void	Start();

public:
	void Set_Optima(CAbility::ROLE eRole);
	void Set_OptimaName(wstring strRoleName) { m_strRoleName = strRoleName; }
	void Reset_Position();

private:
	virtual HRESULT Bind_ShaderResources();
	HRESULT Add_Components();

	HRESULT Render_Font();

	virtual void Move(_float fTimeDelta);		// 현재 위치에서 목표 위치로의 이동, 선형 보간

private:
	CPlayer_Battle* m_pPlayerInfo = { nullptr };	// 플레이어의 포인터
	CTexture*		m_pTextureFaceCom = { nullptr };		// 캐릭터 얼굴 UI
	CTransform*		m_pTransformFaceCom = { nullptr };		// 캐릭터 얼굴 UI 위치

	wstring m_strRoleName = {L"TEST"};
	_float4 m_vRoleColor = { 0.f,0.f,0.f,1.f };
	_float  m_fTimeDelta = { 0.f };
	_int	m_iChrNum = { 0 };	// 이 UI가 가리키는 Chr의 인덱스

	_float3 m_vStartPosition = { 0.f,0.f,0.f };			// 이 Pnal의 애니메이션 시작 위치 
	_float3 m_vTargetPosition = { 0.f,0.f,0.f };		// 이 Pnal이 이동시의 도착 위치

	_float2 m_vFont_RolePosition = { 0.f,0.f };		// 현재 옵티마 출력
	_float2 m_vFont_NumPosition = { 0.f,0.f };			// Optima change

public:
	static CUI_Optima_Info* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI_Optima_Info* Clone(void* pArg);
	virtual void Free() override;
};

END

