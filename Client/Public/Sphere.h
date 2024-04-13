#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CSphere : public CGameObject
{
public:
	typedef struct Sphere_Desc : public CGameObject::GAMEOBJECT_DESC
	{
		CGameObject* pTargetObject;
		_float4 vStartPosition;
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

protected:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
	CGameObject* m_pTargetObject = { nullptr };

	_bool	m_isTargetMonster = { false };		// m_pTargetObject가 monster인지 체크 
	_int	m_iDamage;							// 데미지
	_float	m_fChain;						// 체인 보너스 적용량 : attacker : 0.5, blaster : 10
public:
	static CSphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
