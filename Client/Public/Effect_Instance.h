#pragma once
#include "Effect.h"

BEGIN(Client)
class CEffect_Instance final : public CEffect, public Interface_Instance
{
public:
	typedef struct Effect_Instance_Desc : public CEffect::EFFECT_DESC, public Interface_Instance::INTERFACE_INSTANCE_DESC
	{} EFFECT_INSTANCE_DESC;

protected:
	CEffect_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Instance(const CEffect& rhs);
	virtual ~CEffect_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Start() override;
	virtual HRESULT Save_Effect(ofstream& OFS) override;

	HRESULT Initialize_Load(ifstream& IFS);
	virtual HRESULT Load_Effect(ifstream& IFS) override;

	virtual void Reset_Effect() override;

protected:
	virtual HRESULT Add_Components() override;
	HRESULT Bind_ShaderResources();

public:
	static CEffect_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	static CEffect_Instance* Clone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ifstream& IFS);		// 파일에서 읽기를 통한 생성
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END


