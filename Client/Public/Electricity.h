#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CElectricity final : public CGameObject
{
public:
	typedef struct Electricity_Desc : public CGameObject::GAMEOBJECT_DESC
	{
		_float4 fStartPosition;
		_float4 fTargetPosition;
		_int TurnDist;

	} ELECTRICITY_DESC;
private:
	CElectricity(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CElectricity(const CElectricity& rhs);
	virtual ~CElectricity() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Start() override;

private:
	_uint				m_iNumModel = { 0 };
	vector<CModel*>		m_pModelComs;
	CShader*			m_pShaderCom = { nullptr };
	CTexture*			m_pTextureCom = { nullptr };

	_float				m_fTimeDelta = { 0.f };
	_float				m_fDissolveTimeDelta = { 0.f };

	_float4				m_fDir = { 0.f,0.f,0.f,0.f };
	_float				m_fDist = { 0.f };

	_int				m_iCurPosition = { 0 };
	vector<_float4>		m_fPositions;

	

private:
	void Set_Memeber(_float4 fStartPosition, _float4 fTargetPosition);
	
protected:
	virtual HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CElectricity* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
