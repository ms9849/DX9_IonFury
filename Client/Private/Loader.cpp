#include "Loader.h"

#include "BackGround.h"
#include "Effect.h"
#include "Terrain.h"
#include "Player.h"
#include "Player_RightHand.h"
#include "Player_LeftHand.h"
#include "Sky.h"
#include "Bullet.h"
#include "UIHp.h"
#include "UIText.h"
#include "UIFont.h"
#include "UIBullets.h"
#include "UIInteraction.h"
#include "UIAim.h"
#include "UIArmor.h"
#include "UIItemQueue.h"
#include "UICardKey.h"
#include "ItemArmor.h"
#include "ItemHealpack.h"
#include "ItemPistolBullet.h"
#include "ItemShootGunBullet.h"
#include "ItemCardKey.h"
#include "Soldier.h"
#include "Zombie.h"
#include "Boss.h"
#include "MeleeAttack.h"
#include "DoorLock.h"
#include "Gate.h"
#include "BossGrenade.h"

#include "GameInstance.h"

CLoader::CLoader(LPDIRECT3DDEVICE9 pGraphic_Device)
	: m_pGraphic_Device { pGraphic_Device }
	, m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pGraphic_Device);
}

unsigned int APIENTRY LoadingMain(void* pArg)
{
	CLoader*		pLoader = static_cast<CLoader*>(pArg);

	if (FAILED(pLoader->Loading()))
		return 1;

	return 0;
}

HRESULT CLoader::Initialize(LEVEL eNextLevelID)
{	
	m_eNextLevelID = eNextLevelID;

	/* 세마포어, 뮤텍스, 크리티컬섹션 */

	/* 임계영역(힙, 데이터, 코드)에 접근하기위한 키를 생성한다. */
	InitializeCriticalSection(&m_CriticalSection);

	/* 실제 로딩을 수행하기위한 스레드를 생성한다. */
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingMain, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading()
{
	EnterCriticalSection(&m_CriticalSection);

	HRESULT		hr = {};

	switch (m_eNextLevelID)
	{
	case LEVEL::LOGO:
		hr = Loading_For_Logo();
		break;
	case LEVEL::GAMEPLAY:
		hr = Loading_For_GamePlay();
		break;
	}

	LeaveCriticalSection(&m_CriticalSection);

	if (FAILED(hr))
		return E_FAIL;

	return S_OK;
}

void CLoader::Output()
{
	SetWindowText(g_hWnd, m_strMessage.c_str());
}

HRESULT CLoader::Loading_For_Logo()
{
	m_strMessage = TEXT("텍스쳐를(을) 로딩 중 입니다.");
	/* For.Prototype_Component_Texture_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_Component_Texture_BackGround"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Default%d.jpg"), 2))))
		return E_FAIL;
	
	m_strMessage = TEXT("모델를(을) 로딩 중 입니다.");

	m_strMessage = TEXT("셰이더를(을) 로딩 중 입니다.");
	
	m_strMessage = TEXT("객체원형를(을) 로딩 중 입니다.");
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_GameObject_BackGround"),
		CBackGround::Create(m_pGraphic_Device))))
		return E_FAIL;

	m_strMessage = TEXT("로딩이 완료되었습니다..");

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay()
{
	m_strMessage = TEXT("텍스쳐를(을) 로딩 중 입니다.");
	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Terrain/Tile0.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Sky */
 	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::CUBE, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 5))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_Hp */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_UI_Hp"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/UI/Hp/Hp_%d.png"), 6))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Effect */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_Effect"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Explosion/Explosion%d.png"), 90))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_Aim */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_UI_Aim"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/UI/Aim/Aim_0.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_CardKey */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_UI_CardKey"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/UI/CardKey/CardKey.png"), 1))))
		return E_FAIL;
	
#pragma region 아이템
	/* For.Prototype_Component_Texture_Item_Armor_0 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_Item_Armor_0"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Item/Item_Armor_0.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Item_Healpack_0 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_Item_Healpack_0"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Item/Item_Healpack_0.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Item_CardKey_0 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_Item_CardKey_0"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Item/Item_CardKey_0.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Item_Bullets */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_Item_Bullets"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Item/Item_Bullet_%d.png"), 2))))
		return E_FAIL;
#pragma endregion

#pragma region 상호작용 오브젝트
	/* For.Prototype_Component_Texture_Interaction_DoorLock_Idle */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_Interaction_DoorLock_Idle"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/InteractionObject/DoorLock/DoorLock_0.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Interaction_DoorLock_Unlock */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_Interaction_DoorLock_Unlock"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/InteractionObject/DoorLock/DoorLock_%d.png"), 7))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Interaction_DoorLock_Open */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_Interaction_DoorLock_Open"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/InteractionObject/DoorLock/DoorLock_6.png"), 1))))
		return E_FAIL;
#pragma endregion

#pragma region 맵 오브젝트
	/* For.Prototype_Component_Texture_Map_Gate */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_Map_Gate"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/MapObject/Gate/Gate_0.png"), 1))))
		return E_FAIL;
#pragma endregion

	m_strMessage = TEXT("모델를(을) 로딩 중 입니다.");
	/* For.Prototype_Component_VIBuffer_Terrain */

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pGraphic_Device, 128, 128))))
		return E_FAIL;
	
	/*if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pGraphic_Device, TEXT("../Bin/Resources/Textures/Terrain/Height1.bmp")))))
		return E_FAIL;*/

	/* For.Prototype_GameObject_Bullet */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Bullet"),
		CBullet::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_BossGrenade"),
		CBossGrenade::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_GameObject_Soldier_Bullet */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Soldier_Bullet"),
		CBullet::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_GameObject_Melee_Attack */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Melee_Attack"),
		CMeleeAttack::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_Component_Sight*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Sight"),
		CSight::Create(m_pGraphic_Device))))
		return E_FAIL;

	m_strMessage = TEXT("셰이더를(을) 로딩 중 입니다.");

	m_strMessage = TEXT("객체원형를(을) 로딩 중 입니다.");
	/* For.Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pGraphic_Device))))
		return E_FAIL;

#pragma region 플레이어
	/* For.Prototype_GameObject_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player_RightHand */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Player_RightHand"),
		CPlayer_RightHand::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player_LeftHand */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Player_LeftHand"),
		CPlayer_LeftHand::Create(m_pGraphic_Device))))
		return E_FAIL;
#pragma endregion	

#pragma region 몬스터
	/* For.Prototype_GameObject_Monster_Soldier */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Monster_Soldier"),
		CSoldier::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_GameObject_Monster_Zombie */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Monster_Zombie"),
		CZombie::Create(m_pGraphic_Device))))
		return E_FAIL;
#pragma endregion	

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Monster_Boss"),
		CBoss::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_GameObject_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_GameObject_Effect */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Effect"),
		CEffect::Create(m_pGraphic_Device))))
		return E_FAIL;

#pragma region UI
	/* For.Prototype_GameObject_UIHp */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UIHp"),
		CUIHp::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_GameObject_UIArmor */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UIArmor"),
		CUIArmor::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_GameObject_UIText */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UIText"),
		CUIText::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_GameObject_UIFont */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UIFont"),
		CUIFont::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_GameObject_UIBullets */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UIBullets"),
		CUIBullets::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_GameObject_UIInteraction */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UIInteraction"),
		CUIInteraction::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_GameObject_UIItemQueue */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UIItemQueue"),
		CUIItemQueue::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_GameObject_UIAim */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UIAim"),
		CUIAim::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_GameObject_UICardKey */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UICardKey"),
		CUICardKey::Create(m_pGraphic_Device))))
		return E_FAIL;
#pragma endregion	

#pragma region 아이템
	/* For.Prototype_GameObject_Item_Armor */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Item_Armor"),
		CItemArmor::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_GameObject_Item_Healpack */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Item_Healpack"),
		CItemHealpack::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_GameObject_Item_Pistol_Bullet */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Item_Pistol_Bullet"),
		CItemPistolBullet::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_GameObject_Item_ShootGun_Bullet */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Item_ShootGun_Bullet"),
		CItemShootGunBullet::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_GameObject_Item_CardKey */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Item_CardKey"),
		CItemCardKey::Create(m_pGraphic_Device))))
		return E_FAIL;
#pragma endregion	

#pragma region 상호작용 오브젝트
	/* For.Prototype_GameObject_Interaction_DoorLock */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Interaction_DoorLock"),
		CDoorLock::Create(m_pGraphic_Device))))
		return E_FAIL;
#pragma endregion

#pragma region 맵 오브젝트
	/* For.Prototype_GameObject_Map_Gate */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Map_Gate"),
		CGate::Create(m_pGraphic_Device))))
		return E_FAIL;
#pragma endregion



	m_strMessage = TEXT("로딩이 완료되었습니다..");

	m_isFinished = true;

	return S_OK;
}

CLoader* CLoader::Create(LPDIRECT3DDEVICE9 pGraphic_Device, LEVEL eNextLevelID)
{
	CLoader* pInstance = new CLoader(pGraphic_Device);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX("Failed to Created : CLoader");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLoader::Free()
{
	__super::Free();

	WaitForSingleObject(m_hThread, INFINITE);

	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_CriticalSection);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pGraphic_Device);
}
