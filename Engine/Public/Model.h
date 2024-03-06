#pragma once

#include "Component.h"
#include "Animation.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };

private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}
	const vector<class CMesh*> Get_Meshes() { return m_Meshes; }

	_bool isFinished() {
		return m_Animations[m_iCurrentAnimIndex]->isFinished();
	}
public:
	void Set_Animation(_uint iAnimIndex, _bool isLoop) {
		m_iCurrentAnimIndex = iAnimIndex;
		m_isLoop = isLoop;
	}

public:
	virtual HRESULT Initialize_Prototype(TYPE eType, const string& strModelFilePath, _fmatrix TransformMatrix);
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eTextureType);
	HRESULT Play_Animation(_float fTimeDelta);
	HRESULT Render(_uint iMeshIndex);

public:
	_bool	Compute_Picking(const class CTransform* pTransform, _Out_ _float4* vOutPos = nullptr);

	HRESULT	Save_Model(string filepath);

private:
	
private:
	TYPE						m_eModelType = { TYPE_END };	// �ִϸ��̼� �𵨰� ��ִϸ��̼� �� ���� �ʿ���
	const aiScene*				m_pAIScene = { nullptr };
	Assimp::Importer			m_Importer;

	// ���� �������� aiScene�� ������ ��� ������ �����ͼ� �����ϴ� ��������
	// ���� Initialize���� m_pAIScene ��� ���� 
private:
	_uint						m_iNumMeshes = { 0 };
	vector<class CMesh*>		m_Meshes;

	_uint						m_iNumMaterials = { 0 };
	vector<MESH_MATERIAL>		m_Materials;

	// ������, ȸ�����, �����ϵ��� ���� ���
	// �𵨿� ���� �ٸ��Ƿ�, �δ����� ó��
	_float4x4					m_TransformMatrix;

	// �� ���� ����ϴ� ��� ���� �����ϰ� ����
	vector<class CBone*>		m_Bones;

	_uint						m_iNumAnimations = { 0 };
	_uint						m_iCurrentAnimIndex = { 0 };
	_bool						m_isLoop = { false };
	vector<class CAnimation*>	m_Animations;

	// ���̴��� ������ m_OffsetMatrices
	// ���̴� ���ε� ������ �޽��� ���� �޾ƿ�
	_float4x4					m_MeshBoneMatrices[512];
private:
	HRESULT Ready_Meshes();
	HRESULT Ready_Materials(const _char* pModelFilePath);
	HRESULT Ready_Bones(aiNode* pAINode, _int iParentIndex = -1);
	HRESULT Ready_Animations();

	HRESULT Save_NonAnimModel(ofstream& OFS);
	HRESULT Save_AnimModel(ofstream& OFS);
public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const string& strModelFilePath, _fmatrix TransformMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END