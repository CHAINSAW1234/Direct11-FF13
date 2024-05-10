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
	_bool			m_isTargetMonster = { false };		// m_pTargetObject가 monster인지 체크 
	_int			m_iDamage = { 0 };					// 데미지
	_float			m_fChain = { 0.f };					// 체인 보너스 적용량 : attacker : 0.5, blaster : 10

	_float m_fParticleCreateTimeDelta = { 0.f };

	// 곡선 표현을 위한 변수
	_float3 m_fOBBDirection[3] = {};					// 베지에 곡선 표현을 위한 점의 위치를 비율로 표현
	_float4 m_fStartPosition = {};
	_float m_fDist = { 0.f };							// 거리 계산
	_float m_fTimeDelta = { 0.f };


	// 트레일 표현을 위한 변수
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
