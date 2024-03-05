#pragma once

/* �����̳ʺе��� �������� ������ �ε����� ������ ���������ؼ� ����, �ε������۸� �����Ѵ�.  */
#include "VIBuffer.h"
#include "Model.h"

BEGIN(Engine)
   
// �޽��� ���� ������ �����ϴ� Ŭ����
class CMesh final : public CVIBuffer
{
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;

public:
	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}
	const _char* Get_Name() { return m_szName; }

public:
	virtual HRESULT Initialize_Prototype(CModel::TYPE eModelType, const aiMesh* pAIMesh, const vector<CBone*>& Bones, _fmatrix TransformMatrix);
	virtual HRESULT Initialize(void* pArg) override;
	virtual _bool	Compute_Picking(const CTransform* pTransform, _Out_  _float4* vOutPos = nullptr);

public:
	HRESULT Stock_Matrices(const vector<CBone*>& Bones, _Out_ _float4x4* pMeshBoneMatrices);

private:
	_char					m_szName[MAX_PATH] = { "" };	// �� �޽��� �̸�

	/* �� �޽ô� �𵨿��� �ε��س��� ���׸���� �� ���° ���׸����� �̿��ϴ°�? */
	_uint					m_iMaterialIndex = { 0 };	// �������� �޽ð� ������ ���׸����� ����ϴ� ��츦 ����� ������

	_uint					m_iNumBones = { 0 };	// �� �޽��� ����ϴ� ���� ����
	/* ���� �ε��� : ���� ������ �ִ� ��ü ���� ��, �޽ð� ����ϰ� �ִ� ���� �ε��� */
	vector<_uint>			m_Bones;				// �� �޽��� ����ϴ� ���� �𵨿����� �ε���
	

	/* ���� �޽ÿ� ������ �ִ� ������ ������� �������� �����ߴ�. */
	vector<_float4x4>		m_OffsetMatrices;		

	_uint*					m_pIndices = { nullptr };		// Picking�� ���� Indices ����
private:
	HRESULT Ready_Vertices_For_NonAnimModel(const aiMesh* pAIMesh, _fmatrix TransformationMatrix);
	HRESULT Ready_Vertices_For_AnimModel(const aiMesh* pAIMesh, const vector<CBone*>& Bones);

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, const aiMesh* pAIMesh, const vector<class CBone*>& Bones, _fmatrix TransformMatrix);
	virtual CMesh* Clone(void* pArg);
	virtual void Free() override;
};

END
