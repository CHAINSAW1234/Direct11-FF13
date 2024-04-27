#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CElectricity_Left final: public CGameObject
{
public:
	typedef struct Electricity_left_Desc : public CGameObject::GAMEOBJECT_DESC
	{
		_float4 vPosition;
		_bool isCamLook;					// 카메라 바라봄 또는 바닥에 있음
	} ELECTRICITY_LEFT_DESC;

private:
	CElectricity_Left(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CElectricity_Left(const CElectricity_Left& rhs);
	virtual ~CElectricity_Left() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Start() override;

private:
	CModel*		m_pModelCom = { nullptr };
	CShader*	m_pShaderCom = { nullptr };
	_float		m_fTimeDelta = { 0.f };
	_float		m_fTimeMultiplication = { 1.f };
	_float4		m_fTargetPosition = { 0.f,0.f,0.f,1.f };

protected:
	virtual HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CElectricity_Left* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END