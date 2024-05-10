#include "stdafx.h"
#include "Weapon_Anim.h"
#include "Collider.h"
#include "Bone.h"

CWeapon_Anim::CWeapon_Anim(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CWeapon_Anim::CWeapon_Anim(const CWeapon_Anim& rhs)
	: CPartObject{ rhs }
{
}

HRESULT CWeapon_Anim::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWeapon_Anim::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	WEAPON_ANIM_DESC* pWeapon_Desc = (WEAPON_ANIM_DESC*)pArg;

	m_pSocket = pWeapon_Desc->pSocket;
	Safe_AddRef(m_pSocket);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components(pWeapon_Desc->strModelTag, pWeapon_Desc->Bounding_OBB_Desc, pWeapon_Desc->isAddCollider)))
		return E_FAIL;

	return S_OK;
}

void CWeapon_Anim::Tick(_float fTimeDelta)
{
	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocket->Get_CombinedTransformationMatrix());

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));

	if (nullptr != m_pColliderCom)
		m_pColliderCom->Tick(XMLoadFloat4x4(&m_WorldMatrix));

}

HRESULT CWeapon_Anim::Late_Tick(_float fTimeDelta)
{
	if (FAILED(__super::Late_Tick(fTimeDelta)))
		return E_FAIL;

	m_pModelCom->Play_Animation(fTimeDelta);

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(m_pSocket->Get_CombinedTransformationMatrix()) * XMLoadFloat4x4(m_pParentMatrix));
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponents(m_pColliderCom);
#endif

	return S_OK;
}

HRESULT CWeapon_Anim::Render()
{
	if (!m_isVisible)
		return S_OK;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS))) {
			if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture", i, aiTextureType_SPECULAR))) {
				return E_FAIL;
			}
		}

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		m_pModelCom->Render(i);
	}



	return S_OK;
}

HRESULT CWeapon_Anim::Render_LightDepth()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	_float4x4 ViewMatrix = m_pGameInstance->Get_Shadow_Transform_Float4x4(CPipeLine::D3DTS_VIEW);
	_float4x4 ProjMatrix = m_pGameInstance->Get_Shadow_Transform_Float4x4(CPipeLine::D3DTS_PROJ);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;


	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i)))
			return E_FAIL;

		/* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이ㅏ터를 다 던져야한다. */
		if (FAILED(m_pShaderCom->Begin(4)))
			return E_FAIL;

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

HRESULT CWeapon_Anim::Add_Components(const wstring& strModelTag, CBounding_OBB::BOUNDING_OBB_DESC Bounding_OBB_Desc, _bool isAddCollider)
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(g_Level, strModelTag,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	if(isAddCollider)
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &Bounding_OBB_Desc)))
			return E_FAIL;

	return S_OK;
}

CBone* CWeapon_Anim::Get_BonePtr(const char* pBoneName)
{
	return m_pModelCom->Get_BonePtr(pBoneName);
}

_float4 CWeapon_Anim::Get_BonePosition(const char* pBoneName)
{
	const _float4x4* pMatrix = m_pModelCom->Get_BonePtr(pBoneName)->Get_CombinedTransformationMatrix();
	
	_vector vScale;
	_vector vRoation;
	_vector vTranspose;

	
	XMMatrixDecompose(&vScale, &vRoation, &vTranspose, XMLoadFloat4x4(&m_WorldMatrix));

	XMMatrixRotationQuaternion(vRoation);

	/*
		_float4x4			m_WorldMatrix;					// 자신의 월드 행렬
	const _float4x4*	m_pParentMatrix = { nullptr };	// 이 파츠를 보유하고 있는 GameObject == Parent의 월드 행렬을 포인터로 보유
	*/
	_matrix vMatrix = XMLoadFloat4x4(pMatrix) * XMMatrixRotationQuaternion(vRoation);
	_float4x4 fMatrix;
	XMStoreFloat4x4(&fMatrix, vMatrix);

	_float4 vPos = { fMatrix._41 + vTranspose.m128_f32[0] ,	fMatrix._42 + vTranspose.m128_f32[1],	fMatrix._43 + vTranspose.m128_f32[2],	1.f};

	return vPos;
}

HRESULT CWeapon_Anim::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	/*if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;	*/

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CWeapon_Anim* CWeapon_Anim::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeapon_Anim* pInstance = new CWeapon_Anim(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CWeapon_Anim"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWeapon_Anim::Clone(void* pArg)
{
	CWeapon_Anim* pInstance = new CWeapon_Anim(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CWeapon_Anim"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon_Anim::Free()
{
	__super::Free();

	Safe_Release(m_pSocket);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pColliderCom);
}
