#pragma once

/* 클라이언트개발자가 엔진의 기능을 사용하기위해서 항상 접근해야하는 클래스. */
#include "Renderer.h"
#include "Component_Manager.h"
#include "PipeLine.h"
#include "Sound_Manager.h"
BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
	HRESULT Initialize_Engine(HINSTANCE hInstance, _uint iNumLevels, const ENGINE_DESC& EngineDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext);
	void Tick_Engine(_float fTimeDelta);
	HRESULT Begin_Draw(const _float4& vClearColor);
	HRESULT End_Draw();
	HRESULT Draw();
	HRESULT Clear(_uint iClearLevelIndex);

public: /* For.Input_Device */
	_byte	Get_KeyState(_uint eState, _ubyte byKeyID);
	_byte	Get_DIMouseState(MOUSEKEYSTATE eMouse);
	_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState);

public: /* For.Renderer */
	HRESULT Add_RenderGroup(CRenderer::RENDERGROUP eRenderGroup, class CGameObject* pRenderObject);

#ifdef _DEBUG
public:
	HRESULT Add_DebugComponents(class CComponent* pRenderComponent);
#endif

public: /* For.Level_Manager */
	HRESULT Open_Level(_uint iNewLevelID, class CLevel* pNewLevel);

public: /* For.Object_Manager */
	void Start();
	HRESULT Add_Prototype(const wstring& strPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_Clone(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg = nullptr);
	HRESULT Add_Clone(_uint iLevelIndex, const wstring& strLayerTag, class CGameObject* pGameObject);
	class CGameObject* Add_Clone_With_Object(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg = nullptr);
	class CGameObject* Clone_GameObject(const wstring& strPrototypeTag, void* pArg = nullptr);
	class CGameObject* Get_GameObject(_uint iLevelIndex, const wstring& strLayerTag, _uint iIndex = 0);
	const CComponent* Get_Component(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strComTag, _uint iIndex = 0);
	size_t	Get_LayerCnt(_uint iLevelIndex, const wstring& strLayerTag);

public: /* For.Component_Manager */
	HRESULT Add_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const wstring& strPrototypeTag, void* pArg = nullptr);


public: /* For.Timer_Manager */
	HRESULT Add_Timer(const wstring& strTimerTag);
	_float Compute_TimeDelta(const wstring& strTimerTag);

public: /* For.Picking */
	void Transform_PickingToLocalSpace(const class CTransform* pTransform, _Out_ _float4* pRayDir, _Out_ _float4* pRayPos);

public: /* For.PipeLine */
	void Set_Transform(CPipeLine::TRANSFORMSTATE eState, _fmatrix TransformMatrix);
	_matrix Get_Transform_Matrix(CPipeLine::TRANSFORMSTATE eState) const;
	_float4x4 Get_Transform_Float4x4(CPipeLine::TRANSFORMSTATE eState) const;
	_matrix Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORMSTATE eState) const;
	_float4x4 Get_Transform_Float4x4_Inverse(CPipeLine::TRANSFORMSTATE eState) const;
	_vector Get_CamPosition_Vector() const;
	_float4 Get_CamPosition_Float4() const;

public: /* For.Light_Manager */
	const LIGHT_DESC* Get_LightDesc(_uint iIndex);
	HRESULT Add_Light(const LIGHT_DESC& LightDesc);
	HRESULT Render_Lights(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

public: /* For.Font_Manager */
	HRESULT Add_Font(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strFontTag, const wstring& strFontFilePath);
	HRESULT Render_Font(const wstring& strFontTag, const wstring& strText, const _float2& vPosition, _fvector vColor, _float fRadian);


public: /* For.Target_Manager */
	HRESULT Add_RenderTarget(const wstring& strRenderTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT Add_MRT(const wstring& strMRTTag, const wstring& strRenderTargetTag);
	HRESULT Begin_MRT(const wstring& strMRTTag);
	HRESULT End_MRT();
	HRESULT Bind_RTShaderResource(class CShader* pShader, const wstring& strRenderTargetTag, const _char* pConstantName);
	HRESULT Copy_Resource(const wstring& strRenderTargetTag, ID3D11Texture2D** ppTextureHub);

public: /* For.Frustum */
	_bool isInFrustum_WorldSpace(_fvector vWorldPos, _float fRange = 0.f);

public: /* For.Extractor */
	_vector Compute_WorldPos(const _float2& vViewportPos, const wstring& strZRenderTargetTag, _uint iOffset = 0);

public:	/* For. Sound_Manager */
	_uint	VolumeUp(CSound_Manager::CHANNELID eID, _float _vol);
	_uint	VolumeDown(CSound_Manager::CHANNELID eID, _float _vol);
	_uint	BGMVolumeUp(_float _vol);
	_uint	BGMVolumeDown(_float _vol);
	_uint	Pause(CSound_Manager::CHANNELID eID);
	void	PlaySoundW(TCHAR* pSoundKey, CSound_Manager::CHANNELID eID, _float _vol);		// 호출 마다 다시 재생
	void	PlaySoundOnce(TCHAR* pSoundKey, CSound_Manager::CHANNELID eID, _float _vol);	// 여러 번 호출해도 한 번 재생
	int		PlaySoundLoop(TCHAR* pSoundKey, CSound_Manager::CHANNELID eID, _float _vol);	// 루프 횟수 반환
	void	SetChannelOneOff(_bool bOneOff, CSound_Manager::CHANNELID eID);
	void	PlaySoundOneOff(TCHAR* pSoundKey, CSound_Manager::CHANNELID eID, _float _vol);
	void	SetLoopCount(CSound_Manager::CHANNELID eID, _uint iCount);
	void	PlayBGM(TCHAR* pSoundKey);
	void	StopSound(CSound_Manager::CHANNELID eID);
	void	StopAll();

#ifdef _DEBUG
	HRESULT Ready_RTVDebug(const wstring& strRenderTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Draw_RTVDebug(const wstring& strMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

#endif

private:
	class CGraphic_Device*			m_pGraphic_Device = { nullptr };
	class CInput_Device*			m_pInput_Device = { nullptr };
	class CLevel_Manager*			m_pLevel_Manager = { nullptr };
	class CObject_Manager*			m_pObject_Manager = { nullptr };
	class CComponent_Manager*		m_pComponent_Manager = { nullptr };
	class CRenderer*				m_pRenderer = { nullptr };
	class CTimer_Manager*			m_pTimer_Manager = { nullptr };
	class CPicking*					m_pPicking = { nullptr };
	class CFont_Manager*			m_pFont_Manager = { nullptr };
	class CPipeLine*				m_pPipeLine = { nullptr };
	class CLight_Manager*			m_pLight_Manager = { nullptr };
	class CTarget_Manager*			m_pTarget_Manager = { nullptr };
	class CFrustum*					m_pFrustum = { nullptr };
	class CExtractor*				m_pExtractor = { nullptr };
	class CSound_Manager*			m_pSound_Manager = { nullptr };


public:		
	static void Release_Engine();
	virtual void Free() override;
};

END