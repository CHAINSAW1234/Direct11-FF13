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

void CBone::Free()
{
}
