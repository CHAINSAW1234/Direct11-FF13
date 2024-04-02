#pragma once
#include "Client_Defines.h"
#include "PartObject.h"
#include "Bounding_OBB.h"

BEGIN(Engine)
class CBone;
class CModel;
class CShader;
class CCollider;
END

BEGIN(Client)

class CWeapon_Anim final : public CPartObject
{
public:
	typedef struct Weapon_Anim_Desc : public CPartObject::PARTOBJECT_DESC
	{
		CBone* pSocket = { nullptr };
		wstring strModelTag = {};
		CBounding_OBB::BOUNDING_OBB_DESC Bounding_OBB_Desc = {};
		_bool	isAddCollider = { true };
		
	}WEAPON_ANIM_DESC;

private:
	CWeapon_Anim(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon_Anim(const CWeapon_Anim& rhs);
	virtual ~CWeapon_Anim() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_uint		Get_CurrentAnimationIndex() { return m_pModelCom->Get_CurrentAnimationIndex(); }
	_float		Get_CurrentTrackPosition() { return m_pModelCom->Get_CurrentTrackPosition(); }
	_bool		Is_Animation_Finished() { return m_pModelCom->isFinished(); }
	CCollider*	Get_Collider() { return m_pColliderCom; }

	void		Change_Animation(_uint iAnimationIndex, _bool isLoop) { m_pModelCom->Set_Animation(iAnimationIndex, isLoop); }
	void		Set_TrackPosition(_float fTrackPosition) { m_pModelCom->Set_TrackPosition(fTrackPosition); }

private:
	CModel* m_pModelCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CBone*	m_pSocket = { nullptr };	
	_bool	m_isVisible = { true };

private:
	HRESULT Add_Components(const wstring& strModelTag, CBounding_OBB::BOUNDING_OBB_DESC Bounding_OBB_Desc, _bool isAddCollider = true);
	HRESULT Bind_ShaderResources();

public:
	static CWeapon_Anim* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END