#pragma once

/* �����̳ʺе��� �������� ������ �ε����� ������ ���������ؼ� ����, �ε������۸� �����Ѵ�.  */
#include "VIBuffer.h"

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

public:
	virtual HRESULT Initialize_Prototype(const aiMesh* pAIMesh);
	virtual HRESULT Initialize(void* pArg) override;

private:
	_char			m_szName[MAX_PATH] = { "" };
	 
	/* �� �޽ô� �𵨿��� �ε��س��� ���׸���� �� ���° ���׸����� �̿��ϴ°�? */
	_uint			m_iMaterialIndex = { 0 };	// �������� �޽ð� ������ ���׸����� ����ϴ� ��츦 ����� ������

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const aiMesh* pAIMesh);
	virtual CMesh* Clone(void* pArg);
	virtual void Free() override;
};

END
