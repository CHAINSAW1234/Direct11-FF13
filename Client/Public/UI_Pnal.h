#pragma once
#include "UI.h"

BEGIN(Client)
// �� Ŭ������ �ִϸ��̼��� ���� �����ϴ� pnal�� ������
// �ִϸ��̼��� ���� ������ break ������, ATB������ pnal�� �� Ŭ������ ������� ����
// �� Ŭ������ Observer�� ����� ������� ����
class CUI_Pnal final : public CUI
{
public:
	typedef struct UI_Pnal_Desc : public CUI::UI_DESC
	{
		_float3 vStartPosition;
		_float3 vTargetPosition;
		wstring strName;
	} UI_PNAL_DESC;

private:
	CUI_Pnal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Pnal(const CUI_Pnal& rhs);
	virtual ~CUI_Pnal() = default;

public:
	void Set_TargetPosition(_float3 vTargetPosition);
	void Reset_Position();
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
	void Move(_float fTimeDelta);		// ���� ��ġ���� ��ǥ ��ġ���� �̵�, ���� ����

private:
	class CPlayer_Battle* m_pPlayerInfo = { nullptr };

	CTexture* m_pTextureAnimationCom = { nullptr };
	CTexture* m_pTextureBorderCom = { nullptr };
	CTexture* m_pTextureMaskCom = { nullptr };

	_bool m_isAnimated = { true };
	
	_float m_fMoveTimeDelta = { 0.f };
	_float m_fMaskMovement = { 0.f };					// ���߿� �������� ������ ��
	_float4 m_vColor = { 0.f,1.f,1.f,1.f };			// �� Pnal�� �������� ����
	wstring m_strName = {};							// �� Pnal�� ����Ű�� ����� �̸� ex) ���� �̸�, ������ �̸�
	FRAME m_tFrame = {};
	_float3 m_vStartPosition = { 0.f,0.f,0.f };			// �� Pnal�� �ִϸ��̼� ���� ��ġ 
	_float3 m_vTargetPosition = { 0.f,0.f,0.f };		// �� Pnal�� �̵����� ���� ��ġ

public:
	static CUI_Pnal* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI_Pnal* Clone(void* pArg);
	virtual void Free() override;
};

END