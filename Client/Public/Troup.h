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
	_bool Tick();
	void Add_Monster(wstring strPrototypeTag, CMonster* pMonster);
	vector<pair<wstring, CMonster*>> Get_Monsters() { return m_Monsters; }
	void Save_Troup(ofstream& OFS);

private:
	HRESULT Add_Collider();
	_bool Check_Collision();
	void Save_Troup_For_Battle();
	HRESULT Load_Troup(ifstream& OFS);
	void Start_Battle();

private:
	CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	CCollider* m_pColliderCom = { nullptr };

	_bool m_isCollision = { false };
	_int	m_iNumMonsters = { 0 };
	vector<pair<wstring, CMonster*>> m_Monsters;

public:
	static CTroup* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	static CTroup* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ifstream& IFS);
	virtual void Free() override;
};

END