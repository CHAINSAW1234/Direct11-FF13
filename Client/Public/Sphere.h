#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Role.h"

BEGIN(Client)
class CEffect;

class CSphere : public CGameObject
{
public:
	typedef struct Sphere_Desc : public CGameObject::GAMEOBJECT_DESC
	{
		CGameObject* pTargetObject;
		_float4 vStartPosition;
		CRole::SKILL eSkill; 
		_bool isTargetMonster;
		_int iDamage;
		_float fChain = 10.f;
	}SPHERE_DESC;

protected:
	CSphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSphere(const CSphere& rhs);
	virtual ~CSphere() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Start() override;

protected:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	void Move(_float fTimeDelta);
	virtual void Check_Collision();
	void Update_Trail();

protected:
	CCollider* m_pColliderCom = { nullptr };
	CGameObject* m_pTargetObject = { nullptr };
	vector<CEffect*> m_Effects;

	CRole::SKILL	m_eSkill = { CRole::SKILL_END };
	_bool			m_isTargetMonster = { false };		// m_pTargetObject�� monster���� üũ 
	_int			m_iDamage = { 0 };					// ������
	_float			m_fChain = { 0.f };					// ü�� ���ʽ� ���뷮 : attacker : 0.5, blaster : 10

	_float m_fParticleCreateTimeDelta = { 0.f };

	// � ǥ���� ���� ����
	_float3 m_fOBBDirection[3] = {};					// ������ � ǥ���� ���� ���� ��ġ�� ������ ǥ��
	_float4 m_fStartPosition = {};
	_float m_fDist = { 0.f };							// �Ÿ� ���
	_float m_fTimeDelta = { 0.f };


	// Ʈ���� ǥ���� ���� ����
	CVIBuffer_Trail* m_pVIBufferCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	_float4 m_fTrailPosition[16] = {};

public:
	static CSphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
