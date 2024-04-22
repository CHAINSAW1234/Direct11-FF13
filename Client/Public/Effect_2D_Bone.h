#pragma once

#include "Effect.h"

BEGIN(Client)
class CEffect_2D_Bone final :  public CEffect, public Interface_2D, public Target_Bone
{
public:
	typedef struct Effect_2D_Bone_Desc : public CEffect::EFFECT_DESC, public Interface_2D::INTERFACE_2D_DESC, public Target_Bone::TARGET_BONE_DESC
	{
	
	} EFFECT_2D_BONE_DESC;

protected:
	CEffect_2D_Bone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_2D_Bone(const CEffect& rhs);
	virtual ~CEffect_2D_Bone() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Start() override;

protected:
	HRESULT Bind_ShaderResources();
	virtual HRESULT Add_Components() override;
	virtual void Frame_Finish() override;

public:
	static CEffect_2D_Bone* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
