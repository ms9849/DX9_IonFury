#include "Bullet_Manager.h"

#include "GameInstance.h"

IMPLEMENT_SINGLETON(CBullet_Manager)

CBullet_Manager::CBullet_Manager() :
    m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CBullet_Manager::Initialize()
{
	if (FAILED(Ready_Bullet()))
		return E_FAIL;

	if (FAILED(Ready_BossGrenade()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBullet_Manager::Ready_Bullet()
{
	list<class CBullet*> Bullets = {};

	auto iter = m_Bullets.find(TEXT("Bullet"));
	if (iter != m_Bullets.end())
		return S_OK;

	/* 총알은 1000개 풀링 */
	for (int i = 0; i < 1000; ++i)
	{
		Bullets.push_back(
			static_cast<CBullet*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC),
				TEXT("Prototype_GameObject_Bullet"), nullptr))
		);
	}

	m_Bullets.emplace(TEXT("Bullet"), Bullets);

	return S_OK;
}

HRESULT CBullet_Manager::Ready_BossGrenade()
{
	//list<class CBullet*> Bullets = {};

	///* 유탄은 30개 풀링 */
	//for (int i = 0; i < 30; ++i)
	//{
	//	Bullets.push_back(
	//		static_cast<CBossGrenade*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::GAMEPLAY),
	//			TEXT("Prototype_GameObject_BossGrenade"), nullptr))
	//	);
	//}

	//m_Bullets.emplace(TEXT("BossGrenade"), Bullets);
	return S_OK;
}

void CBullet_Manager::Create_Bullet(const _wstring& strBulletTag, CBullet::BULLET_DESC BulletDesc, _uint iLayerLevelIndex, const _wstring& strLayerTag)
{
	auto Bullets = m_Bullets.find(strBulletTag);

	if (Bullets == m_Bullets.end())
		return;

	for (auto& pBullet : Bullets->second)
	{
		if (pBullet->isDead() == false && pBullet->Get_Fired() == false)
		{
			pBullet->Set_Desc(BulletDesc);
			pBullet->Set_Fired(true);

			Safe_AddRef(pBullet);
			m_pGameInstance->Add_Clone_ToLayer(pBullet, iLayerLevelIndex, strLayerTag);
			
			break;
		}
		else if(pBullet->isDead() == true && pBullet->Get_Fired() == true)
		{
			pBullet->Set_Dead(false);
			pBullet->Set_Fired(false);
		}
	}

}

void CBullet_Manager::Release_Bullet_Manager()
{
	DestroyInstance();
}

void CBullet_Manager::Free()
{
	__super::Free();

	/*이펙트 리스트 비워주기*/
	for (auto& Bullets : m_Bullets)
	{
		for (auto& pBullet : Bullets.second)
		{
			Safe_Release(pBullet);
		}
		Bullets.second.clear();
	}

	Safe_Release(m_pGameInstance);
}
