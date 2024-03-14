#include "stdafx.h"
#include "Grid.h"

#include "VIBuffer_Line.h"

CGrid::CGrid(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CGrid::CGrid(const CGrid& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CGrid::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGrid::Initialize(void* pArg)
{
	GAMEOBJECT_DESC		GameObjectDesc{};

	GameObjectDesc.fSpeedPerSec = 10.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CGrid::Tick(_float fTimeDelta)
{
}

HRESULT CGrid::Late_Tick(_float fTimeDelta)
{
	if (FAILED(__super::Late_Tick(fTimeDelta)))
		return E_FAIL;
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

	return S_OK;
}

HRESULT CGrid::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	/* �� �Լ� ���ο��� ȣ��Ǵ� Apply�Լ� ȣ�� ������ ���̴� ������ �������� ��� ���̤��͸� �� �������Ѵ�. */
	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGrid::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPos"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	CVIBuffer_Line::VIBUFFER_LINE_DESC Description = {};
	Description.iNumVerticesX = Description.iNumVerticesZ = 600;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, TEXT("Prototype_Component_VIBuffer_Line"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom, &Description)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float4(((_float)Description.iNumVerticesX)/2 * -1, 0, ((_float)Description.iNumVerticesX)/2 * -1, 1));


	return S_OK;
}

HRESULT CGrid::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CGrid* CGrid::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGrid* pInstance = new CGrid(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CGrid"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGrid::Clone(void* pArg)
{
	CGrid* pInstance = new CGrid(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CGrid"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGrid::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
