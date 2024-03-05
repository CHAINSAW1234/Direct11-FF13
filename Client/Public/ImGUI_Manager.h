#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CCamera;
END

BEGIN(Client)

class CImGUI_Manager final : public CBase
{
private:
	CImGUI_Manager() = default;
	virtual ~CImGUI_Manager() = default;

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void	Tick(_float fDeltaTime);
	void	Late_Tick(_float fDeltaTime);
	HRESULT Render();

public:
	//void IMGUI_Start();
	void EditTransform(_float4x4& matrix);
	void EditFilePath(_Out_ string& FilePath, _Out_ string& FilePathName);
private:
	class CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

public:
	static CImGUI_Manager* Get_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	static unsigned int Destroy_Instance();

private:
	static CImGUI_Manager* m_pInstance;

public:
	virtual void Free() override;

};

END

