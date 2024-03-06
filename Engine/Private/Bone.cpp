#include "Bone.h"

CBone::CBone()
{
}

HRESULT CBone::Initialize(const aiNode* pAINode, _int iParentIndex)
{
	m_iParentBoneIndex = iParentIndex;

	strcpy_s(m_szName, pAINode->mName.data);	// aiNode���� ���� �̸� ��������

	memcpy(&m_TransformationMatrix, &pAINode->mTransformation, sizeof(_float4x4));	//aiNode���� TransformationMatrix ������
	// aiNode������ TransformationMatrix ���� ����� Col_Major�̹Ƿ� Row_Major�� ����
	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));
	// Initialize �ܰ迡�� CombinedTransformationMatrix(���� ��ġ)�� �׵� ��ķ� ����
	// Model�� PlayAnimation()���� ��� CombinedTransformationMatrix�� Update
	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

	return S_OK;
}

void CBone::Invalidate_CombinedTransformationMatrix(const vector<CBone*>& Bones, _fmatrix TransformationMatrix)
{
	// �� ���� RootNode�� ��� m_TransformationMatrix == m_CombinedTransformationMatrix
	if (-1 == m_iParentBoneIndex) {
		// �θ𿡼��� TransformationMatrix�� ���ؼ� ���� ��ü ��ġ�� �����Ѵ�
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * TransformationMatrix);
	}
	else {
		// �θ��� m_CombinedTransformationMatrix�� �����ͼ� �ڽ��� m_TransformationMatrix�� m_CombinedTransformationMatrix�� ���ؼ� �ڽ��� m_CombinedTransformationMatrix�� ���� 
		XMStoreFloat4x4(&m_CombinedTransformationMatrix,
			XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&Bones[m_iParentBoneIndex]->m_CombinedTransformationMatrix));
	}
}

HRESULT CBone::Save_Bone(ofstream& OFS)
{
	size_t szNameLength = strlen(m_szName);
	OFS.write(reinterpret_cast<const char*>(&szNameLength), sizeof(size_t));
	OFS.write(reinterpret_cast<const char*>(&m_szName), sizeof(char) * szNameLength);
	OFS.write(reinterpret_cast<const char*>(&m_TransformationMatrix), sizeof(_float4x4));
	OFS.write(reinterpret_cast<const char*>(&m_iParentBoneIndex), sizeof(_int));

	return S_OK;
}

HRESULT CBone::Load_Bone(ifstream& IFS)
{
	size_t szNameLength = 0;
	IFS.read(reinterpret_cast<char*>(&szNameLength), sizeof(size_t));
	IFS.read(reinterpret_cast<char*>(&m_szName), sizeof(_char) * szNameLength);
	IFS.read(reinterpret_cast<char*>(&m_TransformationMatrix), sizeof(_float4x4));
	IFS.read(reinterpret_cast<char*>(&m_iParentBoneIndex), sizeof(_int));
	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());
	return S_OK;
}

CBone* CBone::Create(const aiNode* pAINode, _int iParentIndex)
{
	CBone*		pInstance = new CBone();

	if (FAILED(pInstance->Initialize(pAINode, iParentIndex)))
	{
		MSG_BOX(TEXT("Failed To Created : CBone"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CBone* CBone::Create(ifstream& IFS)
{
	CBone* pInstance = new CBone();

	if (FAILED(pInstance->Load_Bone(IFS)))
	{
		MSG_BOX(TEXT("Failed To Load : CBone"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CBone* CBone::Clone()
{
	return new CBone(*this);
}

void CBone::Free()
{
}
