#include "..\Public\Bounding_OBB.h"
#include "DebugDraw.h"

CBounding_OBB::CBounding_OBB(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CBounding{ pDevice, pContext }
{

}

HRESULT CBounding_OBB::Initialize(CBounding::BOUNDING_DESC* pBoundingDesc)
{
	return S_OK;
}

#ifdef _DEBUG

HRESULT CBounding_OBB::Render(PrimitiveBatch<VertexPositionColor>* pBatch)
{
	//DX::Draw(pBatch, *m_pBoundingDesc);

	return S_OK;
}

#endif

CBounding_OBB * CBounding_OBB::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, CBounding::BOUNDING_DESC* pBoundingDesc)
{
	CBounding_OBB*		pInstance = new CBounding_OBB(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pBoundingDesc)))
	{
		MSG_BOX(TEXT("Failed To Created : CBounding_OBB"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

void CBounding_OBB::Free()
{
	__super::Free();
}
