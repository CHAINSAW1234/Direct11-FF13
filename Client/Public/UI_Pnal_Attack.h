#pragma once
#include "UI_Pnal.h"
BEGIN(Client)
// �� Ŭ������ ���¿� ���� �ٸ� �������� ��������
// CUI_Pnal Ŭ������ �̵��� �پ缺�� ���� ���°� �� Ŭ���������� ������ �����Ͽ� �и���
// �� Ŭ������ �ǽð����� ����, ������ ����
// �� Ŭ������ Observer�� ����� ������� ����
class CUI_Pnal_Attack final : public CUI
{
public:
	// CUI_Pnal�� struct�� �״�� ����� ����
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
	void Move(_float fTimeDelta);		// ���� ��ġ���� ��ǥ ��ġ���� �̵�, ���� ����

private:
	class CPlayer_Battle* m_pPlayerInfo = { nullptr };

	CTexture* m_pTextureAnimationCom = { nullptr };
	CTexture* m_pTextureBorderCom = { nullptr };
	CTexture* m_pTextureMaskCom = { nullptr };

	_bool m_isAnimated = { true };
	_float m_fMoveTimeDelta = { 0.f };
	_float m_fMaskMovement = { 0.f };					// ���߿� �������� ������ ��
	_float4 m_vColor = { 1.f,1.f,1.f,1.f };			// �� Pnal�� �������� ���� : ATB�������� ������� �� ���� ���� �ο����� ����
	wstring m_strName = {};							// �� Pnal�� ����Ű�� ����� �̸� ex) ���� �̸�, ������ �̸�
	FRAME m_tFrame = {};
	_float3 m_vStartPosition = { 0.f,0.f,0.f };			// �� Pnal�� �ִϸ��̼� ���� ��ġ : ���� ������ ���� ���
	_float3 m_vTargetPosition = { 0.f,0.f,0.f };		// �� Pnal�� �̵����� ���� ��ġ

	_uint m_iSize = { 1 };								// �� Pnal�� ũ�� : ���

public:
	static CUI_Pnal_Attack* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI_Pnal_Attack* Clone(void* pArg);
	virtual void Free() override;
};

END