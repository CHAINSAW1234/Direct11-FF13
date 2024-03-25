#pragma once
#include "Client_Defines.h"
#include "Base.h"
#include "Ability.h"


BEGIN(Client)

class COptima final : public CBase
{
public:
	typedef CAbility::ROLE Optima[3];

private:
	COptima();
	~COptima() = default;

public:
	/*========================Get========================*/
	size_t	Get_Optima_Size() { return m_Optimas.size(); }
	_int	Get_Current_Optima_Num() { return m_iCurrentOptima; }
	Optima* Get_Current_Optima() { return m_Optimas[m_iCurrentOptima].second; }
	wstring Get_Current_Optima_Name() { return m_Optimas[m_iCurrentOptima].first; }
	//Optima* Get_Optima(wstring strOptimaName);
	Optima* Get_Optima(_int iOptimaIndex);	
	wstring Get_Optima_Name(_int iOptimaIndex);

	HRESULT Change_Optima(_int iOptimaIndex);

public:
	// 생성에 관련됨
	HRESULT Add_Optima(wstring strOptimaName);
	HRESULT Delete_Optima(wstring strOptimaName);
	HRESULT Delete_Optima(_int iOptimaIndex);

	HRESULT Change_Role(wstring strOptimaName, _int iOptimaTargetIndex, CAbility::ROLE eRole);
	HRESULT Change_Role(_int iOptimaIndex, _int iOptimaTargetIndex, CAbility::ROLE eRole);

private:
	_int							m_iCurrentOptima = { 0 };
	vector<pair<wstring, Optima*>>	m_Optimas;


public:
	static COptima* Create();
	void Free();

};

END