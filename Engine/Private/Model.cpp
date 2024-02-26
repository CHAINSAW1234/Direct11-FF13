#include "..\Public\Model.h"

CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CModel::CModel(const CModel& rhs)
	: CComponent{ rhs }
{
}

HRESULT CModel::Initialize_Prototype(TYPE eType, const string& strModelFilePath)
{

	m_pAIScene = m_Importer.ReadFile(strModelFilePath.c_str(), aiProcessPreset_TargetRealtime_Fast | aiProcess_ConvertToLeftHanded);
	if (nullptr == m_pAIScene)
		return E_FAIL;

	aiString			strTexturePath;
	m_pAIScene->mMaterials[0]->GetTexture(aiTextureType_DIFFUSE, 0, &strTexturePath);




	return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
	return S_OK;
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const string& strModelFilePath)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, strModelFilePath)))
	{
		MSG_BOX(TEXT("Failed To Created : CModel"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CModel::Clone(void* pArg)
{
	CModel* pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CModel"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel::Free()
{
	__super::Free();

}
