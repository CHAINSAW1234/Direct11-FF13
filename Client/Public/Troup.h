#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
class CMonster;

class CTroup final : public CBase
{
private:
	CTroup(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CTroup() = default;

public:
	void Tick();
	void Add_Monster(wstring strPrototypeTag, CMonster* pMonster);
	
	vector<pair<wstring, CMonster*>> Get_Monsters() { return m_Monsters; }

private:
	_bool Check_Collision();
	void Write_Troup();
private:
	CGameInstance* m_pGameInstance = { nullptr };

	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	_int	m_iNumMonsters = { 0 };
	vector<pair<wstring, CMonster*>> m_Monsters;

public:
	static CTroup* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END