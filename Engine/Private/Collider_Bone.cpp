#include "Collider_Bone.h"
#include "Bone.h"
#include "GameInstance.h"

CCollider_Bone::CCollider_Bone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCollider{ pDevice, pContext }
{
}

CCollider_Bone::CCollider_Bone(const CCollider_Bone& rhs)
	: CCollider{rhs}
{
}

HRESULT CCollider_Bone::Initialize_Prototype(TYPE eType)
{
	if(FAILED(__super::Initialize_Prototype(eType)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCollider_Bone::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	COLLIDER_BONE_DESC* pCollider_Bone_Desc = (COLLIDER_BONE_DESC*)pArg;
	m_pSocket = pCollider_Bone_Desc->pSocket;
	Safe_AddRef(m_pSocket);

	CBounding::BOUNDING_DESC* pBoundDesc = pCollider_Bone_Desc->pBounding_Desc;

	if (FAILED(__super::Initialize(pBoundDesc)))
		return E_FAIL;

	return S_OK;
}

void CCollider_Bone::Tick(_fmatrix WorldMatrix)
{
	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocket->Get_CombinedTransformationMatrix());

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	__super::Tick(SocketMatrix * WorldMatrix);

}

CCollider_Bone* CCollider_Bone::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType)
{
	CCollider_Bone* pInstance = new CCollider_Bone(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType)))
	{
		MSG_BOX(TEXT("Failed To Created : CCollider_Bone"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CCollider_Bone::Clone(void* pArg)
{
	CComponent* pInstance = new CCollider_Bone(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CCollider_Bone"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollider_Bone::Free()
{
	__super::Free();
	Safe_Release(m_pSocket);
}
