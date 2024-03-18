#pragma once
#include "UI.h"


BEGIN(Client)
class CPlayer_Battle;
class CChr_Battle_Light;
class CChr_Battle;				// 나중에 이거로 변경

class CUI_ATB final : public CUI
{
private:
	CUI_ATB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_ATB(const CUI_ATB& rhs);
	virtual ~CUI_ATB() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Bind_ShaderResources() override;
	virtual void OnNotify()					 	 override;

private:
	HRESULT Add_Components();

private:
	// 이미지가 3장 또는 그 이상 필요할 수도 있음
	CTexture* m_pTextureUVCom = { nullptr };		// 이미지가 움직이는거 처럼 보이기 위한 마스크 텍스처
	CTexture* m_pTextureBorderCom = { nullptr };	// 테두리를 저장하는 텍스처임
													// UI에서 같고있는 텍스처는 내부로 설정해서 RGB Blue와 더해질 예정
	CPlayer_Battle* m_pPlayerInfo = { nullptr };	// 플레이어의 포인터

	_float m_fRatio = { 0.f };						// 비율 저장

public:
	static CUI_ATB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg) override;
	virtual void Free() override;
};

END