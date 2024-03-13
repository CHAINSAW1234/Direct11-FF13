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

	/* 파일의 정보를 읽어서 aiScene안에 모든 데이터를 담아주게된다. */
	m_pAIScene = m_Importer.ReadFile(strModelFilePath.c_str(), iOption);
	if (nullptr == m_pAIScene)
		return E_FAIL;

	// 오프셋 행렬 저장
	XMStoreFloat4x4(&m_TransformMatrix, TransformMatrix);

	/* 읽은 정보를 바탕으로해서 내가 사용하기 좋게 정리한다.  */

	if (FAILED(Ready_Bones(m_pAIScene->mRootNode)))
		return E_FAIL;

	/* 모델을 구성하는 메시들을 생성한다. */
	/* 모델 = 메시 + 메시 + ... */
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

// 주의!!!!
HRESULT CModel::Bind_ShaderResource(CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eTextureType)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	// 렌더링할 메쉬가 사용하는 머테리얼의 번호를 받아옴
	_uint		iMeshMaterialIndex = { m_Meshes[iMeshIndex]->Get_MaterialIndex() };

	if (iMeshMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	// 머테리얼중 이번에 바인딩할 텍스처의 번호의 텍스처를 바인딩
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
		/* 현재 애니메이션에 맞는 뼈의 상태(m_TransformationMatrix)를 갱신해준다. */
		// 현재 애니메이션을 재생한다
		m_Animations[m_iCurrentAnimIndex]->Invalidate_TransformationMatrix(fTimeDelta, m_Bones, m_isLoop);
	}

	// 모든 뼈를 순회하면서 최종 위치 행렬을 계산,
	// 생성할 때 DFS로 순회하였으므로 부모 뼈부터 순회하므로 OK
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

	// 1. 타입 저장
	OFS.write(reinterpret_cast<const char*>(&m_eModelType), sizeof(TYPE));

	// 2. TransformMatrix 저장
	OFS.write(reinterpret_cast<const char*>(&m_TransformMatrix), sizeof(_float4x4));

	// 3. 뼈개수 저장, NonAnim일 경우는 0개로 처리
	size_t iNum = { 0 };
	if (m_eModelType == TYPE_ANIM) {
		iNum = m_Bones.size();
	}
	OFS.write(reinterpret_cast<const char*>(&iNum), sizeof(size_t));
	// 4. Bone 저장
	for (size_t i = 0; i < iNum; ++i) {
		m_Bones[i]->Save_Bone(OFS);
	}

	// 5. Mesh 저장
	OFS.write(reinterpret_cast<const char*>(&m_iNumMeshes), sizeof(_uint));
	for (auto& pMesh : m_Meshes) {
		pMesh->Save_Mesh(m_eModelType, OFS);
	}

	// 6. 머테리얼 저장	: 경로를 저장
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

	// 7. 애니메이션
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

	// 1. 모델의 타입 읽기
	IFS.read(reinterpret_cast<char*>(&m_eModelType), sizeof(TYPE));

	// 2. TransformMatrix 읽기
	IFS.read(reinterpret_cast<char*>(&m_TransformMatrix), sizeof(_float4x4));

	// 3. 뼈의 개수 읽기
	size_t iBoneNum = 0;
	IFS.read(reinterpret_cast<char*>(&iBoneNum), sizeof(size_t));
	
	// 4. 개수 만큼 뼈를 읽는다
	for (size_t i = 0; i < iBoneNum; ++i) {
		CBone* pBone = CBone::Create(IFS);
		if (nullptr == pBone)
			return E_FAIL;

		m_Bones.push_back(pBone);
	}

	// 5. Mesh 읽기
	IFS.read(reinterpret_cast<char*>(&m_iNumMeshes), sizeof(_uint));
	for (size_t i = 0; i < m_iNumMeshes; ++i) {
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, IFS);
		if (nullptr == pMesh)
			return  E_FAIL;

		m_Meshes.push_back(pMesh);
	}

	// 6. 머테리얼 읽기
	IFS.read(reinterpret_cast<char*>(&m_iNumMaterials), sizeof(_uint));
	for (size_t i = 0; i < m_iNumMaterials; ++i) {
		MESH_MATERIAL			MeshMaterial{};

		for (size_t j = aiTextureType_DIFFUSE; j < AI_TEXTURE_TYPE_MAX; ++j) {
			size_t iNum = 0;
			IFS.read(reinterpret_cast<char*>(&iNum), sizeof(size_t));

			if (0 != iNum) {
				IFS.read(reinterpret_cast<char*>(&MeshMaterial.MaterialPath[j]), sizeof(_tchar) * iNum);
				MeshMaterial.MaterialTextures[j] = CTexture::Create(m_pDevice, m_pContext, MeshMaterial.MaterialPath[j]);
				if (nullptr == MeshMaterial.MaterialTextures[j])	// 생성 실패
					return E_FAIL;
			}

		}
		m_Materials.push_back(MeshMaterial);
	}

	// 7. 애니메이션
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
		// material마다 여러개의 텍스처를 사용하므로 여러번 수행해야함
		for (size_t j = aiTextureType_DIFFUSE; j < AI_TEXTURE_TYPE_MAX; j++)
		{
			aiString		strTextureFilePath;
				// 해당 Material에서 사용하는 텍스처가 있다면 그 경로를 가져옴
			if (FAILED(pAIMaterial->GetTexture(aiTextureType(j), 0, &strTextureFilePath)))
				continue;

			// 경로 변환 과정 
			// 1. 드라이브, 디렉토리 경로 얻기
			_char			szDrive[MAX_PATH] = { "" };
			_char			szDirectory[MAX_PATH] = { "" };
			_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);

			// 2. 파일 이름, 확장자 이름 얻기
			_char			szFileName[MAX_PATH] = { "" };
			_char			szEXT[MAX_PATH] = { "" };

			/* ..\Bin\Resources\Models\Fiona\ */
			_splitpath_s(strTextureFilePath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szEXT, MAX_PATH);

			// 3. 가져온 경로들을 결합
			_char			szFullPath[MAX_PATH] = { "" };
			strcpy_s(szFullPath, szDrive);
			strcat_s(szFullPath, szDirectory);
			strcat_s(szFullPath, szFileName);
			strcat_s(szFullPath, szEXT);

			// 4. wstring으로 변환
			_tchar			szPerfectPath[MAX_PATH] = { L"" };

			MultiByteToWideChar(CP_ACP, 0, szFullPath, (int)strlen(szFullPath), szPerfectPath, MAX_PATH);
			
			wcscpy_s(MeshMaterial.MaterialPath[j], szPerfectPath);
			// 변환된 경로를 사용해서 텍스처 생성
			MeshMaterial.MaterialTextures[j] = CTexture::Create(m_pDevice, m_pContext, szPerfectPath);
			if (nullptr == MeshMaterial.MaterialTextures[j])	// 생성 실패
				return E_FAIL;
		}
		m_Materials.push_back(MeshMaterial);

	}

	return S_OK;
}

HRESULT CModel::Ready_Bones(aiNode* pAINode, _int iParentIndex)
{
	// 재귀 함수, 시작 iParentIndex == -1
	CBone* pBone = CBone::Create(pAINode, iParentIndex);
	if (nullptr == pBone)
		return E_FAIL;

	m_Bones.push_back(pBone);

	size_t iIndex = m_Bones.size() -1;
	for (size_t i = 0; i < pAINode->mNumChildren; ++i) {
		// 자식 뼈 생성 -> 재귀로 돌림
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
