#pragma once

#include "Base.h"

/* ���� ������ ǥ���ϱ����� Ÿ�� ������ �����Ѵ�. */
/* aiNode, aiBone, aiAnimNode */

#pragma region Explain
/*
	1. aiNode : 

	2. aiBone :

	3. aiAnimNode : 

*/
#pragma endregion

BEGIN(Engine)

class ENGINE_DLL CBone final : public CBase
{
private:
	CBone();
	virtual ~CBone() = default;

public:
	void Set_TransformationMatrix(_fmatrix TransformationMatrix) {
		XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix);
	}

public:
	const _float4x4* Get_CombinedTransformationMatrix() const {
		return &m_CombinedTransformationMatrix;
	}

#ifdef _ASSIMP
	HRESULT Initialize(const aiNode* pAINode, _int iParentIndex);
#endif

	void Invalidate_CombinedTransformationMatrix(const vector<CBone*>& Bones, _fmatrix TransformationMatrix);

	_bool Compare_Name(const _char* pBoneName) {
		return !strcmp(m_szName, pBoneName);
	}
	
public:
	HRESULT Save_Bone(ofstream& OFS);

private:
	HRESULT Load_Bone(ifstream& IFS);

private:
	_char				m_szName[MAX_PATH] = { "" };	// �� ���� �̸�

	/* �� �� ���� ������� */
	_float4x4			m_TransformationMatrix;			// �� ���� ���� ��ġ. �θ� ����� ���ؼ� ���� ��ġ�� ����

	/* ���� �������. */
	_float4x4			m_CombinedTransformationMatrix; // �� ���� ���� ��ġ

	_int				m_iParentBoneIndex = { -1 };	// * model

public:

#ifdef _ASSIMP
	static CBone* Create(const aiNode* pAINode, _int  iParentIndex);
#endif

	static CBone* Create(ifstream& IFS);
	CBone* Clone();
	virtual void Free() override;
};

END