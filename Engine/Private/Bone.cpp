#include "Bone.h"

CBone::CBone()
{
}

HRESULT CBone::Initialize(const aiNode* pAINode, _int iParentIndex)
{
	m_iParentBoneIndex = iParentIndex;

	strcpy_s(m_szName, pAINode->mName.data);	// aiNode에서 뼈의 이름 가져오기

	memcpy(&m_TransformationMatrix, &pAINode->mTransformation, sizeof(_float4x4));	//aiNode에서 TransformationMatrix 가져옴
	// aiNode에서의 TransformationMatrix 정의 방식이 Col_Major이므로 Row_Major로 변경
	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));
	// Initialize 단계에서 CombinedTransformationMatrix(실제 위치)는 항등 행렬로 설정
	// Model의 PlayAnimation()에서 모든 CombinedTransformationMatrix를 Update
	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

	return S_OK;
}

void CBone::Invalidate_CombinedTransformationMatrix(const vector<CBone*>& Bones, _fmatrix TransformationMatrix)
{
	// 이 뼈가 RootNode인 경우 m_TransformationMatrix == m_CombinedTransformationMatrix
	if (-1 == m_iParentBoneIndex) {
		// 부모에서만 TransformationMatrix를 곱해서 모델의 전체 위치를 수정한다
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * TransformationMatrix);
	}
	else {
		// 부모의 m_CombinedTransformationMatrix를 가져와서 자신의 m_TransformationMatrix와 m_CombinedTransformationMatrix를 곱해서 자신의 m_CombinedTransformationMatrix를 구함 
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
