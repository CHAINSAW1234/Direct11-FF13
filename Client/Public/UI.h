#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Observer.h"

BEGIN(ENGINE)
class CTexture;
END

BEGIN(Client)

class CUI abstract : public CGameObject, public CObserver
{
public:
	typedef struct UI_Desc {
		void* pObserver_Hander = { nullptr };		// 이 값이 비어있으면 즉시 폭팔해야함
													// 잘못된 타입을 전달하면 디버그하기 매우 까다로움
													// 이 값의 사용은 직접적인 실제 생성된 객체에서 할 것 
	} UI_DESC;

protected:
	CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI(const CUI& rhs);
	virtual ~CUI() = default;

public:
	void Set_Render(_bool isRender) { m_isRender = isRender; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void	Tick(_float fTimeDelta);
	virtual HRESULT Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void OnNotify();

protected:
	virtual HRESULT Bind_ShaderResources();

protected:
	CTexture*		m_pTextureCom = { nullptr };
	CShader*		m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	_bool			m_isRender = { true };
	_float			m_fX = { 0.f };					// x축 기준의 중점
	_float			m_fY = { 0.f };					// y축 기준의 중점 	
	_float			m_fSizeX = { 0.f };
	_float			m_fSizeY = { 0.f }; 
	_float4x4		m_ViewMatrix = {};
	_float4x4		m_ProjMatrix = {};

public:
	virtual CUI* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END