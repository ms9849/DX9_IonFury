#include "Player.h"

#include "GameInstance.h"
#include "Bullet.h"
#include "ItemArmor.h"
#include "ItemHealpack.h"
#include "ItemPistolBullet.h"
#include "ItemShootGunBullet.h"
#include "ItemCardKey.h"
#include "ItemShootGun.h"
#include "ItemMachineGun.h"
#include "DoorLock.h"
#include "Lever.h"
#include "Button.h"
#include "MapElevator.h"
#include "MapGate.h"
#include "Player_RightHand.h"
#include "Player_LeftHand.h"
#include "Effect_Manager.h"
#include "Bullet_Manager.h"
#include "MapMachineGunBulletBox.h"
#include "ItemArmorPack.h"
#include "ItemPortableHealPack.h"
#include "MeleeAttack.h"
#include "Zombie.h"
#include "ItemBurger.h"
#include "ItemCoffee.h"

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLandObject{ pGraphic_Device }
{

}

CPlayer::CPlayer(const CPlayer& Prototype)
	: CLandObject(Prototype)
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	m_pObjectDesc = *static_cast<CGameObject::GAMEOBJECT_DESC*>(pArg);

	m_bRideCube = true;
	CLandObject::LANDOBJECT_DESC			Desc{};

	if (m_pObjectDesc.iLayerLevel == ENUM_CLASS(LEVEL::JUSIN))
	{
		Desc.pLandTransform = static_cast<CTransform*>(m_pGameInstance->Get_Component(m_pObjectDesc.iLayerLevel, TEXT("Layer_Map_Objects_Ride"), TEXT("Com_Transform")));
		Desc.pLandVIBuffer = static_cast<CVIBuffer*>(m_pGameInstance->Get_Component(m_pObjectDesc.iLayerLevel, TEXT("Layer_Map_Objects_Ride"), TEXT("Com_VIBuffer")));
	}
	else
	{
		Desc.pLandTransform = static_cast<CTransform*>(m_pGameInstance->Get_Component(m_pObjectDesc.iLayerLevel, TEXT("Layer_BackGround"), TEXT("Com_Transform")));
		Desc.pLandVIBuffer = static_cast<CVIBuffer*>(m_pGameInstance->Get_Component(m_pObjectDesc.iLayerLevel, TEXT("Layer_BackGround"), TEXT("Com_VIBuffer")));
	}

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Weapons()))
		return E_FAIL;

	m_tInfo.iHp = 80;
	m_tInfo.iArmor = 0;
	m_tInfo.iHealpacks = 12;

	auto iter = m_tInfo.Weapons.find(m_tInfo.strWeapon);

	m_tInfo.iBullets = iter->second.iCurrentBullets;
	m_tInfo.iShootBullets = iter->second.iShootBullets;

	//m_pTransformCom->Set_State(STATE::POSITION, _float3(35.f, 1.f, 45.f));
	//m_pTransformCom->Set_State(STATE::POSITION, _float3(30.f, 1.f, 35.f));
	//m_pTransformCom->Set_State(STATE::POSITION, _float3(15.f, 1.f, 95.f)); // 엘베 앞
	//m_pTransformCom->Set_State(STATE::POSITION, _float3(14.f, 26.f, 25.75f)); // 버튼 앞
	//m_pTransformCom->Set_State(STATE::POSITION, _float3(72.f, 26.f, 67.75f)); // 자습실 카드키 앞
	

	//m_pTransformCom->Set_State(STATE::POSITION, _float3(40.f, 26.5f, 197.f)); // 카페 근처

	// 오른손
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_pObjectDesc.iProtoLevel, TEXT("Prototype_GameObject_Player_RightHand"),
		m_pObjectDesc.iLayerLevel, TEXT("Layer_Player_RightHand"), &m_pObjectDesc)))
		return E_FAIL;

	m_pRightHand = dynamic_cast<CPlayer_RightHand*>(m_pGameInstance->Find_GameObject_ToLayer(
		m_pObjectDesc.iLayerLevel, TEXT("Layer_Player_RightHand")));
	Safe_AddRef(m_pRightHand);

	m_pRightHandAnimationCom = dynamic_cast<CAnimation*>(m_pRightHand->Find_Component(TEXT("Com_Animation")));
	Safe_AddRef(m_pRightHandAnimationCom);

	// 왼손
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_pObjectDesc.iProtoLevel, TEXT("Prototype_GameObject_Player_LeftHand"),
		m_pObjectDesc.iLayerLevel, TEXT("Layer_Player_LeftHand"), &m_pObjectDesc)))
		return E_FAIL;

	m_pLeftHand = dynamic_cast<CPlayer_LeftHand*>(m_pGameInstance->Find_GameObject_ToLayer(
		m_pObjectDesc.iLayerLevel, TEXT("Layer_Player_LeftHand")));
	Safe_AddRef(m_pLeftHand);

	m_pLeftHandAnimationCom = dynamic_cast<CAnimation*>(m_pLeftHand->Find_Component(TEXT("Com_Animation")));
	Safe_AddRef(m_pLeftHandAnimationCom);

	return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{
	

	/*치트*/
	if (m_pGameInstance->Key_Down('0'))
	{
		m_pTransformCom->Set_State(STATE::POSITION, _float3(14.f, 26.f, 178.f)); // 아지트 앞
	}
	if (m_pGameInstance->Key_Down('9'))
	{
		m_pTransformCom->Set_State(STATE::POSITION, _float3(70.f, 26.f, 67.75f)); // 자습실 카드키 앞
	}
	if (m_pGameInstance->Key_Down('8'))
	{
		m_pTransformCom->Set_State(STATE::POSITION, _float3(35.f, 6.f, 45.f));
	}
	if (m_pGameInstance->Key_Down('7'))
	{
		m_pTransformCom->Set_State(STATE::POSITION, _float3(40.f, 40.f, 197.f));
	}
	if (m_pGameInstance->Key_Down('6'))
	{
		m_pTransformCom->Set_State(STATE::POSITION, _float3(15.f, 1.f, 95.f)); // 엘베 앞
	}

	/* 점프 로직*/
	if (!m_bJump && m_pGameInstance->Key_Down(VK_SPACE))
	{
		m_bJump = true;
		m_fTime = 0.f;
	}

	__super::Jump(fTimeDelta);

	/* 애니메이션 제어 */
	auto iter = m_tInfo.Weapons.find(m_tInfo.strWeapon);

	if (m_pGameInstance->Key_Down(VK_LCONTROL))
	{
		if (m_tInfo.iHealpacks > 0)
		{
			m_tInfo.iHp += 30;
			if (m_tInfo.iHp >= 100)
				m_tInfo.iHp = 100;
			m_tInfo.iHealpacks -= 1;
			CEffect_Manager::GetInstance()->Create_Effect(TEXT("Effect_Screen_Blur_Heal"), m_pObjectDesc.iLayerLevel, TEXT("Layer_Effect"), { 0.f, 0.f, 0.f });
		}
	}

	if (m_pGameInstance->Key_Down('1'))
	{
		if (m_tInfo.strAction != (TEXT("Reload")) && !m_bWeaponChange && !m_bUseCardKey)
		{
			m_strNextWeapon = TEXT("Pistol");
			if (m_tInfo.strWeapon != m_strNextWeapon)
			{
				m_tInfo.strAction = TEXT("Down");
				m_bWeaponChange = true;
			}
		}
	}
	if (m_pGameInstance->Key_Down('2'))
	{
		if (m_tInfo.strAction != (TEXT("Reload")) && !m_bWeaponChange && !m_bUseCardKey)
		{
			auto iter = m_tInfo.Weapons.find(TEXT("ShootGun"));

			if (iter->second.bUseable)
			{
				m_strNextWeapon = TEXT("ShootGun");
				if (m_tInfo.strWeapon != m_strNextWeapon)
				{
					m_tInfo.strAction = TEXT("Down");
					m_bWeaponChange = true;
				}
			}
		}
	}
	if (m_pGameInstance->Key_Down('3'))
	{
		if (m_tInfo.strAction != (TEXT("Reload")) && !m_bWeaponChange && !m_bUseCardKey)
		{
			auto iter = m_tInfo.Weapons.find(TEXT("MachineGun"));

			if (iter->second.bUseable)
			{
				m_strNextWeapon = TEXT("MachineGun");
				if (m_tInfo.strWeapon != m_strNextWeapon)
				{
					m_tInfo.strAction = TEXT("Down");
					m_bWeaponChange = true;
				}
			}
		}
	}
	if (m_pGameInstance->Key_Down('E'))
	{
		if (m_tInfo.strAction != (TEXT("Reload")) && !m_bWeaponChange && !m_bUseCardKey)
		{
			if (m_bCanUseCardKey && m_bCanOpenDoor)
			{
				m_pGameInstance->PlaySoundOnce(TEXT("keycard_unlock.ogg"), CHANNELID::SOUND_EFFECT, 1.0f);
				m_tInfo.strItem = TEXT("CardKey");
				m_tInfo.strItemAction = TEXT("Up");
				m_tInfo.strAction = TEXT("Down");
				m_bUseCardKey = true;
				m_pDoorLock->Set_Can_Open(m_bUseCardKey);

				if (m_pObjectDesc.iLayerLevel == ENUM_CLASS(LEVEL::GAMEPLAY))
					m_pGameInstance->PlaySoundOnce(TEXT("door_huge_metal_open.ogg"), CHANNELID::SOUND_EFFECT, 1.0f);
				else if (m_pObjectDesc.iLayerLevel == ENUM_CLASS(LEVEL::JUSIN))
					m_pGameInstance->PlaySoundOnce(TEXT("door_scifi02_open.ogg"), CHANNELID::SOUND_EFFECT, 1.0f);
				
				m_bCanUseCardKey = false;
				m_bCanOpenDoor = false;
			}

			if (m_bCanActiveElevator)
			{
				m_pGameInstance->PlaySoundOnce(TEXT("elevator_start.ogg"), CHANNELID::SOUND_EFFECT, 1.0f);
				m_pLever->Set_Active(true);
				m_bActiveElevator = true;
				m_bCanActiveElevator = false;
			}

			if (m_bMachinGunBulletCharge)
			{
				auto iter = m_tInfo.Weapons.find(TEXT("MachineGun"));

				iter->second.iCurrentBullets += 20;

				if (iter->second.iCurrentBullets >= iter->second.iBulletsMax)
					iter->second.iCurrentBullets = iter->second.iBulletsMax;

				if (m_tInfo.strWeapon == TEXT("MachineGun"))
					m_tInfo.iBullets = iter->second.iCurrentBullets;

				Insert_ItemDesc(TEXT("Get MachineGun Bullets [Bullet+20]"));
				m_pGameInstance->PlaySoundOnce(TEXT("Get_Item.ogg"), CHANNELID::SOUND_EFFECT, 0.3f);
				m_bMachinGunBulletCharge = false;
			}

			if (m_bCanActiveDoor)
			{
				m_pButton->Set_Active(true);
				m_bCanActiveDoor = false;
			}
		}
	}

	// 아이템 : 카드키 사용
	// 카드키 사용 중 이동 및 다른 동작 막아야함
	if (m_bUseCardKey)
	{
		if (m_pLeftHandAnimationCom->Check_Animation_Finish(Set_FrameKey(m_tInfo.strItem, TEXT("Up"))))
		{
			m_tInfo.strItemAction = TEXT("Down");
			m_tInfo.strAction = TEXT("Up");
		}
		if (m_pLeftHandAnimationCom->Check_Animation_Finish(Set_FrameKey(m_tInfo.strItem, TEXT("Down"))))
		{
			m_bUseCardKey = false;
		}
	}

	if (m_pLeftHandAnimationCom->Check_Animation_Finish())
	{
		if (!m_bUseCardKey)
			m_tInfo.strItemAction = TEXT("Idle");
	}

	// 무기 교체
	if (m_bWeaponChange)
	{
		if (m_pRightHandAnimationCom->Check_Animation_Finish(Set_FrameKey(m_tInfo.strWeapon, TEXT("Down"))))
		{
			m_tInfo.strWeapon = m_strNextWeapon;
			m_tInfo.strAction = TEXT("Up");
			iter = m_tInfo.Weapons.find(m_tInfo.strWeapon);
			m_tInfo.iBullets = iter->second.iCurrentBullets;
			iter->second.iShootBullets = iter->second.iShootBullets;
			m_tInfo.iShootBullets = iter->second.iShootBullets;
		}

		if (m_pRightHandAnimationCom->Check_Animation_Finish(Set_FrameKey(m_tInfo.strWeapon, TEXT("Up"))))
		{
			m_bWeaponChange = false;
		}
	}

	_float3 vLook = m_pTransformCom->Get_State(STATE::LOOK);
	vLook.y = 0.f;

	_float3 vRight = m_pTransformCom->Get_State(STATE::RIGHT);
	vRight.y = 0.f;

	// 오른손 애니메이션 끝나면 idle로
	if (!m_bWeaponChange && !m_bUseCardKey && m_pRightHandAnimationCom->Check_Animation_Finish())
	{
		/* 장전이 끝났을 때 철컥 소리 나게 */
		if (m_tInfo.strWeapon == TEXT("Pistol") && m_tInfo.strAction == TEXT("Reload"))
		{
			m_pGameInstance->PlaySoundOnce(TEXT("Pistol_Reload_3.ogg"), CHANNELID::SOUND_EFFECT, 0.2f);
		}

		if (m_tInfo.strWeapon == TEXT("MachineGun") && m_tInfo.strAction == TEXT("Shoot"))
			m_tInfo.strAction = TEXT("Shoot");
		else if (m_tInfo.strAction == TEXT("Walk"))
			m_tInfo.strAction = TEXT("Walk");
		else
			m_tInfo.strAction = TEXT("Idle");
	}

#pragma region 플레이어 키 입력

	// 이동
	if (m_pGameInstance->Key_Pressing('W'))
	{
		m_pTransformCom->Go_Direction(vLook, fTimeDelta * m_fSpeed);
		if (m_tInfo.strAction == TEXT("Idle"))
			m_tInfo.strAction = TEXT("Walk");
	}
	if (m_pGameInstance->Key_Pressing('S'))
	{
		m_pTransformCom->Go_Direction(-1.f * vLook, fTimeDelta * m_fSpeed);
		if (m_tInfo.strAction == TEXT("Idle"))
			m_tInfo.strAction = TEXT("Walk");
	}
	if (m_pGameInstance->Key_Pressing('A'))
	{
		m_pTransformCom->Go_Direction(-1.f * vRight, fTimeDelta * m_fSpeed);
		if (m_tInfo.strAction == TEXT("Idle"))
			m_tInfo.strAction = TEXT("Walk");
	}
	if (m_pGameInstance->Key_Pressing('D'))
	{
		m_pTransformCom->Go_Direction(vRight, fTimeDelta * m_fSpeed);
		if (m_tInfo.strAction == TEXT("Idle"))
			m_tInfo.strAction = TEXT("Walk");
	}

	if (m_tInfo.strAction == TEXT("Walk")
		&& !m_pGameInstance->Key_Pressing('W')
		&& !m_pGameInstance->Key_Pressing('A')
		&& !m_pGameInstance->Key_Pressing('S')
		&& !m_pGameInstance->Key_Pressing('D'))
	{
		//m_pRightHandAnimationCom->Clear_Animation(Set_FrameKey(m_tInfo.strWeapon, TEXT("Walk")));
		//if(m_pRightHandAnimationCom->Check_Animation_Finish(Set_FrameKey(m_tInfo.strWeapon, TEXT("Walk"))))
		m_tInfo.strAction = TEXT("Idle");
	}

	if (m_pGameInstance->Key_Down(VK_LSHIFT))
	{
		m_bSpeedUp = !m_bSpeedUp;
		if (m_bSpeedUp)
			m_fSpeed = 2.f;
		else
			m_fSpeed = 1.f;
	}

	// 장전
	if (m_pGameInstance->Key_Down('R'))
	{
		if (!m_bWeaponChange
			&& !m_bUseCardKey
			&& m_tInfo.strAction != TEXT("Reload")
			&& m_tInfo.strWeapon != TEXT("MachineGun"))
		{
			m_tInfo.strAction = TEXT("Reload");
			iter->second.iShootBullets = iter->second.iCanShootBullets;
			m_tInfo.iShootBullets = iter->second.iShootBullets;

			if (m_tInfo.strWeapon == TEXT("Pistol"))
			{
				m_pGameInstance->PlaySoundOnce(TEXT("Pistol_Reload_1.ogg"), CHANNELID::SOUND_EFFECT, 0.5f);
				m_pGameInstance->PlaySoundOnce(TEXT("Pistol_Reload_2.ogg"), CHANNELID::SOUND_EFFECT, 0.5f);
			}

			if (m_tInfo.strWeapon == TEXT("ShootGun"))
			{
				m_pGameInstance->PlaySoundOnce(TEXT("ShotGun_Reload_1.ogg"), CHANNELID::SOUND_EFFECT, 0.5f);
			}
		}
	}

	if (m_pGameInstance->Key_Pressing(VK_LBUTTON))
	{
		// 머신건 총알 발사
		if (m_tInfo.strAction != TEXT("Reload")
			&& !m_bWeaponChange
			&& !m_bUseCardKey
			&& m_tInfo.strWeapon == TEXT("MachineGun"))
		{
			if (iter->second.iCurrentBullets > 0)
			{
				/* 미니건 총알 발사 로직 */
				_float3 vOffset = _float3{ 0.f, 0.f, 0.f };
				vOffset += (m_pTransformCom->Get_State(STATE::RIGHT)) / 50.f;
				vOffset += (m_pTransformCom->Get_State(STATE::LOOK)) / 5.f;
				vOffset.y -= 0.05f;

				_float3 vDir = m_pTransformCom->Get_State(STATE::LOOK);
				vDir.x += m_pGameInstance->Random(-0.05f, 0.05f);
				vDir.y += m_pGameInstance->Random(-0.05f, 0.05f);
				vDir.z += m_pGameInstance->Random(-0.05f, 0.05f);
				_float3 vPos = m_pTransformCom->Get_State(STATE::POSITION);

				if (m_pGameInstance->Key_Pressing('D'))
					vPos -= fTimeDelta * 8.f * m_pTransformCom->Get_State(STATE::RIGHT);
				else if (m_pGameInstance->Key_Pressing('A'))
					vPos += fTimeDelta * 8.f * m_pTransformCom->Get_State(STATE::RIGHT);

				D3DXVec3Normalize(&vDir, &vDir);

				CBullet::BULLET_DESC Desc;
				Desc.vDir = vDir;
				Desc.vPos = vPos + vOffset;
				Desc.vBulletScale = { 0.005f, 0.005f, 0.05f };
				Desc.isPlayerBullet = true;
				Desc.fDuration = 1.5f;

				m_fMachinGunSoundCoolDown += fTimeDelta;
				if (m_fMachinGunSoundCoolDown > 0.05f)
				{
					m_pGameInstance->PlaySoundOnce(TEXT("MachineGun_Fire.ogg"), CHANNELID::SOUND_EFFECT, 0.7f);
					m_fMachinGunSoundCoolDown = 0.f;
				}
				CBullet_Manager::GetInstance()->Create_Bullet(TEXT("Bullet"), Desc, m_pObjectDesc.iLayerLevel, TEXT("Layer_PlayerBullet"));
				/* 애니메이션 제어 */
				iter->second.iCurrentBullets -= 1;
				m_tInfo.iBullets = iter->second.iCurrentBullets;
				m_tInfo.strAction = TEXT("Shoot");
			}
			else
			{
				m_tInfo.strAction = TEXT("Spin");
			}
		}
	}

	if (!m_pGameInstance->Key_Pressing(VK_LBUTTON))
	{
		if (m_tInfo.strWeapon == TEXT("MachineGun")
			&& m_tInfo.strAction == TEXT("Shoot"))
		{
			m_pRightHandAnimationCom->Clear_Animation();
			m_tInfo.strAction = TEXT("Spin");
		}
	}

	if (m_pRightHandAnimationCom->Check_Animation_Finish(TEXT("MachineGun_Spin")))
		m_tInfo.strAction = TEXT("Idle");

	if (m_pGameInstance->Key_Down(VK_LBUTTON))
	{
		if (m_tInfo.strAction != TEXT("Reload")
			&& !m_bWeaponChange
			&& !m_bUseCardKey
			&& iter->second.iCurrentBullets > 0
			&& iter->second.iShootBullets > 0)
		{
			if (m_tInfo.strWeapon == TEXT("Pistol"))
			{
				m_pRightHandAnimationCom->Clear_Animation();

				m_pGameInstance->PlaySoundOnce(TEXT("Pistol_Shoot.ogg"), CHANNELID::SOUND_EFFECT, 0.7f);
				CEffect_Manager::GetInstance()->Create_Effect(TEXT("Effect_Pistol_Fire"), m_pObjectDesc.iLayerLevel, TEXT("Layer_Effect"), { 0.f, 0.f, 0.f });

				_float3 vOffset = _float3{ 0.f, 0.f, 0.f };
				_float3 vDir = Calc_BulletDir(&vOffset);
				_float3 vPos = m_pTransformCom->Get_State(STATE::POSITION);

				if (m_pGameInstance->Key_Pressing('D'))
					vPos -= fTimeDelta * 8.f * m_pTransformCom->Get_State(STATE::RIGHT);
				else if (m_pGameInstance->Key_Pressing('A'))
					vPos += fTimeDelta * 8.f * m_pTransformCom->Get_State(STATE::RIGHT);

				D3DXVec3Normalize(&vDir, &vDir);

				CBullet::BULLET_DESC Desc;
				Desc.vDir = vDir;
				Desc.vPos = vPos + vOffset;
				Desc.vBulletScale = { 0.005f, 0.005f, 0.2f };
				Desc.isPlayerBullet = true;
				Desc.fDuration = 5.f;

				CBullet_Manager::GetInstance()->Create_Bullet(TEXT("Bullet"), Desc, m_pObjectDesc.iLayerLevel, TEXT("Layer_PlayerBullet"));

				iter->second.iCurrentBullets -= 1;
				m_tInfo.iBullets = iter->second.iCurrentBullets;
				iter->second.iShootBullets -= 1;
				m_tInfo.iShootBullets = iter->second.iShootBullets;
				m_tInfo.strAction = TEXT("Shoot");
			}
			else if (m_tInfo.strWeapon == TEXT("ShootGun")
				&& m_tInfo.strAction != TEXT("Shoot"))
			{
				m_pGameInstance->PlaySoundOnce(TEXT("ShotGun_Fire.ogg"), CHANNELID::SOUND_EFFECT, 0.7f);
				CEffect_Manager::GetInstance()->Create_Effect(TEXT("Effect_ShootGun_Fire"), m_pObjectDesc.iLayerLevel, TEXT("Layer_Effect"), { 0.f, 0.f, 0.f });

				CBullet::BULLET_DESC Desc;
				for (_uint i = 0; i < 7; ++i)
				{
					_float3 vOffset = _float3{ 0.f, 0.f, 0.f };
					vOffset += (m_pTransformCom->Get_State(STATE::RIGHT)) / 15.f;
					vOffset += (m_pTransformCom->Get_State(STATE::LOOK)) / 5.f;
					vOffset.y -= 0.05f;

					_float3 vDir = m_pTransformCom->Get_State(STATE::LOOK);
					vDir.x += m_pGameInstance->Random(-0.05f, 0.05f);
					vDir.y += m_pGameInstance->Random(-0.05f, 0.05f);
					vDir.z += m_pGameInstance->Random(-0.05f, 0.05f);
					_float3 vPos = m_pTransformCom->Get_State(STATE::POSITION);

					if (m_pGameInstance->Key_Pressing('D'))
						vPos -= fTimeDelta * 8.f * m_pTransformCom->Get_State(STATE::RIGHT);
					else if (m_pGameInstance->Key_Pressing('A'))
						vPos += fTimeDelta * 8.f * m_pTransformCom->Get_State(STATE::RIGHT);

					D3DXVec3Normalize(&vDir, &vDir);

					Desc.vDir = vDir;
					Desc.vPos = vPos + vOffset;
					Desc.vBulletScale = { 0.005f, 0.005f, 0.2f };
					Desc.isPlayerBullet = true;
					Desc.fDuration = 5.f;

					CBullet_Manager::GetInstance()->Create_Bullet(TEXT("Bullet"), Desc, m_pObjectDesc.iLayerLevel, TEXT("Layer_PlayerBullet"));
				}

				iter->second.iCurrentBullets -= 1;
				m_tInfo.iBullets = iter->second.iCurrentBullets;
				iter->second.iShootBullets -= 1;
				m_tInfo.iShootBullets = iter->second.iShootBullets;
				m_tInfo.strAction = TEXT("Shoot");
			}
		}
	}
#pragma endregion

	m_pRightHand->Set_Player_TimeDelta(fTimeDelta);
	m_pLeftHand->Set_Player_TimeDelta(fTimeDelta);

	m_pRightHand->Set_Current_Animation(Set_FrameKey(m_tInfo.strWeapon, m_tInfo.strAction));
	m_pLeftHand->Set_Current_Animation(Set_FrameKey(m_tInfo.strItem, m_tInfo.strItemAction));

	/* Priority에서 한번 바뀜*/
	SetUp_OnTerrain(m_pTransformCom, 1.0f, &m_bJump);

	m_pRightHand->Set_Player_Transform(m_pTransformCom);
	m_pLeftHand->Set_Player_Transform(m_pTransformCom);

	if (!m_ItemQueues.empty())
		Pop_ItemDesc(fTimeDelta);

	if (m_bCanOpenDoor && m_bCanUseCardKey)
	{
		m_fColTimeStack += fTimeDelta;

		if (m_fColTimeStack >= 1.f)
			m_bCanOpenDoor = false;
	}
	else
	{
		m_fColTimeStack = 0.f;
	}
}

void CPlayer::Update(_float fTimeDelta)
{	
}

void CPlayer::Late_Update(_float fTimeDelta)
{
}

HRESULT CPlayer::Render()
{
	m_pTransformCom->Set_Transform();

	return S_OK;
}

CPlayer::PLAYER_INFO CPlayer::Get_Player_Info()
{
	return m_tInfo;
}

void CPlayer::Set_Player_Info(PLAYER_INFO tPlayerInfo)
{
	m_tInfo = tPlayerInfo;
}

_bool CPlayer::Get_CanUse_CardKey()
{
	return m_bCanUseCardKey;
}

_bool CPlayer::Get_Can_Open_Door()
{
	return m_bCanOpenDoor;
}

_bool CPlayer::Get_Use_CardKey()
{
	return m_bUseCardKey;
}

_bool CPlayer::Get_Active_Elevator()
{
	return m_bActiveElevator;
}

void CPlayer::Set_Active_Elevator(_bool bActive)
{
	m_bActiveElevator = bActive;
}

HRESULT CPlayer::Ready_Components()
{
	/* Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{ 4.f, D3DXToRadian(90.0f) };
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;	

	/* Com_BoxCollider */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_BoxCollider"),
		TEXT("Com_BoxCollider"), reinterpret_cast<CComponent**>(&m_pBoxColliderCom))))
		return E_FAIL;

	/* Com_SphereCollider */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_SphereCollider"),
		TEXT("Com_SphereCollider"), reinterpret_cast<CComponent**>(&m_pSphereColliderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Ready_Weapons()
{
	WEAPON_INFO PistolDesc{};

	PistolDesc.iBulletsMax = 250;
	PistolDesc.iCurrentBullets = 230;
	PistolDesc.iCanShootBullets = 7;
	PistolDesc.iShootBullets = PistolDesc.iCanShootBullets;
	PistolDesc.bUseable = true;

	m_tInfo.Weapons.emplace(TEXT("Pistol"), PistolDesc);

	WEAPON_INFO ShootGunDesc{};

	ShootGunDesc.iBulletsMax = 150;
	ShootGunDesc.iCurrentBullets = 130;
	ShootGunDesc.iCanShootBullets = 2;
	ShootGunDesc.iShootBullets = ShootGunDesc.iCanShootBullets;

	m_tInfo.Weapons.emplace(TEXT("ShootGun"), ShootGunDesc);

	WEAPON_INFO MachineGunDesc{};

	/* 임시로 999로 설정*/
	//MachineGunDesc.iBulletsMax = 100;
	MachineGunDesc.iBulletsMax = 999;
	MachineGunDesc.iCurrentBullets = MachineGunDesc.iBulletsMax;
	MachineGunDesc.iCanShootBullets = MachineGunDesc.iBulletsMax;
	MachineGunDesc.iShootBullets = MachineGunDesc.iCanShootBullets;

	m_tInfo.Weapons.emplace(TEXT("MachineGun"), MachineGunDesc);

	return S_OK;
}

HRESULT CPlayer::Begin_RenderState()
{
	/* 알파 테스트 : 픽셀의 알파를 비교해서 그린다 안그린다를 설정. */
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CPlayer::End_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

_float3 CPlayer::Calc_BulletDir(_float3* vOffset)
{
	_float3 vCollisionPos{0.f, 0.f, 0.f};

	/*
	vLook, vRight 가져와서 보정하기
	*/
	_float3 vPlayerRight = m_pTransformCom->Get_State(STATE::RIGHT);
	_float3 vPlayerLook = m_pTransformCom->Get_State(STATE::LOOK);
	_float3 vPlayerPos = m_pTransformCom->Get_State(STATE::POSITION);

	D3DXVec3Normalize(&vPlayerLook, &vPlayerLook);
	m_pGameInstance->Check_RayCollision(vPlayerPos, vPlayerLook, TEXT("Layer_Map_Objects_AABB"), m_pObjectDesc.iLayerLevel, &vCollisionPos);
	m_pGameInstance->Check_RayCollision(vPlayerPos, vPlayerLook, TEXT("Layer_Map_Objects_AABB_Ride"), m_pObjectDesc.iLayerLevel, &vCollisionPos);
	m_pGameInstance->Check_RayCollision(vPlayerPos, vPlayerLook, TEXT("Layer_Monster"), m_pObjectDesc.iLayerLevel, &vCollisionPos);

	*vOffset = (*D3DXVec3Normalize(&vPlayerRight, &vPlayerRight) / 10.f) + (vPlayerLook) / 5.f;
	vOffset->y -= 0.02f;

	if (vCollisionPos == _float3{ 0.f, 0.f, 0.f })
		return m_pTransformCom->Get_State(STATE::LOOK);
	else
	{
		_float3 vDir = vCollisionPos - (vPlayerPos + *vOffset);
		D3DXVec3Normalize(&vDir, &vDir);
		return vDir;
	}
}

//_float3 vCollisionPos{ 0.f, 0.f, 0.f };
//
////mat view Inv는 카메라의 월드 행렬.
//_float4x4 m_matViewInv;
//m_matViewInv = m_pGameInstance->Get_CameraWorld();
//
///*
//vLook, vRight 가져와서 보정하기
//*/
//_float3 vCamRight = *(_float3*)(&m_matViewInv.m[0][0]);
//_float3 vCamLook = *(_float3*)(&m_matViewInv.m[2][0]);
//_float3 vCamPos = *(_float3*)(&m_matViewInv.m[3][0]);
//
//D3DXVec3Normalize(&vCamLook, &vCamLook);
//m_pGameInstance->Check_RayCollision(vCamPos, vCamLook, TEXT("Layer_Monster"), m_pObjectDesc.iLayerLevel, &vCollisionPos);
//
//*vOffset = (*D3DXVec3Normalize(&vCamRight, &vCamRight) / 10.f) + (vCamLook) / 5.f;
//vOffset->y -= 0.02f;
//
//if (vCollisionPos == _float3{ 0.f, 0.f, 0.f })
//return m_pTransformCom->Get_State(STATE::LOOK);
//else
//{
//	_float3 vDir = vCollisionPos - (vCamPos + *vOffset);
//	D3DXVec3Normalize(&vDir, &vDir);
//	return vDir;
//}

void CPlayer::Insert_ItemDesc(const _wstring strItemText)
{
	if (m_ItemQueues.size() >= 20)
	{
		m_ItemQueues.pop_back();
	}

	ITEM_DESC Desc;
	Desc.fCreateTime = 0.f;
	Desc.strItemText = strItemText;

	m_ItemQueues.push_front(Desc);
}

void CPlayer::Pop_ItemDesc(_float fTimeDelta)
{
	for (auto& item : m_ItemQueues)
		item.fCreateTime += fTimeDelta;

	if(!m_ItemQueues.empty() && m_ItemQueues.back().fCreateTime > 2.f)
	{
		m_ItemQueues.pop_back();
	}
}

_wstring CPlayer::Set_FrameKey(_wstring strDst, _wstring strSrc)
{
	_tchar strResult[256];
	wsprintf(strResult, TEXT("%s_%s"), strDst.c_str(), strSrc.c_str());
	return strResult;
}

void CPlayer::OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta)
{
	if (eColType == COLLISION::SPHERE)
	{
		if (dynamic_cast<CMeleeAttack*>(pDst))
		{
			if (m_tInfo.iHp >= 4)
			{
				m_tInfo.iHp -= 3.f;
				m_pGameInstance->PlaySoundOnce(TEXT("hurt02.ogg"), CHANNELID::SOUND_EFFECT, 0.4f);
				CEffect_Manager::GetInstance()->Create_Effect(TEXT("Effect_Screen_Blur_Hit"), m_pObjectDesc.iLayerLevel, TEXT("Layer_Effect"), { 0.f, 0.f, 0.f });
			}
		}
		if (dynamic_cast<CItemArmor*>(pDst))
		{
			m_tInfo.iArmor = 100;
			m_tInfo.bArmor = true;
			Insert_ItemDesc(TEXT("Get Armor [Armor+100]"));
		}
		if (dynamic_cast<CItemArmorPack*>(pDst))
		{
			m_tInfo.iArmor += 1;
			if (m_tInfo.iArmor >= 100)
				m_tInfo.iArmor = 100;

			CEffect_Manager::GetInstance()->Create_Effect(TEXT("Effect_Screen_Blur_Blue"), m_pObjectDesc.iLayerLevel, TEXT("Layer_Effect"), { 0.f, 0.f, 0.f });
			Insert_ItemDesc(TEXT("Get Armor Fragment [Armor+1]"));
		}
		if (dynamic_cast<CItemHealpack*>(pDst))
		{
			m_tInfo.iHp += 10;
			if (m_tInfo.iHp >= 100)
				m_tInfo.iHp = 100;
			CEffect_Manager::GetInstance()->Create_Effect(TEXT("Effect_Screen_Blur_Heal"), m_pObjectDesc.iLayerLevel, TEXT("Layer_Effect"), { 0.f, 0.f, 0.f });
			Insert_ItemDesc(TEXT("Get Healpack [HP+10]"));
		}
		if (dynamic_cast<CItemBurger*>(pDst))
		{
			m_tInfo.iHp += 5;
			if (m_tInfo.iHp >= 100)
				m_tInfo.iHp = 100;

			Insert_ItemDesc(TEXT("Eat Burger [HP+5]"));
		}
		if (dynamic_cast<CItemCoffee*>(pDst))
		{
			m_tInfo.iHp += 5;
			if (m_tInfo.iHp >= 100)
				m_tInfo.iHp = 100;

			Insert_ItemDesc(TEXT("Drink Coffee [HP+5]"));
		}
		if (dynamic_cast<CItemPortableHealPack*>(pDst))
		{
			m_tInfo.iHealpacks += 1;
		}
		if (dynamic_cast<CItemPistolBullet*>(pDst))
		{
			auto iter = m_tInfo.Weapons.find(TEXT("Pistol"));
			iter->second.iCurrentBullets += 10;

			if (iter->second.iCurrentBullets >= iter->second.iBulletsMax)
				iter->second.iCurrentBullets = iter->second.iBulletsMax;

			if (m_tInfo.strWeapon == TEXT("Pistol"))
				m_tInfo.iBullets = iter->second.iCurrentBullets;

			Insert_ItemDesc(TEXT("Get Pistol Bullets [Bullet+10]"));
		}
		if (dynamic_cast<CItemShootGunBullet*>(pDst))
		{
			auto iter = m_tInfo.Weapons.find(TEXT("ShootGun"));

			iter->second.iCurrentBullets += 10;

			if (iter->second.iCurrentBullets >= iter->second.iBulletsMax)
				iter->second.iCurrentBullets = iter->second.iBulletsMax;

			if (m_tInfo.strWeapon == TEXT("ShootGun"))
				m_tInfo.iBullets = iter->second.iCurrentBullets;

			Insert_ItemDesc(TEXT("Get ShotGun Bullets [Bullet+10]"));
		}
		if (dynamic_cast<CItemShootGun*>(pDst))
		{
			auto iter = m_tInfo.Weapons.find(TEXT("ShootGun"));
			iter->second.bUseable = true;

			// 문 열리는 사운드 넣어주세요

			dynamic_cast<CMapGate*>(
				m_pGameInstance->Get_GameObject_By_ID(
					m_pObjectDesc.iLayerLevel,
					TEXT("Layer_Map_Objects_Gate"),
					2))->Set_Dead(true);

			dynamic_cast<CMapGate*>(
				m_pGameInstance->Get_GameObject_By_ID(
					m_pObjectDesc.iLayerLevel,
					TEXT("Layer_Map_Objects_Gate"),
					3))->Set_Dead(true);

			for (size_t i = 12; i < 52; ++i)
			{
				CZombie* pZombie = dynamic_cast<CZombie*>(m_pGameInstance->Get_GameObject_By_ID(
					m_pObjectDesc.iLayerLevel,
					TEXT("Layer_Monster"), i));

				if (pZombie != nullptr)
					pZombie->Set_TargetMove(true);

				/*dynamic_cast<CZombie*>(
					m_pGameInstance->Get_GameObject_By_ID(
						m_pObjectDesc.iLayerLevel,
						TEXT("Layer_Monster"), i))->Set_TargetMove(true);*/
			}

			Insert_ItemDesc(TEXT("Get ShotGun"));

		}
		if (dynamic_cast<CItemMachineGun*>(pDst))
		{
			auto iter = m_tInfo.Weapons.find(TEXT("MachineGun"));
			iter->second.bUseable = true;

			Insert_ItemDesc(TEXT("Get MachineGun"));
		}
		if (dynamic_cast<CItemCardKey*>(pDst))
		{
			Insert_ItemDesc(TEXT("Get CardKey"));
			m_bCanUseCardKey = true;
		}
		if (dynamic_cast<CDoorLock*>(pDst) && m_bCanUseCardKey)
		{
			if (m_pObjectDesc.iLayerLevel == ENUM_CLASS(LEVEL::GAMEPLAY))
			{
				if (pDst->Get_Desc().iObjectID == 0)
				{
					m_pDoorLock = dynamic_cast<CDoorLock*>(pDst);
					m_pDoorLock->Set_TargetID(0);
				}
				if (pDst->Get_Desc().iObjectID == 1)
				{
					m_pDoorLock = dynamic_cast<CDoorLock*>(pDst);
					m_pDoorLock->Set_TargetID(1);
				}
			}
			else if (m_pObjectDesc.iLayerLevel == ENUM_CLASS(LEVEL::JUSIN))
			{
				if (pDst->Get_Desc().iObjectID == 0)
				{
					m_pDoorLock = dynamic_cast<CDoorLock*>(pDst);
					m_pDoorLock->Set_TargetID(1);
				}
			}

			m_bCanOpenDoor = true;
			m_fColTimeStack = 0.f;
		}
		if (dynamic_cast<CLever*>(pDst))
		{
			if (pDst->Get_Desc().iObjectID == 2)
			{
				m_pLever = dynamic_cast<CLever*>(pDst);
				m_pLever->Set_TargetID(11);
			}

			m_bCanActiveElevator = true;
		}
		if (dynamic_cast<CButton*>(pDst))
		{
			if (pDst->Get_Desc().iObjectID == 1)
			{
				m_pButton = dynamic_cast<CButton*>(pDst);
				m_pButton->Set_TargetID(0);
			}

			m_bCanActiveDoor = true;
		}
	}

	if (eColType == COLLISION::BOX)
	{
		if (dynamic_cast<CMapMachineGunBulletBox*>(pDst))
		{
			auto iter = m_tInfo.Weapons.find(TEXT("MachineGun"));

			if(iter->second.iCurrentBullets < iter->second.iBulletsMax)
				m_bMachinGunBulletCharge = true;
		}
	}
}

void CPlayer::OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta, CComponent* pCollider, const _float3& vPos)
{
	if (eColType == COLLISION::RAY)
	{
		if (m_tInfo.iHp >= 3)
		{
			if (m_tInfo.iArmor >= 2)
			{
				m_tInfo.iArmor -= 2;
			}
			else if (m_tInfo.iArmor < 2)
			{
				_int iDamage = 2 - m_tInfo.iArmor;
				m_tInfo.iArmor = 0.f;
				m_tInfo.iHp -= iDamage;
			}
			else
				m_tInfo.iHp -= 2;
			m_pGameInstance->PlaySoundOnce(TEXT("hurt02.ogg"), CHANNELID::SOUND_EFFECT, 0.4f);
			CEffect_Manager::GetInstance()->Create_Effect(TEXT("Effect_Screen_Blur_Hit"), m_pObjectDesc.iLayerLevel, TEXT("Layer_Effect"), { 0.f, 0.f, 0.f });
		}
	}
}

const COLLISION_DESC& CPlayer::Get_CollisionDesc(COLLISION eColType)
{
	COLLISION_DESC Desc;
	Desc.pTransform = m_pTransformCom;

	if (eColType == COLLISION::SPHERE)
		Desc.pCollider = m_pSphereColliderCom;
	/* 플레이어는 헤드샷 판정 X */
	else if (eColType == COLLISION::BOX)
		Desc.pCollider = m_pBoxColliderCom;

	return Desc;
}

_wstring CPlayer::Get_ItemText(size_t iIndex)
{
	return m_ItemQueues[iIndex].strItemText;
}

size_t CPlayer::Get_ItemQueue_Length()
{
	return m_ItemQueues.size();
}

CPlayer* CPlayer::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CPlayer* pInstance = new CPlayer(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pBoxColliderCom);
	Safe_Release(m_pLeftHand);
	Safe_Release(m_pLeftHandAnimationCom);
	Safe_Release(m_pRightHand);
	Safe_Release(m_pRightHandAnimationCom);
	Safe_Release(m_pSphereColliderCom);
}
