#include "..\Public\Renderer.h"
#include "GameObject.h"



CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CRenderer::Initialize()
{
	// 1. 알파 블랜딩을 위한 
	D3D11_BLEND_DESC BlendDesc;
	ZeroMemory(&BlendDesc, sizeof(D3D11_BLEND_DESC));
	BlendDesc.RenderTarget[0].BlendEnable = true;
	BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	if (FAILED(m_pDevice->CreateBlendState(&BlendDesc, &m_pBlendState)))
		return E_FAIL;


	// 2. UI 렌더링을 위한 z쓰기 끄기
	D3D11_DEPTH_STENCILOP_DESC Depth_Stencilop_desc = {};
	Depth_Stencilop_desc.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	Depth_Stencilop_desc.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	Depth_Stencilop_desc.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	Depth_Stencilop_desc.StencilFunc = D3D11_COMPARISON_ALWAYS;


	D3D11_DEPTH_STENCIL_DESC Depth_Stencil_Desc = {};
	Depth_Stencil_Desc.DepthEnable = true;
	Depth_Stencil_Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	Depth_Stencil_Desc.DepthFunc = D3D11_COMPARISON_LESS;
	Depth_Stencil_Desc.StencilEnable = FALSE;
	Depth_Stencil_Desc.StencilReadMask = 0xFF;
	Depth_Stencil_Desc.StencilWriteMask = 0xFF;
	Depth_Stencil_Desc.FrontFace = Depth_Stencilop_desc;
	Depth_Stencil_Desc.BackFace = Depth_Stencilop_desc;

	if (FAILED(m_pDevice->CreateDepthStencilState(&Depth_Stencil_Desc, &m_pRenderState_UI)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Add_RenderGroup(RENDERGROUP eRenderGroup, CGameObject * pRenderObject)
{
	if (eRenderGroup >= RENDER_END)
		return E_FAIL;

	m_RenderObjects[eRenderGroup].push_back(pRenderObject);

	Safe_AddRef(pRenderObject);

	return S_OK;
}

HRESULT CRenderer::Render()
{
	if (FAILED(Render_Priority()))
		return E_FAIL;
	if (FAILED(Render_NonBlend()))
		return E_FAIL;
	if (FAILED(Render_Blend()))
		return E_FAIL;
	if (FAILED(Render_UI()))
		return E_FAIL;
	if (FAILED(Render_UI_Late()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Priority()
{
	for (auto& pRenderObject : m_RenderObjects[RENDER_PRIORITY])
	{
		if(nullptr != pRenderObject)
			pRenderObject->Render();
		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_PRIORITY].clear();

	return S_OK;
}

HRESULT CRenderer::Render_NonBlend()
{
	for (auto& pRenderObject : m_RenderObjects[RENDER_NONBLEND])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();
		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_NONBLEND].clear();

	return S_OK;
}
//
//_bool Compare(CGameObject* pSour, CGameObject* pDest)
//{
//	return ((CBlendObject*)pSour)->Get_ViewZ() > ((CBlendObject*)pDest)->Get_ViewZ();
//}

HRESULT CRenderer::Render_Blend()
{
	//m_RenderObjects[RENDER_BLEND].sort([](CGameObject* pSour, CGameObject* pDest)->_bool
	//{
	//	return ((CBlendObject*)pSour)->Get_ViewZ() > ((CBlendObject*)pDest)->Get_ViewZ();
	//});

	m_pContext->OMSetBlendState(m_pBlendState, 0, 0xffff);

	for (auto& pRenderObject : m_RenderObjects[RENDER_BLEND])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();
		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_BLEND].clear();

	m_pContext->OMSetBlendState(nullptr, 0, 0xffff);

	return S_OK;
}

HRESULT CRenderer::Render_UI()
{

	m_pContext->OMSetBlendState(m_pBlendState, 0, 0xffff);
	m_pContext->OMSetDepthStencilState(m_pRenderState_UI, 1);

	for (auto& pRenderObject : m_RenderObjects[RENDER_UI])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();	
		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_UI].clear();

	m_pContext->OMSetDepthStencilState(nullptr, 1);
	m_pContext->OMSetBlendState(nullptr, 0, 0xffff);
	return S_OK;
}

HRESULT CRenderer::Render_UI_Late()
{

	m_pContext->OMSetBlendState(m_pBlendState, 0, 0xffff);
	m_pContext->OMSetDepthStencilState(m_pRenderState_UI, 1);

	for (auto& pRenderObject : m_RenderObjects[RENDER_UI_LATE])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();
		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_UI_LATE].clear();

	m_pContext->OMSetDepthStencilState(nullptr, 1);
	m_pContext->OMSetBlendState(nullptr, 0, 0xffff);

	return S_OK;
}

CRenderer * CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRenderer*		pInstance = new CRenderer(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CRenderer"));

		Safe_Release(pInstance);
	}

	return pInstance;
}


void CRenderer::Free()
{
	__super::Free();

	for (auto& RenderList : m_RenderObjects)
	{
		for (auto& pRenderObject : RenderList)		
			Safe_Release(pRenderObject);
		RenderList.clear();		
	}

	Safe_Release(m_pBlendState);
	Safe_Release(m_pRenderState_UI);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
