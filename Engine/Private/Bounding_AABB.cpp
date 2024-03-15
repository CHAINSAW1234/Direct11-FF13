#include "Bounding_AABB.h"
#include "DebugDraw.h"

CBounding_AABB::CBounding_AABB(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CBounding{ pDevice, pContext }
{

}

HRESULT CBounding_AABB::Initialize(CBounding::BOUNDING_DESC* pBoundingDesc)
{
	BOUNDING_AABB_DESC*		pDesc = (BOUNDING_AABB_DESC*)pBoundingDesc;

	m_pBoundingDesc = new BoundingBox(pDesc->vCenter, _float3(pDesc->vSize.x * 0.5f, pDesc->vSize.y * 0.5f, pDesc->vSize.z * 0.5f));

	return S_OK;
}

#ifdef _DEBUG

HRESULT CBounding_AABB::Render(PrimitiveBatch<VertexPositionColor>* pBatch)
{
	//DX::Draw(pBatch, *m_pBoundingDesc);

	return S_OK;
}

#endif

CBounding_AABB * CBounding_AABB::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, CBounding::BOUNDING_DESC* pBoundingDesc)
{
	CBounding_AABB*		pInstance = new CBounding_AABB(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pBoundingDesc)))
	{
		MSG_BOX(TEXT("Failed To Created : CBounding_AABB"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

void CBounding_AABB::Free()
{
	__super::Free();

	Safe_Delete(m_pBoundingDesc);
}
