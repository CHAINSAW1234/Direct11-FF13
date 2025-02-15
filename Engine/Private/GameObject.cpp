#include "..\Public\GameObject.h"
#include "GameInstance.h"


CGameObject::CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CGameObject::CGameObject(const CGameObject & rhs)
	: m_pDevice(rhs.m_pDevice)
	, m_pContext(rhs.m_pContext)
	, m_pGameInstance(rhs.m_pGameInstance)
	, m_eLevel(rhs.m_eLevel)
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CComponent * CGameObject::Get_Component(const wstring & strComTag)
{
	auto	iter = m_Components.find(strComTag);

	if (iter == m_Components.end())
		return nullptr;

	return iter->second;	
}

HRESULT CGameObject::Initialize_Prototype()
{
	return S_OK;
}

/* 실제 게임내엣 사용되는 객체가 호출하는 함수다. */
HRESULT CGameObject::Initialize(void* pArg)
{
	if(nullptr != pArg)
	{
		GAMEOBJECT_DESC*		pGameObjectDesc = (GAMEOBJECT_DESC*)pArg;		
	}

	m_pTransformCom = CTransform::Create(m_pDevice, m_pContext);
	if (nullptr == m_pTransformCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Initialize(pArg)))
		return E_FAIL;

	m_Components.emplace(g_strTransformTag, m_pTransformCom);

	Safe_AddRef(m_pTransformCom);
	
	return S_OK;
}

void CGameObject::Start()
{
}

void CGameObject::Tick(_float fTimeDelta)
{
}

HRESULT CGameObject::Late_Tick(_float fTimeDelta)
{
	if (m_isDead)
		return E_FAIL;

	return S_OK;
}

HRESULT CGameObject::Render()
{
	return S_OK;
}



HRESULT CGameObject::Add_Component(_uint iLevelIndex, const wstring & strPrototypeTag, const wstring & strComponentTag, CComponent** ppOut, void * pArg)
{
	CComponent*		pComponent = m_pGameInstance->Clone_Component(iLevelIndex, strPrototypeTag, pArg);
	if (nullptr == pComponent)
		return E_FAIL;

	auto	iter = m_Components.find(strComponentTag);
	if (iter != m_Components.end())
		return E_FAIL;

	m_Components.emplace(strComponentTag, pComponent);
	
	*ppOut = pComponent;

	Safe_AddRef(pComponent);

	return S_OK;
}

HRESULT CGameObject::Compute_ViewZ()
{
	_float4		vPosition = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

	_matrix  ViewMatrix = m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW);

	
	XMStoreFloat4(&vPosition, XMVector3TransformCoord(XMLoadFloat4(&vPosition), ViewMatrix));
	
	m_fViewZ = vPosition.z;

	return S_OK;
}

void CGameObject::SetUp_BillBoard()
{
	_float3		vScaled = m_pTransformCom->Get_Scaled();
	_vector		vLook = m_pTransformCom->Get_State_Vector(CTransform::STATE_LOOK);
	_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);

	vLook = XMVector3Cross(vRight, XMVectorSet(0.f, 1.f, 0.f, 0.f));

	m_pTransformCom->Set_State(CTransform::STATE::STATE_RIGHT, XMVector3Normalize(vRight) * vScaled.x);
	m_pTransformCom->Set_State(CTransform::STATE::STATE_LOOK, XMVector3Normalize(vLook) * vScaled.z);
}

void CGameObject::Free()
{
	__super::Free();

	for (auto& Pair : m_Components)
		Safe_Release(Pair.second);

	m_Components.clear();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	
}
