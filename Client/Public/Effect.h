#pragma once
#include "Effect_Interface.h"

BEGIN(Client)
class CEffect abstract : public CGameObject
{
public:
	enum TYPE { EFFECT_2D, EFFECT_3D, EFFECT_INSTANCE, EFFECT_END };

	typedef struct Effect_Desc : public CGameObject::GAMEOBJECT_DESC
	{
		_float4 vColor = {};
		_float  fEffectTimeDelta = { 10.f };
		string	strEffectName;
	} EFFECT_DESC;
protected:
	CEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect(const CEffect& rhs);
	virtual ~CEffect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Start() override;

public:
	/*========================Get=============================*/
	TYPE Get_EffectType() { return m_eType; }
	_uint Get_DiffuseTexture() { return m_iDiffuseTextureIndex; }
	_uint Get_MaskTexture() { return m_iMaskTextureIndex; }
	_uint Get_DissolveTexture() { return m_iDissolveTextureIndex; }
	_float Get_Color_Magnification() { return m_fColorMagnification; }
	_float4 Get_Color() { return m_vColor; }
	_float Get_EffectTime() { return m_fEffectTimeDelta; }
	_float2 Get_TextureTimeDelta() { return m_fTextureMovementTimeDelta; }
	_float4 Get_Turn() { return m_vTurnDir; }
	_float Get_TurnSpeed() { return m_fTurnSpeed; }
	_float4* Get_MaskValue() { return m_vMaskValue; }
	_float3 Get_LerpScaleStart() { return m_vLerpScaleStart; }
	_float3 Get_LerpScaleEnd() { return m_vLerpScaleEnd; }
	string Get_Effect_Name() { return m_strEffectName; }

	/*========================Set=============================*/
	void Set_Color_Magnification(_float fColorMagnification) { m_fColorMagnification = fColorMagnification; }
	void Set_Color(_float4 vColor) { m_vColor = vColor; }
	void Set_EffectTime(_float fEffectTime) { m_fEffectTimeDelta = fEffectTime; }
	void Set_TextureMovement(_float2 fMovementTimeDelta) { m_fTextureMovementTimeDelta = fMovementTimeDelta; }
	void Set_Turn(_float4 vTurnDir);
	void Set_TurnSpeed(_float fTurnSpeed) { m_fTurnSpeed = fTurnSpeed; }
	void Set_DiffuseTexture(_uint iIndex) { m_iDiffuseTextureIndex = iIndex; }
	void Set_MaskTexture(_uint iIndex) { m_iMaskTextureIndex = iIndex; }
	void Set_DissolveTexture(_uint iIndex) { m_iDissolveTextureIndex = iIndex; }
	void Set_MaskValue(_float4* vMaskValue) { m_vMaskValue[0] = vMaskValue[0]; m_vMaskValue[1] = vMaskValue[1]; }
	void Set_LerpScaleStart(_float3 vLerfScale) { m_vLerpScaleStart = vLerfScale; }
	void Set_LerpScaleEnd(_float3 vLerfScale) { m_vLerpScaleEnd = vLerfScale; }

public:
	/*========================생성 관련=============================*/
	virtual HRESULT Save_Effect(ofstream& OFS);
	virtual HRESULT Load_Effect(ifstream& IFS);

public:
	void Reset_Effect();
	void Reset_Position();


protected:
	virtual HRESULT Add_Components() { return S_OK;  }
	void Update_Texture_Movement(_float fTimeDelta);
	void Turn(_float fTimeDelta);
	void Lerp();

protected:
	TYPE m_eType = { EFFECT_END };
	CShader*		m_pShaderCom = { nullptr };
	string			m_strEffectName = {};
	_float			m_fColorMagnification = { 1.f };
	_float4			m_vColor = { 1.f,1.f,1.f,1.f };

	_float			m_fEffectTimeDelta = { INFINITY };				// 이펙트 전체 지속 시간

	_uint			m_iDiffuseTextureIndex = { 999 };
	CTexture*		m_pDiffuseTextureCom = { nullptr };

	_uint			m_iMaskTextureIndex = { 999 };
	CTexture*		m_pMaskTextureCom = { nullptr };

	_uint			m_iDissolveTextureIndex = { 999 };
	CTexture*		m_pDissolveTextureCom = { nullptr };

	_float4			m_vMaskValue[2] = { {0.f,0.f,0.f,0.f}, {0.f,0.f,0.f,1.f} };		// 결과 색이 이 범위 내부인 경우 렌더하지 않음 

	_float			m_fTimeDelta = { 0.f };
	_float2			m_fTextureTimeDelta = { 0.f, 0.f };
	_float2			m_fTextureMovementTimeDelta = { 0.f, 0.f };
	_float			m_fDissolveTimeDelta = { 0.f };									// dissolve time은 effect 타임과 연동

	_float4			m_vTurnDir = { 0.f,0.f,0.f,0.f };
	_float			m_fTurnSpeed = { 1.f };

	_float3			m_vLerpScaleStart = { 1.f,1.f,1.f };
	_float3			m_vLerpScaleEnd = { 1.f,1.f,1.f };									// Lerf가 들어갈때 변화될 스케일 값 

public:
	virtual void Free() override;
};

END
