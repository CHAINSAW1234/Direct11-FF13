#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT Initialize();
	void Tick(_float fTimeDelta);
	HRESULT Render();

private:
	CGameInstance*			m_pGameInstance = { nullptr };
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

private:
	HRESULT Ready_Default_Setting();
	HRESULT Open_Level(LEVEL eLevelID);
	HRESULT Ready_Prototype_GameObject();
	HRESULT Ready_Prototype_Component_For_Static();

	HRESULT Ready_Gara_Light();
	HRESULT Ready_Gara();

public:	
	static CMainApp* Create();
	virtual void Free() override;
};

END


