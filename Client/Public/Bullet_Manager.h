#pragma once

#include "Client_Defines.h"
#include "Bullet.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CBullet_Manager : public CBase 
{
	DECLARE_SINGLETON(CBullet_Manager);
private:
	CBullet_Manager();
	virtual ~CBullet_Manager() = default;

public:
	HRESULT Initialize();

	HRESULT Ready_Bullet();
	HRESULT Ready_BossGrenade();

	void Create_Bullet(const _wstring& strBulletTag, CBullet::BULLET_DESC BulletDesc, _uint iLayerLevelIndex, const _wstring& strLayerTag);

	void Release_Bullet_Manager();
private:
	map<const _wstring, list<class CBullet*>> m_Bullets;
	class CGameInstance* m_pGameInstance = { nullptr };

public:
	virtual void Free() override;
};

NS_END
