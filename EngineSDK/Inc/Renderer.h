#pragma once

#include "Base.h"

/* 1. 화면에 그려져야할 객체들만 그려지는 순서대로 보관하는 클래스이다.*/
/* 2. 보관하고 있는 순서대로 객체들의 Draw콜(렌더함수를호출한다.)을 수행한다.*/

BEGIN(Engine)

class CRenderer final : public CBase
{
public:
	enum RENDERGROUP { RENDER_PRIORITY, RENDER_SSAO_OBJECT, RENDER_NONBLEND, RENDER_SHADOW, RENDER_BRIGHT, RENDER_NONLIGHT, RENDER_BLEND, RENDER_UI, RENDER_UI_LATE, RENDER_END };
private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderer() = default;
public:
	HRESULT Initialize();
	HRESULT Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pRenderObject);
	HRESULT Render();


#ifdef _DEBUG
public:
	HRESULT Add_DebugComponents(class CComponent* pRenderObject);
#endif

private:
	ID3D11Device*						m_pDevice = { nullptr };
	ID3D11DeviceContext*				m_pContext = { nullptr };
	class CGameInstance*				m_pGameInstance = { nullptr };
	list<class CGameObject*>			m_RenderObjects[RENDER_END];

	_float m_fViewPortWidth = {};
	_float m_fViewPortHeight = {};

private:
	class CVIBuffer_Rect*	m_pVIBuffer = { nullptr };
	class CShader*			m_pShader = { nullptr };
	class CTexture*			m_pTexture = { nullptr };

	_float4x4				m_WorldMatrix{}, m_ViewMatrix{}, m_ProjMatrix{};
	ID3D11DepthStencilView* m_pLightDepthDSV = { nullptr };

#ifdef _DEBUG
private:
	list<class CComponent*>				m_DebugComponents;
#endif

private:
	HRESULT Render_Priority();

	// 맵에는 SSAO 적용
	HRESULT Render_SSAO_Objects();
	HRESULT Render_SSAO_Lights();
	HRESULT Render_SSAO_Blur();
	HRESULT Render_SSAO_Result();

	HRESULT Render_Bright();	// renderbright 이후 글로우까지 먹임
	HRESULT Render_Shadow();
	HRESULT Render_NonBlend();
	HRESULT Render_NonLight();
	HRESULT Render_Blend();
	HRESULT Render_UI();
	HRESULT Render_UI_Late();

private:
	HRESULT Render_Lights();
	HRESULT Render_Result();

#ifdef _DEBUG
private:
	HRESULT Render_Debug();
#endif

public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;


};

END