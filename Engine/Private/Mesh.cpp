#include "Mesh.h"
#include "Bone.h"
#include "GameInstance.h"

CMesh::CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CMesh::CMesh(const CMesh& rhs)
	: CVIBuffer(rhs)
	, m_pIndices(rhs.m_pIndices)
{
}

#ifdef _ASSIMP
HRESULT CMesh::Initialize_Prototype(CModel::TYPE eModelType, const aiMesh* pAIMesh, const vector<CBone*>& Bones, _fmatrix TransformMatrix)
{
	strcpy_s(m_szName, pAIMesh->mName.data);
	m_iMaterialIndex = pAIMesh->mMaterialIndex;
	m_iNumVertices = pAIMesh->mNumVertices;
	m_pVerticesPos = new _float3[m_iNumVertices];
	m_iNumIndices = pAIMesh->mNumFaces * 3;
	m_iIndexStride = sizeof(_uint);
	m_iNumVertexBuffers = 1;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER

	// 모델 타입에 따라 다른 방식으로 버텍스를 정의할 것
	HRESULT hr = CModel::TYPE_NONANIM == eModelType ? Ready_Vertices_For_NonAnimModel(pAIMesh, TransformMatrix) : Ready_Vertices_For_AnimModel(pAIMesh, Bones);
	if (FAILED(hr))
		return E_FAIL;

#pragma endregion


#pragma region INDEX_BUFFER
	// 인덱스 버퍼의 정의는 모델의 타입과 관계 없다

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	/* 인덱스 버퍼의 byte크기 */
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;


	m_pIndices = new _uint[m_iNumIndices];
	ZeroMemory(m_pIndices, sizeof(_uint) * m_iNumIndices);

	_uint		iNumIndices = { 0 };

	for (size_t i = 0; i < pAIMesh->mNumFaces; i++)
	{
		m_pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[0];
		m_pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[1];
		m_pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[2];
	}

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = m_pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

#pragma endregion

	return S_OK;
}
#endif

HRESULT CMesh::Initialize(void* pArg)
{
	return S_OK;
}

_bool CMesh::Compute_Picking(const CTransform* pTransform, _Out_ _float4* vOutPos)
{
	// Animation에 적용하면 안될듯 
	_float4		fRayDir, fRayPos;
	_vector		vRayDir, vRayPos;
	m_pGameInstance->Transform_PickingToLocalSpace(pTransform, &fRayDir, &fRayPos);
	vRayDir = XMLoadFloat4(&fRayDir);
	vRayPos = XMLoadFloat4(&fRayPos);

	_float		fMinDist = INFINITY;
	_vector		vOut = { 0.f,0.f,0.f,0.f };
	_uint		iIndicesCount = 0;
	for (size_t i = 0; i < m_iNumIndices/3; ++i) {
		_float fDist = 0.f;

		if (DirectX::TriangleTests::Intersects(vRayPos, vRayDir,
			XMVectorSetW(XMLoadFloat3(&m_pVerticesPos[m_pIndices[iIndicesCount++]]), 1.f),
			XMVectorSetW(XMLoadFloat3(&m_pVerticesPos[m_pIndices[iIndicesCount++]]), 1.f),
			XMVectorSetW(XMLoadFloat3(&m_pVerticesPos[m_pIndices[iIndicesCount++]]), 1.f),
			fDist)) {
			fMinDist = min(fMinDist, fDist);

			vOut = vRayPos + vRayDir * fMinDist;
		}
	}

	if (vOutPos != nullptr) {
		XMStoreFloat4(vOutPos, vOut);
	}

	return !XMVector4Equal(vOut, XMVectorZero());
}

HRESULT CMesh::Stock_Matrices(const vector<CBone*>& Bones, _Out_ _float4x4* pMeshBoneMatrices)
{
	if (nullptr == pMeshBoneMatrices)
		return E_FAIL;

	for (size_t i = 0; i < m_iNumBones; ++i) 
		XMStoreFloat4x4(&pMeshBoneMatrices[i], XMLoadFloat4x4(&m_OffsetMatrices[i]) * XMLoadFloat4x4(Bones[m_Bones[i]]->Get_CombinedTransformationMatrix()));

	return S_OK;
}


#ifdef _ASSIMP
HRESULT CMesh::Save_Mesh(CModel::TYPE eModelType, ofstream& OFS)
{
	//1. m_szName의 길이 저장
	size_t szNameLength = strlen(m_szName);
	OFS.write(reinterpret_cast<const char*>(&szNameLength), sizeof(size_t));
	// 2. m_szName 저장
	OFS.write(reinterpret_cast<const char*>(&m_szName), sizeof(_char) * szNameLength);
	// 3. m_iMaterialIndex 저장
	OFS.write(reinterpret_cast<const char*>(&m_iMaterialIndex), sizeof(_uint));
	// 4. m_iNumVertices
	OFS.write(reinterpret_cast<const char*>(&m_iNumVertices), sizeof(_uint));
	// 5.m_iNumIndices
	OFS.write(reinterpret_cast<const char*>(&m_iNumIndices), sizeof(_uint));
	// 6. m_iNumBones;
	OFS.write(reinterpret_cast<const char*>(&m_iNumBones), sizeof(_uint));
	
#pragma region VERTEX_BUFFER

	// 7. 버텍스 정보 저장
	HRESULT hr = CModel::TYPE_NONANIM == eModelType ? Save_Mesh_For_NonAnimModel(OFS) : Save_Mesh_For_AnimModel(OFS);
	if (FAILED(hr))
		return E_FAIL;

#pragma endregion

#pragma region INDEX_BUFFER

	// 8. 인덱스 정보 저장

	// 데이터를 읽기 위해 버퍼 생성
	D3D11_BUFFER_DESC BufferDesc{};
	BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	BufferDesc.Usage = D3D11_USAGE_STAGING;
	BufferDesc.BindFlags = 0;
	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	BufferDesc.MiscFlags = 0;
	BufferDesc.StructureByteStride = 0;

	ID3D11Buffer* pTempBuffer = { nullptr };

	if (FAILED(m_pDevice->CreateBuffer(&BufferDesc, nullptr, &pTempBuffer)))
		return E_FAIL;

	m_pContext->CopyResource(pTempBuffer, m_pIB);

	D3D11_MAPPED_SUBRESOURCE mappedResource = {};

	if (FAILED(m_pContext->Map(pTempBuffer, 0, D3D11_MAP_READ, 0, &mappedResource)))
		return E_FAIL;

	_uint* pIndices = (_uint*)(mappedResource.pData);
	for (size_t i = 0; i < m_iNumIndices; i++)
	{
		OFS.write(reinterpret_cast<const char*>(&pIndices[i]), sizeof(_uint));
	}
	m_pContext->Unmap(pTempBuffer, 0);

	Safe_Release(pTempBuffer);

#pragma endregion

	return S_OK;
}
#endif

HRESULT CMesh::Load_Mesh(CModel::TYPE eModelType, ifstream& IFS)
{
	//1. m_szName의 길이
	size_t szNameLength = { 0 };
	IFS.read(reinterpret_cast<char*>(&szNameLength), sizeof(size_t));
	// 2. m_szName
	IFS.read(reinterpret_cast<char*>(&m_szName), sizeof(_char) * szNameLength);
	// 3. m_iMaterialIndex
	IFS.read(reinterpret_cast<char*>(&m_iMaterialIndex), sizeof(_uint));
	// 4. m_iNumVertices
	IFS.read(reinterpret_cast<char*>(&m_iNumVertices), sizeof(_uint));
	// 5. m_iNumIndices
	IFS.read(reinterpret_cast<char*>(&m_iNumIndices), sizeof(_uint));
	// 6. m_iBones
	IFS.read(reinterpret_cast<char*>(&m_iNumBones), sizeof(_uint));
	// 6. 기타 고정된 정보 저장
	m_pVerticesPos = new _float3[m_iNumVertices];
	m_iIndexStride = sizeof(_uint);
	m_iNumVertexBuffers = 1;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER

	// 7. 버텍스 정보 읽기
	HRESULT hr = CModel::TYPE_NONANIM == eModelType ? Load_Mesh_For_NonAnimModel(IFS) : Load_Mesh_For_AnimModel(IFS);
	if (FAILED(hr))
		return E_FAIL;

#pragma endregion

#pragma region INDEX_BUFFER

	// 8. 인덱스 정보 읽기
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	/* 인덱스 버퍼의 byte크기 */
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	m_pIndices = new _uint[m_iNumIndices];
	ZeroMemory(m_pIndices, sizeof(_uint) * m_iNumIndices);

	_uint		iNumIndices = { 0 };
	for (size_t i = 0; i < m_iNumIndices; i++)
	{
		IFS.read(reinterpret_cast<char*>(&m_pIndices[i]), sizeof(_uint));
	}

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = m_pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

#pragma endregion

	return S_OK;
}

#ifdef _ASSIMP
HRESULT CMesh::Ready_Vertices_For_NonAnimModel(const aiMesh* pAIMesh, _fmatrix TransformMatrix)
{
	m_iVertexStride = sizeof(VTXMESH);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	/* 정점버퍼의 byte크기 */
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	VTXMESH*		pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), TransformMatrix));
		memcpy(&m_pVerticesPos[i], &pVertices[i].vPosition, sizeof(_float3));


		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), TransformMatrix));
		
		if (nullptr != pAIMesh->mTextureCoords[0]) {
			memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		}

		if (nullptr != pAIMesh->mTangents) {
			memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
			XMStoreFloat3(&pVertices[i].vTangent, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vTangent), TransformMatrix));
		}

	}

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMesh::Ready_Vertices_For_AnimModel(const aiMesh* pAIMesh, const vector<CBone*>& Bones)
{
	m_iVertexStride = sizeof(VTXANIMMESH);
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	/* 정점버퍼의 byte크기 */
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	VTXANIMMESH*		pVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		// XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), TransformMatrix)); 안하는 이유?

		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&m_pVerticesPos[i], &pVertices[i].vPosition, sizeof(_float3));

		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
	}

	// 이 메쉬가 사용하는 뼈의 개수만큼 순회
	m_iNumBones = pAIMesh->mNumBones;

	for (_uint i = 0; i < m_iNumBones; ++i) {
		aiBone* pAIBone = pAIMesh->mBones[i];

		_float4x4 OffsetMatrix;
		memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));

		m_OffsetMatrices.push_back(OffsetMatrix);

		// 이 메쉬가 사용하는 뼈의 Model에서의 위치 찾기
		auto iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)-> _bool
			{
				return pBone->Compare_Name(pAIBone->mName.data);
			});

		// 찾은 뼈는 이 메쉬가 사용하는 뼈이므로 모델에서의 위치를 저장
		m_Bones.push_back(iter - Bones.begin());	// 메쉬에서의 i번째 위치에 모델에서의 뼈의 위치가 저장됨

		// 이 뼈가 영향을 주는 버텍스의 개수
		_uint		iNumWeights = pAIBone->mNumWeights;

		// 이 뼈가 영향을 주는 모든 버텍스에 이 뼈의 메쉬에서의 위치와 가중치를 넣어줘야함
		// 메쉬에서의 위치가 모델에서의 위치를 알려줌
		for (_uint j = 0; j < iNumWeights; j++) {
			// 비어있는 위치에 저장해야함
			// 최대 4개까지 저장
			if (0.0f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.x)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.x = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.x = pAIBone->mWeights[j].mWeight;
			}
			else if (0.0f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.y)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.y = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.y = pAIBone->mWeights[j].mWeight;
			}
			else if (0.0f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.z)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.z = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.z = pAIBone->mWeights[j].mWeight;
			}
			else if (0.0f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.w)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.w = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.w = pAIBone->mWeights[j].mWeight;
			}
		}
	}

	if (0 == m_iNumBones) {
		m_iNumBones = 1;

		auto	iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)->_bool
			{
				return pBone->Compare_Name(m_szName);
			});

		m_Bones.push_back(iter - Bones.begin());

		_float4x4		OffsetMatrix;
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());
		m_OffsetMatrices.push_back(OffsetMatrix);
	}

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}
#endif

HRESULT CMesh::Save_Mesh_For_NonAnimModel(ofstream& OFS)
{
	// 데이터를 읽기 위해 버퍼 생성
	D3D11_BUFFER_DESC BufferDesc{};

	BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	BufferDesc.Usage = D3D11_USAGE_STAGING;
	BufferDesc.BindFlags = 0;
	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	BufferDesc.MiscFlags = 0;
	BufferDesc.StructureByteStride = m_iVertexStride;
	
	ID3D11Buffer* pTempBuffer = { nullptr };

	if(FAILED(m_pDevice->CreateBuffer(&BufferDesc, nullptr, &pTempBuffer)))
		return E_FAIL;

	m_pContext->CopyResource(pTempBuffer, m_pVB);

	D3D11_MAPPED_SUBRESOURCE mappedResource = {};

	if (FAILED(m_pContext->Map(pTempBuffer, 0, D3D11_MAP_READ, 0, &mappedResource)))
		return E_FAIL;
	
	VTXMESH* pVertices = (VTXMESH*)(mappedResource.pData);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		OFS.write(reinterpret_cast<const char*>(&pVertices[i].vPosition), sizeof(_float3));
		OFS.write(reinterpret_cast<const char*>(&pVertices[i].vNormal), sizeof(_float3));
		OFS.write(reinterpret_cast<const char*>(&pVertices[i].vTexcoord), sizeof(_float2));
		OFS.write(reinterpret_cast<const char*>(&pVertices[i].vTangent), sizeof(_float3));
	}

	m_pContext->Unmap(pTempBuffer, 0);
	Safe_Release(pTempBuffer);

	return S_OK;
}

HRESULT CMesh::Save_Mesh_For_AnimModel(ofstream& OFS)
{
	// 1. AnimModel이므로 Vector Container 2개 저장해야함
	for (auto& pBoneIndex : m_Bones) {
		OFS.write(reinterpret_cast<const char*>(&pBoneIndex), sizeof(_uint));
	}
	for (auto& pOffsetMatrix : m_OffsetMatrices) {
		OFS.write(reinterpret_cast<const char*>(&pOffsetMatrix), sizeof(_float4x4));
	}

	// 2. 버텍스 읽기
	// 
	// 데이터를 읽기 위해 버퍼 생성
	D3D11_BUFFER_DESC BufferDesc{};

	BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	BufferDesc.Usage = D3D11_USAGE_STAGING;
	BufferDesc.BindFlags = 0;
	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	BufferDesc.MiscFlags = 0;
	BufferDesc.StructureByteStride = m_iVertexStride;

	ID3D11Buffer* pTempBuffer = { nullptr };

	if (FAILED(m_pDevice->CreateBuffer(&BufferDesc, nullptr, &pTempBuffer)))
		return E_FAIL;

	m_pContext->CopyResource(pTempBuffer, m_pVB);

	D3D11_MAPPED_SUBRESOURCE mappedResource = {};

	if (FAILED(m_pContext->Map(pTempBuffer, 0, D3D11_MAP_READ, 0, &mappedResource)))
		return E_FAIL;

	VTXANIMMESH* pVertices = (VTXANIMMESH*)(mappedResource.pData);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		OFS.write(reinterpret_cast<const char*>(&pVertices[i].vPosition), sizeof(_float3));
		OFS.write(reinterpret_cast<const char*>(&pVertices[i].vNormal), sizeof(_float3));
		OFS.write(reinterpret_cast<const char*>(&pVertices[i].vTexcoord), sizeof(_float2));
		OFS.write(reinterpret_cast<const char*>(&pVertices[i].vTangent), sizeof(_float3));
		OFS.write(reinterpret_cast<const char*>(&pVertices[i].vBlendIndices), sizeof(XMUINT4));
		OFS.write(reinterpret_cast<const char*>(&pVertices[i].vBlendWeights), sizeof(_float4));
	}

	m_pContext->Unmap(pTempBuffer, 0);
	Safe_Release(pTempBuffer);

	return S_OK;
}

HRESULT CMesh::Load_Mesh_For_NonAnimModel(ifstream& IFS)
{
	m_iVertexStride = sizeof(VTXMESH);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	/* 정점버퍼의 byte크기 */
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		IFS.read(reinterpret_cast<char*>(&pVertices[i].vPosition), sizeof(_float3));
		IFS.read(reinterpret_cast<char*>(&pVertices[i].vNormal), sizeof(_float3));
		IFS.read(reinterpret_cast<char*>(&pVertices[i].vTexcoord), sizeof(_float2));
		IFS.read(reinterpret_cast<char*>(&pVertices[i].vTangent), sizeof(_float3));

		memcpy(&m_pVerticesPos[i], &pVertices[i].vPosition, sizeof(_float3));
	}

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMesh::Load_Mesh_For_AnimModel(istream& IFS)
{
	for (size_t i = 0; i < m_iNumBones; ++i) {
		_uint iBoneIndex;
		IFS.read(reinterpret_cast<char*>(&iBoneIndex), sizeof(_uint));
		m_Bones.push_back(iBoneIndex);
	}

	for (size_t i = 0; i < m_iNumBones; ++i) {
		_float4x4 iOffsetMatrix;
		IFS.read(reinterpret_cast<char*>(&iOffsetMatrix), sizeof(_float4x4));
		m_OffsetMatrices.push_back(iOffsetMatrix);
	}

	m_iVertexStride = sizeof(VTXANIMMESH);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	/* 정점버퍼의 byte크기 */
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		IFS.read(reinterpret_cast<char*>(&pVertices[i].vPosition), sizeof(_float3));
		IFS.read(reinterpret_cast<char*>(&pVertices[i].vNormal), sizeof(_float3));
		IFS.read(reinterpret_cast<char*>(&pVertices[i].vTexcoord), sizeof(_float2));
		IFS.read(reinterpret_cast<char*>(&pVertices[i].vTangent), sizeof(_float3));
		IFS.read(reinterpret_cast<char*>(&pVertices[i].vBlendIndices), sizeof(XMUINT4));
		IFS.read(reinterpret_cast<char*>(&pVertices[i].vBlendWeights), sizeof(_float4));

		memcpy(&m_pVerticesPos[i], &pVertices[i].vPosition, sizeof(_float3));
	}

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

#ifdef _ASSIMP
CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, const aiMesh* pAIMesh, const vector<class CBone*>& Bones, _fmatrix TransformMatrix)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eModelType, pAIMesh, Bones, TransformMatrix)))
	{
		MSG_BOX(TEXT("Failed To Created : CMesh"));

		Safe_Release(pInstance);
	}

	return pInstance;
}
#endif

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, ifstream& IFS)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Load_Mesh(eModelType, IFS)))
	{
		MSG_BOX(TEXT("Failed To Load : CMesh"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CMesh* CMesh::Clone(void* pArg)
{
	return nullptr;
}

void CMesh::Free()
{
	__super::Free();

	if (false == m_isCloned) {
		Safe_Delete_Array(m_pIndices);
	}
}
