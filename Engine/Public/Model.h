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
	/*========================Get==============================*/
	TYPE Get_Type() const { return m_eModelType; }

	class CBone* Get_BonePtr(const _char* pBoneName) const;
	// Mesh ����
	_uint Get_NumMeshes() const { return m_iNumMeshes; }
	const vector<class CMesh*> Get_Meshes() { return m_Meshes; }

	// Animation ���� 
	_uint Get_CurrentAnimationIndex() { return m_iCurrentAnimIndex; }
	_float Get_CurrentTrackPosition() { 
		if (m_iNextAnimIndex != INFINITE) {
			return m_Animations[m_iNextAnimIndex]->Get_TrackPosition();
		}
		return m_Animations[m_iCurrentAnimIndex]->Get_TrackPosition();
	}
	_bool isFinished() { 
		if (m_iNextAnimIndex != INFINITE) {
			return m_Animations[m_iNextAnimIndex]->isFinished();
		}
		return m_Animations[m_iCurrentAnimIndex]->isFinished(); }

public:
	void Set_Animation(_uint iAnimIndex, _bool isLoop) {
		if (iAnimIndex < m_iNumAnimations) {	// �ִϸ��̼� ��ȯ �߿� �ִϸ��̼� ������ ��û�ϸ� ����

			if (m_iNextAnimIndex == INFINITE)
				m_fTime_Iinear_Interpolation = 0;
			m_NextAnimationLoop = isLoop;
			// ���� statement�� ���� ������ ����
			m_iNextAnimIndex = iAnimIndex;
		}
	}

	void Set_TrackPosition(_float fTrackPosition) {
		if (m_iNextAnimIndex != INFINITE) {
			m_Animations[m_iNextAnimIndex]->Set_TrackPosition(fTrackPosition);
			return;
		}
		m_Animations[m_iCurrentAnimIndex]->Set_TrackPosition(fTrackPosition);
	}

	void Set_TransformMatrix(_float4x4 TransformMatrix) {
		// ���� ������� ����
		m_TransformMatrix = TransformMatrix;
		if (m_eModelType == TYPE_NONANIM) {
			// ���� ��ü ��ü;
		}
	}

public:
#ifdef _ASSIMP
	virtual HRESULT Initialize_Prototype(TYPE eType, const string& strModelFilePath, _fmatrix TransformMatrix);
#endif
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex, _uint eTextureType);
	HRESULT Play_Animation(_float fTimeDelta);
	HRESULT Render(_uint iMeshIndex);

public:
	_bool	Compute_Picking(const class CTransform* pTransform, _Out_ _float4* vOutPos = nullptr);

#ifdef _ASSIMP
	HRESULT	Save_Model(string filepath);
#endif

private:
	HRESULT Play_Animation_Linear_Interpolation(_float fTimeDelta);
	HRESULT	Load_Model(string filepath);

private:
	TYPE						m_eModelType = { TYPE_END };	// �ִϸ��̼� �𵨰� ��ִϸ��̼� �� ���� �ʿ���
#ifdef _ASSIMP
	const aiScene*				m_pAIScene = { nullptr };
	Assimp::Importer			m_Importer;
#endif
	// ���� �������� aiScene�� ������ ��� ������ �����ͼ� �����ϴ� ��������
	// ���� Initialize���� m_pAIScene ��� ���� 
private:
	_uint						m_iNumMeshes = { 0 };
	vector<class CMesh*>		m_Meshes;

	_uint						m_iNumMaterials = { 0 };
	vector<MESH_MATERIAL>		m_Materials;

	// ������, ȸ�����, �����ϵ��� ���� ���
	// �𵨿� ���� �ٸ��Ƿ�, �δ����� ó��
	_float4x4					m_TransformMatrix = {};

	// �� ���� ����ϴ� ��� ���� �����ϰ� ����
	vector<class CBone*>		m_Bones;

	_uint						m_iNumAnimations = { 0 };
	_uint						m_iCurrentAnimIndex = { 0 };
	_bool						m_isLoop = { false };
	vector<class CAnimation*>	m_Animations;

	// ���̴��� ������ m_OffsetMatrices
	// ���̴� ���ε� ������ �޽��� ���� �޾ƿ�
	_float4x4					m_MeshBoneMatrices[512] = {};

	// ���� �ִϸ��̼� �ε����� ����
	_uint						m_iNextAnimIndex = { INFINITE };
	_bool						m_NextAnimationLoop = { false };
	_float						m_fTime_Iinear_Interpolation = { 0.f};
	
private:
#ifdef _ASSIMP
	HRESULT Ready_Meshes();
	HRESULT Ready_Materials(const _char* pModelFilePath);
	HRESULT Ready_Bones(aiNode* pAINode, _int iParentIndex = -1);
	HRESULT Ready_Animations();
#endif

public:
#ifdef _ASSIMP
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const string& strModelFilePath, _fmatrix TransformMatrix);
#endif
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const string& strModelFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END
