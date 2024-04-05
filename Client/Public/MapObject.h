#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
END

BEGIN(Client)

// 모든 모델을 관리?
class CMapObject final : public CGameObject
{
public:
	typedef struct MapObject_Desc : public GAMEOBJECT_DESC
	{
		wstring strModelTag;
	} MAPOBJECT_DESC;
	enum TYPE { WALL, ROAD, OBJECT, TYPE_END };

private:
	CMapObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMapObject(const CMapObject& rhs);
	virtual ~CMapObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	wstring Get_ModelTag() { return m_strModelTag; }

public:
	_bool Compute_Picking(_Out_ _float4* vOutPos = nullptr);
	void Tick_Navigation();

private:
	wstring			m_strModelTag = {};			// 이 객체가 사용하는 모델의 이름
	CModel*			m_pModelCom = { nullptr };
	CShader*		m_pShaderCom = { nullptr };
	CNavigation*	m_pNavigationCom = { nullptr };
private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CMapObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END