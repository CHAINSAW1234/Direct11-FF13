#pragma once
#include "Client_Defines.h"
#include "PartObject.h"
#include "Model.h"

BEGIN(Engine)
class CShader;
END

BEGIN(Client)

class CBody final : public CPartObject
{
public:
	typedef struct Body_Desc : public PARTOBJECT_DESC
	{
		wstring strModelTag;
	}BODY_DESC;

private:
	CBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody(const CBody& rhs);
	virtual ~CBody() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_uint		Get_CurrentAnimationIndex() { return m_pModelCom->Get_CurrentAnimationIndex(); }
	_float		Get_CurrentTrackPosition() { return m_pModelCom->Get_CurrentTrackPosition(); }
	_bool		Is_Animation_Finished() { return m_pModelCom->isFinished(); }
	void		Change_Animation(_uint iAnimationIndex, _bool isLoop) { m_pModelCom->Set_Animation(iAnimationIndex, isLoop); }
	void		Set_TrackPosition(_float fTrackPosition) { m_pModelCom->Set_TrackPosition(fTrackPosition); }
private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };

private:
	HRESULT Add_Components(const wstring& strModelTag);
	HRESULT Bind_ShaderResources();

public:
	static CBody* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END