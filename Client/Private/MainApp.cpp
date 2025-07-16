#include "MainApp.h"

#include "GameInstance.h"
#include "Level_Loading.h"
#include "Camera.h"
#include "Player.h"
#include "Player_RightHand.h"
#include "Player_LeftHand.h"
#include "Effect_Manager.h"
#include "Particle_Manager.h"
#include "Bullet_Manager.h"
#include "Snow.h"
#include "Blood.h"
#include "YellowDust.h"
#include "Effect.h"
#include "Effect_Pistol_Fire.h"
#include "Effect_ShootGun_Fire.h"
#include "Effect_Screen_Blur.h"
#include "Bullet_Wound.h"
#include "Bullet.h"
#include "EventBox.h"
#include "Background.h"
#include "UIText.h"
#include "UIFont.h"
#include "UIPressEnter.h"

/*****************************
대재훈의 은총 이 얼마나 관대한가
******************************/

Client::CMainApp::CMainApp()
	: m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT Client::CMainApp::Initialize()
{
	ENGINE_DESC				EngineDesc{};
	EngineDesc.hWnd = g_hWnd;
	EngineDesc.eWindowMode = WINMODE::WIN;
	EngineDesc.iWinSizeX = g_iWinSizeX;
	EngineDesc.iWinSizeY = g_iWinSizeY;
	EngineDesc.iNumLevels = ENUM_CLASS(LEVEL::END);

	if (FAILED(m_pGameInstance->Initialize_Engine(EngineDesc, &m_pGraphic_Device)))
		return E_FAIL;

	if (FAILED(Ready_Default_Setting()))
		return E_FAIL;

	if (FAILED(Ready_Prototypes()))
		return E_FAIL;

	if (FAILED(Start_Level(LEVEL::LOGO)))
		return E_FAIL;	

	return S_OK;
}

void Client::CMainApp::Update(_float fTimeDelta)
{
	m_pGameInstance->Update_Engine(fTimeDelta);
}

HRESULT Client::CMainApp::Render()
{
	m_pGameInstance->Render_Begin(D3DXCOLOR(0.f, 0.f, 1.f, 1.f));

	m_pGameInstance->Draw();

	m_pGameInstance->Render_End();

	return S_OK;
}

HRESULT CMainApp::Ready_Default_Setting()
{
	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_MIRROR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_MIRROR);

	return S_OK;
}

HRESULT CMainApp::Start_Level(LEVEL eLevelID)
{
	if (FAILED(m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pGraphic_Device, LEVEL::LOADING, eLevelID))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototypes()
{
	/* For.Prototype_Component_BoxCollider*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_BoxCollider"),
		CBoxCollider::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_Component_SphereCollider*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_SphereCollider"),
		CSphereCollider::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_Component_Transform*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		CTransform::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_GameObject_Camera */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Camera"),
		CCamera::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_GameObject_EventBox */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_EventBox"),
		CEventBox::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_Component_Animation */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Animation"),
		CAnimation::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_Component_Sight */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Sight"),
		CSight::Create(m_pGraphic_Device))))
		return E_FAIL;

#pragma region Logo, Loading, Ending 화면 공통 객체

	/* For.Prototype_GameObject_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_BackGround"),
		CBackGround::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_GameObject_UIText */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UIText"),
		CUIText::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_GameObject_UIFont */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UIFont"),
		CUIFont::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_GameObject_UIPressEnter */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UIPressEnter"),
		CUIPressEnter::Create(m_pGraphic_Device))))
		return E_FAIL;

#pragma endregion

#pragma region 백그라운드 이미지
	/* For.Prototype_Component_Texture_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_BackGround"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Backgrounds/Background_%d.png"), 3))))
		return E_FAIL;
#pragma endregion

#pragma region 폰트 텍스처
	/* For.Prototype_Component_Fonts_Default */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Fonts_Default"),
		CFonts::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Fonts/Default/Text_%d.png"), DefaultFontSet, 38))))
		return E_FAIL;

	/* For.Prototype_Component_Fonts_Gray */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Fonts_Gray"),
		CFonts::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Fonts/Gray/Text_%d.png"), GrayFontSet, 37))))
		return E_FAIL;

	/* For.Prototype_Component_Fonts_Primary */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Fonts_Primary"),
		CFonts::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Fonts/Primary/Text_%d.png"), PrimaryFontSet, 94))))
		return E_FAIL;

#pragma endregion

#pragma region 플레이어 애니메이션 텍스처
	// 권총
	/* For.Prototype_Component_Texture_Player_Pistol_Idle */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Player_Pistol_Idle"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Player/Pistol/Idle_%d.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Player_Pistol_Walk */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Player_Pistol_Walk"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Player/Pistol/Idle_%d.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Player_Pistol_Down */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Player_Pistol_Down"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Player/Pistol/Idle_%d.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Player_Pistol_Up */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Player_Pistol_Up"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Player/Pistol/Idle_%d.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Player_Pistol_Reload */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Player_Pistol_Reload"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Player/Pistol/Reload_%d.png"), 19))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Player_Pistol_Shoot */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Player_Pistol_Shoot"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Player/Pistol/Shoot_%d.png"), 5))))
		return E_FAIL;

	// 샷건
	/* For.Prototype_Component_Texture_Player_ShootGun_Idle */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Player_ShootGun_Idle"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Player/ShootGun/Idle_%d.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Player_ShootGun_Walk */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Player_ShootGun_Walk"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Player/ShootGun/Idle_%d.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Player_ShootGun_Down */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Player_ShootGun_Down"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Player/ShootGun/Idle_%d.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Player_ShootGun_Up */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Player_ShootGun_Up"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Player/ShootGun/Idle_%d.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Player_ShootGun_Reload */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Player_ShootGun_Reload"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Player/ShootGun/Reload_%d.png"), 29))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Player_ShootGun_Shoot */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Player_ShootGun_Shoot"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Player/ShootGun/Shoot_%d.png"), 11))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Player_MachineGun_Idle */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Player_MachineGun_Idle"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Player/MachineGun/Idle_%d.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Player_MachineGun_Walk */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Player_MachineGun_Walk"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Player/MachineGun/Idle_%d.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Player_MachineGun_Down */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Player_MachineGun_Down"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Player/MachineGun/Idle_%d.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Player_MachineGun_Up */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Player_MachineGun_Up"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Player/MachineGun/Idle_%d.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Player_MachineGun_Shoot */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Player_MachineGun_Shoot"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Player/MachineGun/Shoot_%d.png"), 7))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Player_MachineGun_Spin */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Player_MachineGun_Spin"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Player/MachineGun/Spin_%d.png"), 12))))
		return E_FAIL;
#pragma endregion

#pragma region 플레이어 아이템 사용 텍스처
	/* For.Prototype_Component_Texture_Player_LeftHand_CardKey_Idle */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Player_LeftHand_CardKey_Idle"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Player/CardKey/Idle_0.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Player_LeftHand_CardKey_Idle */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Player_LeftHand_CardKey_Up"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Player/CardKey/Idle_0.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Player_LeftHand_CardKey_Idle */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Player_LeftHand_CardKey_Down"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Player/CardKey/Idle_0.png"), 1))))
		return E_FAIL;
#pragma endregion

//#pragma region 보스 UI 텍스처
//	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Boss_HpBar"),
//		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Boss/HpBar_%d.png"), 1))))
//		return E_FAIL;
//
//	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Boss_HpFill"),
//		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Boss/HpFill_%d.png"), 1))))
//		return E_FAIL;
//
//	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Boss_Name"),
//		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Boss/Name_%d.png"), 1))))
//		return E_FAIL;
//#pragma endregion

#pragma region 몬스터 애니메이션 텍스처
	/* For.Prototype_Component_Texture_Monster */
	/* Melee Monster */
	/* Zombie Monster */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Zombie_Front"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Zombie/Front_%d.png"), 6))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Zombie_Back"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Zombie/Back_%d.png"), 8))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Zombie_Left"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Zombie/Left_%d.png"), 9))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Zombie_Right"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Zombie/Right_%d.png"), 9))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Zombie_Direction_NE"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Zombie/Direction_NE_%d.png"), 5))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Zombie_Direction_NW"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Zombie/Direction_NW_%d.png"), 5))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Zombie_Direction_SE"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Zombie/Direction_SE_%d.png"), 3))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Zombie_Direction_SW"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Zombie/Direction_SW_%d.png"), 3))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Zombie_Attack"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Zombie/Attack_%d.png"), 4))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Zombie_Die_Default"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Zombie/Die_Default_%d.png"), 6))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Zombie_Die_Explosion"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Zombie/Die_Explosion_%d.png"), 9))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Zombie_Die_Idle"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Zombie/Die_Idle_%d.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Zombie_Die_HeadShot"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Zombie/Die_HeadShot_%d.png"), 7))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Zombie_Die_HeadShot_Idle"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Zombie/Die_HeadShot_Idle_%d.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Zombie_Awake"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Zombie/Awake_%d.png"), 7))))
		return E_FAIL;

	/* Spider Monster */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Spider_Front"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Spider/Front_%d.png"), 6))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Spider_Back"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Spider/Back_%d.png"), 6))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Spider_Left"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Spider/Left_%d.png"), 6))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Spider_Right"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Spider/Right_%d.png"), 6))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Spider_Direction_NE"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Spider/Direction_NE_%d.png"), 6))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Spider_Direction_NW"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Spider/Direction_NW_%d.png"), 6))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Spider_Direction_SE"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Spider/Direction_SE_%d.png"), 6))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Spider_Direction_SW"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Spider/Direction_SW_%d.png"), 6))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Spider_Attack"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Spider/Attack_%d.png"), 2))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Spider_Die_Default"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Spider/Die_Default_%d.png"), 5))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Spider_Die_Idle"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Spider/Die_Idle_%d.png"), 1))))
		return E_FAIL;

	/*if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Zombie_Die_HeadShot"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Zombie/Die_HeadShot_%d.png"), 6))))
		return E_FAIL;*/

	/* For.Prototype_Component_Texture_Monster */
	/* Range Monster */
	/*Soldier*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Soldier_Front"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Soldier/Front_%d.png"), 5))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Soldier_Back"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Soldier/Back_%d.png"), 4))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Soldier_Left"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Soldier/Left_%d.png"), 5))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Soldier_Right"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Soldier/Right_%d.png"), 5))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Soldier_Direction_NE"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Soldier/Direction_NE_%d.png"), 4))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Soldier_Direction_NW"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Soldier/Direction_NW_%d.png"), 4))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Soldier_Direction_SE"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Soldier/Direction_SE_%d.png"), 4))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Soldier_Direction_SW"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Soldier/Direction_SW_%d.png"), 4))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Soldier_Attack_Front"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Soldier/Attack_Front_%d.png"), 3))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Soldier_Attack_SE"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Soldier/Attack_SE_%d.png"), 3))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Soldier_Attack_SW"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Soldier/Attack_SW_%d.png"), 3))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Soldier_Die_Default"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Soldier/Die_Default_%d.png"), 6))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Soldier_Die_Explosion"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Soldier/Die_Explosion_%d.png"), 9))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Soldier_Die_HeadShot"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Soldier/Die_HeadShot_%d.png"), 6))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Soldier_Die_Idle"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Soldier/Die_Idle_%d.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Soldier_Die_HeadShot_Idle"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Soldier/Die_HeadShot_Idle_%d.png"), 1))))
		return E_FAIL;

	/*EliteSoldier*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_EliteSoldier_Front"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/EliteSoldier/Front_%d.png"), 5))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_EliteSoldier_Back"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/EliteSoldier/Back_%d.png"), 4))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_EliteSoldier_Left"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/EliteSoldier/Left_%d.png"), 4))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_EliteSoldier_Right"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/EliteSoldier/Right_%d.png"), 4))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_EliteSoldier_Direction_NE"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/EliteSoldier/Direction_NE_%d.png"), 4))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_EliteSoldier_Direction_NW"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/EliteSoldier/Direction_NW_%d.png"), 4))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_EliteSoldier_Direction_SE"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/EliteSoldier/Direction_SE_%d.png"), 4))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_EliteSoldier_Direction_SW"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/EliteSoldier/Direction_SW_%d.png"), 4))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_EliteSoldier_Attack_Front"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/EliteSoldier/Attack_Front_%d.png"), 3))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_EliteSoldier_Attack_Seat"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/EliteSoldier/Attack_Seat_%d.png"), 3))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_EliteSoldier_Die_Default"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/EliteSoldier/Die_Default_%d.png"), 6))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_EliteSoldier_Die_Idle"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/EliteSoldier/Die_Idle_%d.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_EliteSoldier_Die_HeadShot"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/EliteSoldier/Die_HeadShot_%d.png"), 7))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_EliteSoldier_Die_HeadShot_Idle"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/EliteSoldier/Die_HeadShot_Idle_%d.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Monster */
	/* Boss Monster */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Boss_Front"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Boss/Front_%d.png"), 1))))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Boss_Back"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Boss/Back_%d.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Boss_Left"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Boss/Left_%d.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Boss_Right"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Boss/Right_%d.png"), 1))))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Boss_Direction_NE"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Boss/Direction_NE_%d.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Boss_Direction_NW"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Boss/Direction_NW_%d.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Boss_Direction_SE"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Boss/Direction_SE_%d.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Boss_Direction_SW"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Boss/Direction_SW_%d.png"), 1))))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Boss_Front_Leg"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Boss/Front_Leg_%d.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Boss_Back_Leg"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Boss/Back_Leg_%d.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Boss_Left_Leg"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Boss/Left_Leg_%d.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Boss_Right_Leg"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Boss/Right_Leg_%d.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Boss_Leg_Direction_NE"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Boss/Leg_Direction_NE_%d.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Boss_Leg_Direction_NW"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Boss/Leg_Direction_NW_%d.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Boss_Leg_Direction_SE"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Boss/Leg_Direction_SE_%d.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Boss_Leg_Direction_SW"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Boss/Leg_Direction_SW_%d.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Boss_Attack_Front"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Boss/Boss_Attack_Front_%d.png"), 4))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Boss_Attack_SE"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Boss/Boss_Attack_SE_%d.png"), 4))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Boss_Attack_SW"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Boss/Boss_Attack_SW_%d.png"), 4))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Boss_Attack_NE"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Boss/Boss_Attack_NE_%d.png"), 4))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Boss_Attack_NW"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Boss/Boss_Attack_NW_%d.png"), 4))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Boss_Attack_Left"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Boss/Boss_Attack_Left_%d.png"), 2))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Boss_Attack_Right"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Boss/Boss_Attack_Right_%d.png"), 2))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Boss_Attack_Back"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Boss/Boss_Attack_Back_%d.png"), 4))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Boss_Die"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Boss/Boss_Die_%d.png"), 32))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Boss_Front_LeftLeg"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Boss/Front_LeftLeg_%d.png"), 4))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Boss_Front_RightLeg"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Boss/Front_RightLeg_%d.png"), 4))))
		return E_FAIL;
	// 여기부터
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Boss_Back_LeftLeg"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Boss/Back_LeftLeg_%d.png"), 4))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Boss_Back_RightLeg"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Boss/Back_RightLeg_%d.png"), 4))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Boss_Left_LeftLeg"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Boss/Left_LeftLeg_%d.png"), 4))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Boss_Left_RightLeg"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Boss/Left_RightLeg_%d.png"), 4))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Boss_Right_LeftLeg"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Boss/Right_LeftLeg_%d.png"), 4))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Boss_Right_RightLeg"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Boss/Right_RightLeg_%d.png"), 4))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Boss_Direction_NE_LeftLeg"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Boss/Leg_LeftDirection_NE_%d.png"), 4))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Boss_Direction_NE_RightLeg"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Boss/Leg_RightDirection_NE_%d.png"), 4))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Boss_Direction_NW_LeftLeg"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Boss/Leg_LeftDirection_NW_%d.png"), 4))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Boss_Direction_NW_RightLeg"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Boss/Leg_RightDirection_NW_%d.png"), 4))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Boss_Direction_SE_LeftLeg"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Boss/Leg_LeftDirection_SE_%d.png"), 4))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Boss_Direction_SE_RightLeg"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Boss/Leg_RightDirection_SE_%d.png"), 4))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Boss_Direction_SW_LeftLeg"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Boss/Leg_LeftDirection_SW_%d.png"), 4))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Boss_Direction_SW_RightLeg"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Boss/Leg_RightDirection_SW_%d.png"), 4))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Boss_FlyLeft"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Boss/FlyLeft_%d.png"), 20))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Boss_FlyRight"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Boss/FlyRight_%d.png"), 20))))
		return E_FAIL;

#pragma endregion

#pragma region 총알 등 기타 텍스쳐
	/* For.Prototype_Component_Texture_Bullet */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Bullet"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::CUBE, TEXT("../Bin/Resources/Textures/Bullet/Bullet%d.dds"), 3))))
		return E_FAIL;

	/*For Prototype_Component_Texture_Grenade_Bullet*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Grenade"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Bullet/Grenade/Grenade_%d.png"), 1))))
		return E_FAIL;

	/*For Prototype_Component_Texture_Grenade_Explosion*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Grenade_Explosion"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Bullet/Grenade/Grenade_Explosion_%d.png"), 31))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Bullet"),
		CBullet::Create(m_pGraphic_Device))))
		return E_FAIL;
#pragma endregion

#pragma region 플레이어
	/* For.Prototype_GameObject_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player_RightHand */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Player_RightHand"),
		CPlayer_RightHand::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player_LeftHand */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Player_LeftHand"),
		CPlayer_LeftHand::Create(m_pGraphic_Device))))
		return E_FAIL;
#pragma endregion	

#pragma region 이펙트
	/* For.Prototype_Component_Texture_Effect_Pistol_Fire */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Effect_Pistol_Fire"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Effect/Pistol_Fire/Pistol_Fire_%d.png"), 3))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Effect_ShootGun_Fire */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Effect_ShootGun_Fire"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Effect/ShootGun_Fire/ShootGun_Fire_%d.png"), 4))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Effect */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Effect"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Explosion/Explosion%d.png"), 90))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Effect_Boss_Die */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Effect_Boss_Die"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Effect/Boss_Die/Boss_Die_%d.png"), 32))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Effect_Grenade_Explosion */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Effect_Grenade_Explosion"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Effect/Grenade_Explosion/Grenade_Explosion_%d.png"), 31))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Effect_Bullet_Wound */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Effect_Bullet_Wound"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Effect/Bullet_Wound/Bullet_Wound_%d.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Effect_Screen_Blur */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Effect_Screen_Blur"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Effect/Screen_Blur/Screen_Blur_%d.png"), 3))))
		return E_FAIL;
	
	/* For.Prototype_GameObject_Effect_Screen_Blur */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Effect_Screen_Blur"),
		CEffect_Screen_Blur::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_GameObject_Effect_Pistol_Fire */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Effect_Pistol_Fire"),
		CEffect_Pistol_Fire::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_GameObject_Effect_ShootGun_Fire */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Effect_ShootGun_Fire"),
		CEffect_ShootGun_Fire::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_GameObject_Effect */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Effect"),
		CEffect::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_GameObject_Bullet_Wound*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Effect_Bullet_Wound"),
		CBullet_Wound::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Snow */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Snow"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Snow/Snow.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Blood */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Blood"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Particle/Blood.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_YellowDust */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_YellowDust"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Particle/YellowDust.png"), 1))))
		return E_FAIL;

	/* For. Prototype_GameObject_Snow*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Snow"), CSnow::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Blood */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Blood"), CBlood::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_YellowDust */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_YellowDust"), CYellowDust::Create(m_pGraphic_Device))))
		return E_FAIL;

#pragma endregion
	return S_OK;
}

CMainApp* Client::CMainApp::Create()
{
	CMainApp* pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::CMainApp::Free()
{
	__super::Free();

	Safe_Release(m_pGraphic_Device);

	m_pGameInstance->Release_Engine();

	Safe_Release(m_pGameInstance);	

	//CEffect_Manager::GetInstance()->Release_Effect_Manager();
	//CParticle_Manager::GetInstance()->Release_Particle_Manager();
	//CBullet_Manager::GetInstance()->Release_Bullet_Manager();
}
