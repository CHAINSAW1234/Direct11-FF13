#include "..\Public\Bounding_Sphere.h"
#include "DebugDraw.h"

CBounding_Sphere::CBounding_Sphere(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CBounding{ pDevice, pContext }
{

}

HRESULT CBounding_Sphere::Initialize(CBounding::BOUNDING_DESC* pBoundingDesc)
{
	return S_OK;
}

#ifdef _DEBUG

HRESULT CBounding_Sphere::Render(PrimitiveBatch<VertexPositionColor>* pBatch)
{
	//DX::Draw(pBatch, *m_pBoundingDesc);

	return S_OK;
}

#endif

CBounding_Sphere * CBounding_Sphere::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, CBounding::BOUNDING_DESC* pBoundingDesc)
{
	CBounding_Sphere*		pInstance = new CBounding_Sphere(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pBoundingDesc)))
	{
		MSG_BOX(TEXT("Failed To Created : CBounding_Sphere"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

void CBounding_Sphere::Free()
{
	__super::Free();
}
