#pragma once
#include "UI.h"

BEGIN(Client)
class CPlayer_Battle;
class CChr_Battle_Light;
class CChr_Battle;				// 나중에 이거로 변경

// 이 클래스는 Observer의 기능을 사용하지 않음
class CUI_ATB final : public CUI
{
private:
	CUI_ATB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_ATB(const CUI_ATB& rhs);
	virtual ~CUI_ATB() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual HRESULT Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Start() override;
	virtual void OnNotify() override;


private:
	virtual HRESULT Bind_ShaderResources() override;
	HRESULT Add_Components();
	void	Reset_Position();
	void	Move(_float fTimeDelta);

private:
	// 이미지가 3장 또는 그 이상 필요할 수도 있음
	CTexture* m_pTextureMaskCom = { nullptr };		// 이미지가 움직이는거 처럼 보이기 위한 마스크 텍스처
	CTexture* m_pTextureBorderCom = { nullptr };	// 테두리를 저장하는 텍스처임
													// UI에서 같고있는 텍스처는 내부로 설정해서 RGB Blue와 더해질 예정
	CPlayer_Battle* m_pPlayerInfo = { nullptr };	// 플레이어의 포인터

	_float m_fRatio = { 0.f };						// 비율 저장
	_float m_fCurRatio = { 0.f };					// 자연스러운 렌더링을 위한 시간값을 먹이는 Ratio

	_float m_fMaskMovement = { 0.f };
	_float	m_fMoveTimeDelta = { 0.f };
	_float3 m_vStartPosition = { 0.f,0.f,0.f };			// 이 Pnal의 애니메이션 시작 위치 
	_float3 m_vTargetPosition = { 0.f,0.f,0.f };		// 이 Pnal이 이동시의 도착 위치


public:
	static CUI_ATB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg) override;
	virtual void Free() override;
};

END