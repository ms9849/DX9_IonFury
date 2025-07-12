#pragma once

#include "Client_Defines.h"
#include "Level.h"
#include "Player.h"

/* 2. 자원을 로드하는 동안, 화면에 로딩 상태를 표현해준다. */

NS_BEGIN(Client)

class CLevel_Loading final : public CLevel
{
private:
	CLevel_Loading(LPDIRECT3DDEVICE9 pGraphic_Device, LEVEL eLevelID);
	virtual ~CLevel_Loading() = default;

public:
	virtual HRESULT Initialize(LEVEL eNextLevelID, void* pArg = nullptr);
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:

private:
	LEVEL				m_eNextLevelID = { LEVEL::END };
	class CLoader*		m_pLoader = { nullptr };

	CPlayer::PLAYER_INFO m_tPlayerInfo{};

private:
	HRESULT Ready_Layer_BackGround();


public:
	static CLevel_Loading* Create(LPDIRECT3DDEVICE9 pGraphic_Device, LEVEL eLevelID, LEVEL eNextLevelID, void* pArg = nullptr);
	virtual void Free() override;
};

NS_END