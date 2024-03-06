#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Model.h"

BEGIN(Engine)
class CShader;
END

BEGIN(Client)

class CTestModel final : public CGameObject
{
public:
	typedef struct TestModel_Desc : public CGameObject::GAMEOBJECT_DESC
	{
		CModel* pModel = { nullptr };
	}TESTMODEL_DESC;

private:
	CTestModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTestModel(const CTestModel& rhs);
	virtual ~CTestModel() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	//void Set_Model(CModel* pModelCom) {
	//	if (nullptr != m_pModelCom) {
	//		Safe_Release(m_pModelCom);
	//	}

	//	m_pModelCom = pModelCom;
	//}
	void Set_FilePath(wstring strFilePath) {
		m_strFilePath = strFilePath;
	}

private:
	CModel*		m_pModelCom = { nullptr };
	CShader*	m_pShaderCom = { nullptr };
	wstring		m_strFilePath = L"";

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CTestModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END