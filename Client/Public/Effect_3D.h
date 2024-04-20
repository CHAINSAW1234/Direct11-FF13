#pragma once
#include "Effect.h"

BEGIN(Client)
class CEffect_3D final : public CEffect, public Interface_3D, public Target_Position
{
public:
	typedef struct Effect_3D_Desc : public CEffect::EFFECT_DESC, public Interface_3D::INTERFACE_3D_DESC, public Target_Position::TARGET_POSITION_DESC
	{} EFFECT_3D_DESC;

protected:
	CEffect_3D(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_3D(const CEffect& rhs);
	virtual ~CEffect_3D() = default;

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

protected:
	virtual HRESULT Add_Components() override;
	HRESULT Bind_ShaderResources();

public:
	static CEffect_3D* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	static CEffect_3D* Clone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ifstream& IFS);		// 파일에서 읽기를 통한 생성
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END