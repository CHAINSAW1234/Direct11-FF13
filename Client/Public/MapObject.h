#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
END

BEGIN(Client)

// ��� ���� ����?
class CMapObject final : public CGameObject
{
public:
	typedef struct MapObject_Desc : public GAMEOBJECT_DESC
	{
		wstring strModelTag;
	} MAPOBJECT_DESC;

private:
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
	_bool Compute_Picking(_Out_ _float4* vOutPos = nullptr);

private:
	wstring			m_strModelTag = {};			// �� ��ü�� ����ϴ� ���� �̸�
	CModel*			m_pModelCom = { nullptr };
	CShader*		m_pShaderCom = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CMapObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END