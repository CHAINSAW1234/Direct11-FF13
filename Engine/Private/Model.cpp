#include "..\Public\Model.h"
#include "Mesh.h"
#include "Bone.h"
#include "Shader.h"
#include "Texture.h"
#include "Animation.h"
#include "GameInstance.h"

CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CModel::CModel(const CModel& rhs)
	: CComponent{ rhs }
	, m_iNumMeshes{ rhs.m_iNumMeshes }
	, m_Meshes{ rhs.m_Meshes }
	, m_iNumMaterials{ rhs.m_iNumMaterials }
	, m_Materials{ rhs.m_Materials }
	, m_TransformMatrix{ rhs.m_TransformMatrix }
	, m_iNumAnimations{ rhs.m_iNumAnimations }
{
	for (auto& pPrototypeAnimation : rhs.m_Animations) {
		m_Animations.push_back(pPrototypeAnimation->Clone());
	}

	for (auto& pPrototypeBone : rhs.m_Bones) {
		m_Bones.push_back(pPrototypeBone->Clone());
	}

	for (auto& pMesh : m_Meshes)
		Safe_AddRef(pMesh);

	for (auto& Material : m_Materials)
	{
		for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
			Safe_AddRef(Material.MaterialTextures[i]);
	}

}

HRESULT CModel::Initialize_Prototype(TYPE eType, const string& strModelFilePath, _fmatrix TransformMatrix)
{
	m_eModelType = eType;
	_uint		iOption = { aiProcessPreset_TargetRealtime_Fast | aiProcess_ConvertToLeftHanded };

	iOption = eType == TYPE_NONANIM ? iOption | aiProcess_PreTransformVertices : iOption;

	/* ������ ������ �о aiScene�ȿ� ��� �����͸� ����ְԵȴ�. */
	m_pAIScene = m_Importer.ReadFile(strModelFilePath.c_str(), iOption);
	if (nullptr == m_pAIScene)
		return E_FAIL;

	// ������ ��� ����
	XMStoreFloat4x4(&m_TransformMatrix, TransformMatrix);

	/* ���� ������ ���������ؼ� ���� ����ϱ� ���� �����Ѵ�.  */

	if (FAILED(Ready_Bones(m_pAIScene->mRootNode)))
		return E_FAIL;

	/* ���� �����ϴ� �޽õ��� �����Ѵ�. */
	/* �� = �޽� + �޽� + ... */
	if (FAILED(Ready_Meshes()))
		return E_FAIL;

	if (FAILED(Ready_Materials(strModelFilePath.c_str())))
		return E_FAIL;

	if (FAILED(Ready_Animations()))
		return E_FAIL;
	return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
	return S_OK;
}


HRESULT CModel::Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, _uint iMeshIndex)
{
	ZeroMemory(m_MeshBoneMatrices, sizeof(_float4x4) * 512);
	
	m_Meshes[iMeshIndex]->Stock_Matrices(m_Bones, m_MeshBoneMatrices);
	return pShader->Bind_Matrices(pConstantName, m_MeshBoneMatrices, 512);
}

// ����!!!!
HRESULT CModel::Bind_ShaderResource(CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eTextureType)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	// �������� �޽��� ����ϴ� ���׸����� ��ȣ�� �޾ƿ�
	_uint		iMeshMaterialIndex = { m_Meshes[iMeshIndex]->Get_MaterialIndex() };

	if (iMeshMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	// ���׸����� �̹��� ���ε��� �ؽ�ó�� ��ȣ�� �ؽ�ó�� ���ε�
	if (nullptr == m_Materials[iMeshMaterialIndex].MaterialTextures[eTextureType])
		return E_FAIL;

	if (FAILED(m_Materials[iMeshMaterialIndex].MaterialTextures[eTextureType]->Bind_ShaderResource(pShader, pConstantName)))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Play_Animation(_float fTimeDelta)
{
	if (INFINITE != m_iNextAnimIndex) {
		Play_Animation_Linear_Interpolation(fTimeDelta);
	}
	else {
		/* ���� �ִϸ��̼ǿ� �´� ���� ����(m_TransformationMatrix)�� �������ش�. */
		// ���� �ִϸ��̼��� ����Ѵ�
		m_Animations[m_iCurrentAnimIndex]->Invalidate_TransformationMatrix(fTimeDelta, m_Bones, m_isLoop);
	}

	// ��� ���� ��ȸ�ϸ鼭 ���� ��ġ ����� ���,
	// ������ �� DFS�� ��ȸ�Ͽ����Ƿ� �θ� ������ ��ȸ�ϹǷ� OK
	for (auto& pBone : m_Bones)
		pBone->Invalidate_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_TransformMatrix));
	return S_OK;
}

HRESULT CModel::Render(_uint iMeshIndex)
{
	m_Meshes[iMeshIndex]->Bind_Buffers();
	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

_bool CModel::Compute_Picking(const CTransform* pTransform, _Out_ _float4* vOutPos)
{
	_float4 vCamPos = m_pGameInstance->Get_CamPosition_Float4();
	_float4 vCurrentOutPos = { 0.f,0.f,0.f,0.f };
	_float vCurrentLength = INFINITE;
	
	for (size_t i = 0; i < m_iNumMeshes; ++i) {
		if (m_Meshes[i]->Compute_Picking(pTransform, &vCurrentOutPos))
			if (vCurrentLength > XMVector3Length((XMLoadFloat4(&vCamPos) - XMLoadFloat4(&vCurrentOutPos))).m128_f32[0]) {
				vCurrentLength = XMVector3Length((XMLoadFloat4(&vCamPos) - XMLoadFloat4(&vCurrentOutPos))).m128_f32[0];
				memcpy(vOutPos, &vCurrentOutPos, sizeof(_float4));
			}
	}
	if (vCurrentLength != INFINITE)
		return true;

	return false;
}

HRESULT CModel::Save_Model(string filepath)
{
	ofstream OFS{filepath, ios::out | ios::binary};

	// 1. Ÿ�� ����
	OFS.write(reinterpret_cast<const char*>(&m_eModelType), sizeof(TYPE));

	// 2. TransformMatrix ����
	OFS.write(reinterpret_cast<const char*>(&m_TransformMatrix), sizeof(_float4x4));

	// 3. ������ ����, NonAnim�� ���� 0���� ó��
	size_t iNum = { 0 };
	if (m_eModelType == TYPE_ANIM) {
		iNum = m_Bones.size();
	}
	OFS.write(reinterpret_cast<const char*>(&iNum), sizeof(size_t));
	// 4. Bone ����
	for (size_t i = 0; i < iNum; ++i) {
		m_Bones[i]->Save_Bone(OFS);
	}

	// 5. Mesh ����
	OFS.write(reinterpret_cast<const char*>(&m_iNumMeshes), sizeof(_uint));
	for (auto& pMesh : m_Meshes) {
		pMesh->Save_Mesh(m_eModelType, OFS);
	}

	// 6. ���׸��� ����	: ��θ� ����
	OFS.write(reinterpret_cast<const char*>(&m_iNumMaterials), sizeof(_uint));
	for (auto& pMaterial : m_Materials) {
		for (size_t i = aiTextureType_DIFFUSE; i < AI_TEXTURE_TYPE_MAX; ++i) {
			//iNum = 0;
			iNum = wcslen(pMaterial.MaterialPath[i]);
			OFS.write(reinterpret_cast<const char*>(&iNum), sizeof(size_t));
			if (iNum != 0) {
				OFS.write(reinterpret_cast<const char*>(&pMaterial.MaterialPath[i]), sizeof(_tchar) * iNum);
			}
		}
	}

	// 7. �ִϸ��̼�
	OFS.write(reinterpret_cast<const char*>(&m_iNumAnimations), sizeof(_uint));
	for (auto& pAnimation : m_Animations) {
		pAnimation->Save_Animation(OFS);
	}

	OFS.close();
	return S_OK;
}

HRESULT CModel::Play_Animation_Linear_Interpolation(_float fTimeDelta)
{
	m_fTime_Inear_Interpolation += fTimeDelta;

	m_Animations[m_iCurrentAnimIndex]->Invalidate_TransformationMatrix_Linear_Interpolation(m_fTime_Inear_Interpolation, m_Bones, m_Animations[m_iNextAnimIndex]);

	if (m_fTime_Inear_Interpolation >= 0.2) {
		m_Animations[m_iCurrentAnimIndex]->Reset_Animation();
		m_iCurrentAnimIndex = m_iNextAnimIndex;
		m_iNextAnimIndex = INFINITE;
		m_fTime_Inear_Interpolation = 0;

	}

	return S_OK;
}

HRESULT CModel::Load_Model(string filepath)
{
	ifstream IFS{ filepath, ios::in | ios::binary };

	// 1. ���� Ÿ�� �б�
	IFS.read(reinterpret_cast<char*>(&m_eModelType), sizeof(TYPE));

	// 2. TransformMatrix �б�
	IFS.read(reinterpret_cast<char*>(&m_TransformMatrix), sizeof(_float4x4));

	// 3. ���� ���� �б�
	size_t iBoneNum = 0;
	IFS.read(reinterpret_cast<char*>(&iBoneNum), sizeof(size_t));
	
	// 4. ���� ��ŭ ���� �д´�
	for (size_t i = 0; i < iBoneNum; ++i) {
		CBone* pBone = CBone::Create(IFS);
		if (nullptr == pBone)
			return E_FAIL;

		m_Bones.push_back(pBone);
	}

	// 5. Mesh �б�
	IFS.read(reinterpret_cast<char*>(&m_iNumMeshes), sizeof(_uint));
	for (size_t i = 0; i < m_iNumMeshes; ++i) {
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, IFS);
		if (nullptr == pMesh)
			return  E_FAIL;

		m_Meshes.push_back(pMesh);
	}

	// 6. ���׸��� �б�
	IFS.read(reinterpret_cast<char*>(&m_iNumMaterials), sizeof(_uint));
	for (size_t i = 0; i < m_iNumMaterials; ++i) {
		MESH_MATERIAL			MeshMaterial{};

		for (size_t j = aiTextureType_DIFFUSE; j < AI_TEXTURE_TYPE_MAX; ++j) {
			size_t iNum = 0;
			IFS.read(reinterpret_cast<char*>(&iNum), sizeof(size_t));

			if (0 != iNum) {
				IFS.read(reinterpret_cast<char*>(&MeshMaterial.MaterialPath[j]), sizeof(_tchar) * iNum);
				MeshMaterial.MaterialTextures[j] = CTexture::Create(m_pDevice, m_pContext, MeshMaterial.MaterialPath[j]);
				if (nullptr == MeshMaterial.MaterialTextures[j])	// ���� ����
					return E_FAIL;
			}

		}
		m_Materials.push_back(MeshMaterial);
	}

	// 7. �ִϸ��̼�
	IFS.read(reinterpret_cast<char*>(&m_iNumAnimations), sizeof(_uint));
	for (size_t i = 0; i < m_iNumAnimations; ++i) {
		CAnimation* pAnimation = CAnimation::Create(IFS);
		if (nullptr == pAnimation) {
			return E_FAIL;
		}
		m_Animations.push_back(pAnimation);
	}

	IFS.close();

	return S_OK;
}

HRESULT CModel::Ready_Meshes()
{
	m_iNumMeshes = m_pAIScene->mNumMeshes;

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		CMesh*		pMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, m_pAIScene->mMeshes[i], m_Bones, XMLoadFloat4x4(&m_TransformMatrix));
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.push_back(pMesh);
	}

	return S_OK;
}

HRESULT CModel::Ready_Materials(const _char* pModelFilePath)
{
	m_iNumMaterials = m_pAIScene->mNumMaterials;

	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		aiMaterial* pAIMaterial = m_pAIScene->mMaterials[i];

		MESH_MATERIAL			MeshMaterial{};
		// material���� �������� �ؽ�ó�� ����ϹǷ� ������ �����ؾ���
		for (size_t j = aiTextureType_DIFFUSE; j < AI_TEXTURE_TYPE_MAX; j++)
		{
			aiString		strTextureFilePath;
				// �ش� Material���� ����ϴ� �ؽ�ó�� �ִٸ� �� ��θ� ������
			if (FAILED(pAIMaterial->GetTexture(aiTextureType(j), 0, &strTextureFilePath)))
				continue;

			// ��� ��ȯ ���� 
			// 1. ����̺�, ���丮 ��� ���
			_char			szDrive[MAX_PATH] = { "" };
			_char			szDirectory[MAX_PATH] = { "" };
			_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);

			// 2. ���� �̸�, Ȯ���� �̸� ���
			_char			szFileName[MAX_PATH] = { "" };
			_char			szEXT[MAX_PATH] = { "" };

			/* ..\Bin\Resources\Models\Fiona\ */
			_splitpath_s(strTextureFilePath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szEXT, MAX_PATH);

			// 3. ������ ��ε��� ����
			_char			szFullPath[MAX_PATH] = { "" };
			strcpy_s(szFullPath, szDrive);
			strcat_s(szFullPath, szDirectory);
			strcat_s(szFullPath, szFileName);
			strcat_s(szFullPath, szEXT);

			// 4. wstring���� ��ȯ
			_tchar			szPerfectPath[MAX_PATH] = { L"" };

			MultiByteToWideChar(CP_ACP, 0, szFullPath, (int)strlen(szFullPath), szPerfectPath, MAX_PATH);
			
			wcscpy_s(MeshMaterial.MaterialPath[j], szPerfectPath);
			// ��ȯ�� ��θ� ����ؼ� �ؽ�ó ����
			MeshMaterial.MaterialTextures[j] = CTexture::Create(m_pDevice, m_pContext, szPerfectPath);
			if (nullptr == MeshMaterial.MaterialTextures[j])	// ���� ����
				return E_FAIL;
		}
		m_Materials.push_back(MeshMaterial);

	}

	return S_OK;
}

HRESULT CModel::Ready_Bones(aiNode* pAINode, _int iParentIndex)
{
	// ��� �Լ�, ���� iParentIndex == -1
	CBone* pBone = CBone::Create(pAINode, iParentIndex);
	if (nullptr == pBone)
		return E_FAIL;

	m_Bones.push_back(pBone);

	size_t iIndex = m_Bones.size() -1;
	for (size_t i = 0; i < pAINode->mNumChildren; ++i) {
		// �ڽ� �� ���� -> ��ͷ� ����
		Ready_Bones(pAINode->mChildren[i], iIndex);
	}

	return S_OK;
}

HRESULT CModel::Ready_Animations()
{
	m_iNumAnimations = m_pAIScene->mNumAnimations;

	for (size_t i = 0; i < m_iNumAnimations; ++i) {
		CAnimation* pAnimation = CAnimation::Create(m_pAIScene->mAnimations[i], m_Bones);
		if (nullptr == pAnimation) {
			return E_FAIL;
		}
		m_Animations.push_back(pAnimation);
	}

	return S_OK;
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const string& strModelFilePath, _fmatrix TransformMatrix)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, strModelFilePath, TransformMatrix)))
	{
		MSG_BOX(TEXT("Failed To Created : CModel"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const string& strModelFilePath)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Load_Model(strModelFilePath)))
	{
		MSG_BOX(TEXT("Failed To Load : CModel"));

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

	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);
	m_Animations.clear();

	for (auto& pBone : m_Bones)
		Safe_Release(pBone);
	m_Bones.clear();

	for (auto& Material : m_Materials)
	{
		for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
			Safe_Release(Material.MaterialTextures[i]);
	}
	m_Materials.clear();

	for (auto& pMesh : m_Meshes)
		Safe_Release(pMesh);
	m_Meshes.clear();

	m_Importer.FreeScene();
}
