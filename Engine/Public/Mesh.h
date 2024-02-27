#pragma once

/* 디자이너분들이 저장해준 정점과 인덱스의 정보를 바탕으로해서 정점, 인덱스버퍼를 생성한다.  */
#include "VIBuffer.h"

BEGIN(Engine)
   
// 메쉬의 정점 정보를 저장하는 클래스
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
	 
	/* 이 메시는 모델에서 로드해놓은 머테리얼들 중 몇번째 머테리얼을 이용하는가? */
	_uint			m_iMaterialIndex = { 0 };	// 여러개의 메시가 동일한 머테리얼을 사용하는 경우를 대비해 지정함

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const aiMesh* pAIMesh);
	virtual CMesh* Clone(void* pArg);
	virtual void Free() override;
};

END
