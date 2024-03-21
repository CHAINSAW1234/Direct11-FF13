#pragma once

#include "Base.h"

/* 1. ȭ�鿡 �׷������� ��ü�鸸 �׷����� ������� �����ϴ� Ŭ�����̴�.*/
/* 2. �����ϰ� �ִ� ������� ��ü���� Draw��(�����Լ���ȣ���Ѵ�.)�� �����Ѵ�.*/

BEGIN(Engine)

class CRenderer final : public CBase
{
public:
	enum RENDERGROUP { RENDER_PRIORITY, RENDER_NONBLEND, RENDER_BLEND, RENDER_UI, RENDER_UI_LATE, RENDER_END };
private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderer() = default;
public:
	HRESULT Initialize();
	HRESULT Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pRenderObject);
	HRESULT Render();
	
private:
	ID3D11Device*						m_pDevice = { nullptr };
	ID3D11DeviceContext*				m_pContext = { nullptr };
	list<class CGameObject*>			m_RenderObjects[RENDER_END];

	ID3D11DepthStencilState*			m_pRenderState_UI = {nullptr};

private:
	HRESULT Render_Priority();
	HRESULT Render_NonBlend();
	HRESULT Render_Blend();
	HRESULT Render_UI();
	HRESULT Render_UI_Late();

public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;


};

END