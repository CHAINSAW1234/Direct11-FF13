#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CGrid final : public CGameObject
{
private:
	CGrid(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGrid(const CGrid& rhs);
	virtual ~CGrid() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*			m_pShaderCom;
	CVIBuffer_Rect*		m_pVIBufferCom;

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CGrid* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
