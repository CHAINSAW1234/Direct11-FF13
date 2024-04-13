#pragma once
#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)
// �� ������ ���� �Ľ��ϱ����� �����ϴ� ������ ���� ���� �÷��̿� ������ �ʽ��ϴ�
class CLevel_Parsing final : public CLevel
{
private:
	CLevel_Parsing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Parsing() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	vector<CModel*> m_Models;

private:

#ifdef _DEBUG
	HRESULT Ready_Models();
	HRESULT Parse_Models();
	HRESULT Test_Parsed_Models();
#endif

public:
	static CLevel_Parsing* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};


END