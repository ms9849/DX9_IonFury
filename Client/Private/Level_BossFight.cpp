#include "Level_BossFight.h"

#include "GameInstance.h"
#include "CFile_Manager.h"
#include "Level_Loading.h"
#include "UIHp.h"
#include "UIBullets.h"
#include "UIInteraction.h"
#include "UIAim.h"
#include "UIArmor.h"
#include "UIItemQueue.h"
#include "UICardKey.h"
#include "UIUseableItems.h"
#include "Terrain.h"
#include "ParticleSystem.h"
#include "MapTrashBox.h"
#include "Spawner.h"
#include "UIBossHpBar.h"
#include "UIBossHpFill.h"
#include "UIBossName.h"
#include "Boss.h"

#include "Particle_Manager.h"
#include "Bullet_Manager.h"
#include "Terrain_Manager.h"
#include "Effect_Manager.h"
#include "Level_Ending.h"

CLevel_BossFight::CLevel_BossFight(LPDIRECT3DDEVICE9 pGraphic_Device, LEVEL eLevelID)
	: CLevel{ pGraphic_Device, ENUM_CLASS(eLevelID) }
{
}

HRESULT CLevel_BossFight::Initialize(void* pArg)
{
	m_pGameInstance->PlayBGM(L"bango.xm", 0.7f);

	if (pArg != nullptr)
		m_tPlayerInfo = *static_cast<CPlayer::PLAYER_INFO*>(pArg);

	if (FAILED(Ready_Objects_By_JSON()))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	//if (FAILED(Ready_Layer_Spawner(TEXT("Layer_Spawner"))))
	//	return E_FAIL;
	//
	////if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
	////	return E_FAIL;
	//
	if (FAILED(Ready_Layer_Boss(TEXT("Layer_Boss"))))
		return E_FAIL;
	// 
	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;
	
	 if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
	 	return E_FAIL;
	 
	 if (FAILED(Ready_Layer_Particle(TEXT("Layer_Particle"))))
	 	return E_FAIL;
	// 
	//if (FAILED(Ready_Layer_Items(TEXT("Layer_Items"))))
	//	return E_FAIL;
	//
	if (FAILED(Ready_Layer_Map_Objects_AABB(TEXT("Layer_Map_Objects_AABB")))) // 벽 같이 회전 안한 큐브
		return E_FAIL;
	
	if (FAILED(Ready_Layer_Map_Objects_AABB_Ride(TEXT("Layer_Map_Objects_AABB_Ride")))) // 회전 안한 탈 수 있는 큐브
		return E_FAIL;
	
	////if (FAILED(Ready_Layer_Map_Objects_OBB(TEXT("Layer_Map_Objects_OBB")))) // 회전 한 큐브
	////	return E_FAIL;
	//
	//if (FAILED(Ready_Layer_Map_Objects_OBB_Ride(TEXT("Layer_Map_Objects_OBB_Ride")))) // 회전 한 탈 수 있는 큐브
	//	return E_FAIL;
	//
	//if (FAILED(Ready_Layer_Map_Objects_Ride(TEXT("Layer_Map_Objects_Ride")))) // 엘베
	//	return E_FAIL;
	//
	//if (FAILED(Ready_Layer_Map_Objects_Gate(TEXT("Layer_Map_Objects_Gate")))) // 문
	//	return E_FAIL;
	//
	////if (FAILED(Ready_Layer_Map_Objects_Ray(TEXT("Layer_Map_Objects_Ray"))))
	////	return E_FAIL;
	//
	//if (FAILED(Ready_Layer_Map_Objects_Deco(TEXT("Layer_Map_Objects_Deco")))) // 데코레이션
	//	return E_FAIL;
	//
	//if (FAILED(Ready_Layer_Interaction_Objects(TEXT("Layer_Interaction_Objects"))))
	//	return E_FAIL;

	if (FAILED(Ready_Layer_Bullet(TEXT("Layer_Bullet"))))
	 	return E_FAIL;

	m_pTerrain_Manager->Add_Terrian(LEVEL::BOSSFIGHT);
	m_pTerrain_Manager->Add_Cube(LEVEL::BOSSFIGHT);

	return S_OK;
}

void CLevel_BossFight::Update(_float fTimeDelta)
{
	m_pUIHp->Set_Hp();
	m_pUIArmor->Set_Armor();
	m_pUIBullets->Set_Bullets();
	m_pUICardKey->Set_CardKey();
	m_pUIInteraction->Set_Interaction();
	m_pUIUseableItems->Set_UseableItems();

	size_t iItemQueueLength = dynamic_cast<CPlayer*>(
		m_pGameInstance->Find_GameObject_ToLayer(
			ENUM_CLASS(LEVEL::BOSSFIGHT),
			TEXT("Layer_Player")
		))->Get_ItemQueue_Length();

	for (size_t i = 0; i < 20; ++i)
	{
		m_pUIItemQueues[i]->Set_ItemQueue(TEXT(" "));
	}

	for (size_t i = 0; i < iItemQueueLength; ++i)
	{
		_wstring strText = dynamic_cast<CPlayer*>(
			m_pGameInstance->Find_GameObject_ToLayer(
				ENUM_CLASS(LEVEL::BOSSFIGHT),
				TEXT("Layer_Player")
			))->Get_ItemText(i);

		m_pUIItemQueues[i]->Set_ItemQueue(strText);
	}

	m_pTerrain_Manager->Check_Landing();

	m_pGameInstance->Check_RayToAABBCollision(TEXT("Layer_PlayerBullet"), TEXT("Layer_Monster"), ENUM_CLASS(LEVEL::BOSSFIGHT), fTimeDelta, nullptr);
	m_pGameInstance->Check_RayToAABBCollision(TEXT("Layer_PlayerBullet"), TEXT("Layer_Boss1_Upper"), ENUM_CLASS(LEVEL::BOSSFIGHT), fTimeDelta, nullptr);
	m_pGameInstance->Check_RayToAABBCollision(TEXT("Layer_PlayerBullet"), TEXT("Layer_Boss1_Lower"), ENUM_CLASS(LEVEL::BOSSFIGHT), fTimeDelta, nullptr);
	m_pGameInstance->Check_RayToAABBCollision(TEXT("Layer_Monster_Bullet"), TEXT("Layer_Player"), ENUM_CLASS(LEVEL::BOSSFIGHT), fTimeDelta, nullptr);

	////
	m_pGameInstance->Check_SphereCollision(TEXT("Layer_Player"), TEXT("Layer_Items"), ENUM_CLASS(LEVEL::BOSSFIGHT), fTimeDelta);
	m_pGameInstance->Check_SphereCollision(TEXT("Layer_Player"), TEXT("Layer_Interaction_Objects"), ENUM_CLASS(LEVEL::BOSSFIGHT), fTimeDelta);
	m_pGameInstance->Check_SphereCollision(TEXT("Layer_Player"), TEXT("Layer_Melee_Attack"), ENUM_CLASS(LEVEL::BOSSFIGHT), fTimeDelta);
	m_pGameInstance->Check_SphereCollision(TEXT("Layer_Player"), TEXT("Layer_Map_Objects_AABB_Ride"), ENUM_CLASS(LEVEL::BOSSFIGHT), fTimeDelta);

	m_pGameInstance->Check_AABBCollision(TEXT("Layer_Player"), TEXT("Layer_Map_Objects_AABB"), ENUM_CLASS(LEVEL::BOSSFIGHT), fTimeDelta);
	m_pGameInstance->Check_AABBCollision(TEXT("Layer_Player"), TEXT("Layer_Map_Objects_AABB_Ride"), ENUM_CLASS(LEVEL::BOSSFIGHT), fTimeDelta);

	//m_pGameInstance->Check_AABBCollision(TEXT("Layer_Monster"), TEXT("Layer_Monster"), ENUM_CLASS(LEVEL::BOSSFIGHT), fTimeDelta);
	//m_pGameInstance->Check_AABBCollision(TEXT("Layer_Monster"), TEXT("Layer_Map_Objects_AABB"), ENUM_CLASS(LEVEL::BOSSFIGHT), fTimeDelta);

	//m_pGameInstance->Check_AABBCollision(TEXT("Layer_Player"), TEXT("Layer_Spawner"), ENUM_CLASS(LEVEL::BOSSFIGHT), fTimeDelta);

	//m_pGameInstance->Check_RayToAABBCollision(TEXT("Layer_PlayerBullet"), TEXT("Layer_Map_Objects_AABB"), ENUM_CLASS(LEVEL::BOSSFIGHT), fTimeDelta, nullptr);
	//m_pGameInstance->Check_RayToAABBCollision(TEXT("Layer_PlayerBullet"), TEXT("Layer_Map_Objects_AABB_Ride"), ENUM_CLASS(LEVEL::BOSSFIGHT), fTimeDelta, nullptr);
	//m_pGameInstance->Check_RayToAABBCollision(TEXT("Layer_PlayerBullet"), TEXT("Layer_Map_Objects_Ray"), ENUM_CLASS(LEVEL::BOSSFIGHT), fTimeDelta, nullptr);

	//m_pGameInstance->Check_RayToAABBCollision(TEXT("Layer_Monster_Bullet"), TEXT("Layer_Player"), ENUM_CLASS(LEVEL::BOSSFIGHT), fTimeDelta, nullptr);
	//m_pGameInstance->Check_RayToAABBCollision(TEXT("Layer_Monster_Bullet"), TEXT("Layer_Map_Objects_AABB"), ENUM_CLASS(LEVEL::BOSSFIGHT), fTimeDelta, nullptr);
	//m_pGameInstance->Check_RayToAABBCollision(TEXT("Layer_Monster_Bullet"), TEXT("Layer_Map_Objects_AABB_Ride"), ENUM_CLASS(LEVEL::BOSSFIGHT), fTimeDelta, nullptr);
	//m_pGameInstance->Check_RayToAABBCollision(TEXT("Layer_Monster_Bullet"), TEXT("Layer_Map_Objects_Ray"), ENUM_CLASS(LEVEL::BOSSFIGHT), fTimeDelta, nullptr);

	if (m_pGameInstance->Key_Down(VK_F9))
	{
		if (FAILED(m_pGameInstance->Change_Level(
				CLevel_Ending::Create(m_pGraphic_Device, LEVEL::ENDING))))
			return;

	}
	
	if (m_pBoss->isDead() == true)
	{
		if (m_bFlag == false)
		{
			m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Effect_Black_Sight"), ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Layer_Effect"), nullptr);
			m_bFlag = true;
		}

		m_fTimeAcc += fTimeDelta;

		if (m_fTimeAcc >= 5.f)
		{
			if (FAILED(m_pGameInstance->Change_Level(
				CLevel_Ending::Create(m_pGraphic_Device, LEVEL::ENDING))))
				return;
		}
	}
}

HRESULT CLevel_BossFight::Render()
{
	SetWindowText(g_hWnd, TEXT("게임플레이레벨이빈다"));
	return S_OK;
}

HRESULT CLevel_BossFight::Ready_Objects_By_JSON()
{
	// json 파일 로드
	m_pFileMgr = CFile_Manager::Create();
	if (m_pFileMgr == nullptr)
		return E_FAIL;

	if (FAILED(m_pFileMgr->Load_Layers(LEVEL::BOSSFIGHT)))
		return E_FAIL;

	m_ObjectDescs = m_pFileMgr->Get_ObjectDescs();

	return S_OK;
}

HRESULT CLevel_BossFight::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	for (auto& iter : m_ObjectDescs->find(strLayerTag)->second)
	{
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(iter.iProtoLevel), iter.strProto,
			ENUM_CLASS(iter.iLayerLevel), iter.strLayer, &iter)))
			return E_FAIL;

		dynamic_cast<CTransform*>(
			m_pGameInstance->Get_Component(
				ENUM_CLASS(iter.iLayerLevel), iter.strLayer, TEXT("Com_Transform"), iter.iObjectID)
			)->Set_State(STATE::RIGHT, iter.matWorld.m[0]);
		dynamic_cast<CTransform*>(
			m_pGameInstance->Get_Component(
				ENUM_CLASS(iter.iLayerLevel), iter.strLayer, TEXT("Com_Transform"), iter.iObjectID)
			)->Set_State(STATE::UP, iter.matWorld.m[1]);
		dynamic_cast<CTransform*>(
			m_pGameInstance->Get_Component(
				ENUM_CLASS(iter.iLayerLevel), iter.strLayer, TEXT("Com_Transform"), iter.iObjectID)
			)->Set_State(STATE::LOOK, iter.matWorld.m[2]);
		dynamic_cast<CTransform*>(
			m_pGameInstance->Get_Component(
				ENUM_CLASS(iter.iLayerLevel), iter.strLayer, TEXT("Com_Transform"), iter.iObjectID)
			)->Set_State(STATE::POSITION, iter.matWorld.m[3]);
	}
	/*if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Prototype_GameObject_Terrain"),
		ENUM_CLASS(LEVEL::BOSSFIGHT), strLayerTag)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Prototype_GameObject_Sky"),
		ENUM_CLASS(LEVEL::BOSSFIGHT), strLayerTag)))
		return E_FAIL;*/

		//dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Layer_BackGround"), TEXT("Com_Transform"), 1))->Set_State(STATE::POSITION, _float3{ 5.f, 0.f, 5.f });

		//dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Layer_BackGround"), TEXT("Com_Transform"), 1))->Rotation(
		//	dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Layer_BackGround"), TEXT("Com_Transform"), 1))->Get_State(STATE::RIGHT), D3DXToRadian(-20.f)
		//);
		//
	m_pTerrain_Manager = CTerrain_Manager::GetInstance();

	return S_OK;
}

HRESULT CLevel_BossFight::Ready_Layer_Camera(const _wstring& strLayerTag)
{
	CCamera::CAMERA_DESC			CameraDesc{};
	CameraDesc.fFov = D3DXToRadian(60.0f);
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 300.f;
	CameraDesc.vEye = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Layer_Player"), TEXT("Com_Transform")))->Get_State(STATE::POSITION);
	CameraDesc.vAt = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Layer_Player"), TEXT("Com_Transform")))->Get_State(STATE::LOOK);
	CameraDesc.fSpeedPerSec = 4.f;
	CameraDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Camera"),
		ENUM_CLASS(LEVEL::BOSSFIGHT), strLayerTag, &CameraDesc)))
		return E_FAIL;

	CCamera* pCamera = dynamic_cast<CCamera*>(m_pGameInstance->Find_GameObject_ToLayer(ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Layer_Camera")));

	//pCamera->Camera_Configure_Clear(m_CameraSettings);
	m_CameraSettings.pTarget = dynamic_cast<CPlayer*>(m_pGameInstance->Find_GameObject_ToLayer(ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Layer_Player")));
	m_CameraSettings.isChaseTarget = true;
	m_CameraSettings.isSyncLook = true;
	m_CameraSettings.isMouseFixCenter = true;
	m_CameraSettings.isCanTurn = true;
	//m_CameraSettings.vLimitDistance = _float3{ 0.f, 0.f, -1.f };
	SetCursorPos(g_iWinSizeX * 0.5f, g_iWinSizeY * 0.5f);

	pCamera->Camera_Configure(m_CameraSettings);

	return S_OK;
}

HRESULT CLevel_BossFight::Ready_Layer_Player(const _wstring& strLayerTag)
{
	for (auto& iter : m_ObjectDescs->find(strLayerTag)->second)
	{
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(iter.iProtoLevel), iter.strProto,
			ENUM_CLASS(iter.iLayerLevel), iter.strLayer, &iter)))
			return E_FAIL;

		dynamic_cast<CPlayer*>(
			m_pGameInstance->Find_GameObject_ToLayer(
				ENUM_CLASS(iter.iLayerLevel), iter.strLayer)
			)->Set_Player_Info(m_tPlayerInfo);

		dynamic_cast<CTransform*>(
			m_pGameInstance->Get_Component(
				ENUM_CLASS(iter.iLayerLevel), iter.strLayer, TEXT("Com_Transform"), iter.iObjectID)
			)->Set_State(STATE::RIGHT, iter.matWorld.m[0]);
		dynamic_cast<CTransform*>(
			m_pGameInstance->Get_Component(
				ENUM_CLASS(iter.iLayerLevel), iter.strLayer, TEXT("Com_Transform"), iter.iObjectID)
			)->Set_State(STATE::UP, iter.matWorld.m[1]);
		dynamic_cast<CTransform*>(
			m_pGameInstance->Get_Component(
				ENUM_CLASS(iter.iLayerLevel), iter.strLayer, TEXT("Com_Transform"), iter.iObjectID)
			)->Set_State(STATE::LOOK, iter.matWorld.m[2]);
		dynamic_cast<CTransform*>(
			m_pGameInstance->Get_Component(
				ENUM_CLASS(iter.iLayerLevel), iter.strLayer, TEXT("Com_Transform"), iter.iObjectID)
			)->Set_State(STATE::POSITION, iter.matWorld.m[3]);

	}

	/*if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Prototype_GameObject_Player"),
		ENUM_CLASS(LEVEL::BOSSFIGHT), strLayerTag)))
		return E_FAIL;*/

		//m_pTerrain_Manager->Add_LandObject(LEVEL::BOSSFIGHT, TEXT("Layer_Player"));
	m_pTerrain_Manager->Add_LandObject(LEVEL::BOSSFIGHT, TEXT("Layer_Player"));

	return S_OK;
}

HRESULT CLevel_BossFight::Ready_Layer_Spawner(const _wstring& strLayerTag)
{
	/*for (size_t i = 0; i < 30; i++)
	{
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Prototype_GameObject_Spawner"),
			ENUM_CLASS(LEVEL::BOSSFIGHT), strLayerTag, _float3(0.5f, 0.5f, 0.5f))))
			return E_FAIL;
	}*/

	CSpawner::SPAWNER_DESC desc;
	desc.vPos = _float3(22.5f, 1.f, 12.5f);
	desc.idx = 0;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Prototype_GameObject_Spawner"),
		ENUM_CLASS(LEVEL::BOSSFIGHT), strLayerTag, &desc)))
		return E_FAIL;

	desc.vPos = _float3(12.5f, 1.f, 5.5f);
	desc.idx = 1;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Prototype_GameObject_Spawner"),
		ENUM_CLASS(LEVEL::BOSSFIGHT), strLayerTag, &desc)))
		return E_FAIL;

	desc.vPos = _float3(15.5f, 1.f, 8.5f);
	desc.idx = 2;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Prototype_GameObject_Spawner"),
		ENUM_CLASS(LEVEL::BOSSFIGHT), strLayerTag, &desc)))
		return E_FAIL;

	desc.vPos = _float3(17.5f, 1.f, 10.5f);
	desc.idx = 3;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Prototype_GameObject_Spawner"),
		ENUM_CLASS(LEVEL::BOSSFIGHT), strLayerTag, &desc)))
		return E_FAIL;

	desc.vPos = _float3(20.5f, 1.f, 13.5f);
	desc.idx = 0;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Prototype_GameObject_Spawner"),
		ENUM_CLASS(LEVEL::BOSSFIGHT), strLayerTag, &desc)))
		return E_FAIL;

	desc.vPos = _float3(23.5f, 1.f, 17.5f);
	desc.idx = 1;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Prototype_GameObject_Spawner"),
		ENUM_CLASS(LEVEL::BOSSFIGHT), strLayerTag, &desc)))
		return E_FAIL;

	desc.vPos = _float3(26.5f, 1.f, 22.5f);
	desc.idx = 2;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Prototype_GameObject_Spawner"),
		ENUM_CLASS(LEVEL::BOSSFIGHT), strLayerTag, &desc)))
		return E_FAIL;

	desc.vPos = _float3(16.5f, 1.f, 30.5f);
	desc.idx = 3;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Prototype_GameObject_Spawner"),
		ENUM_CLASS(LEVEL::BOSSFIGHT), strLayerTag, &desc)))
		return E_FAIL;
}

HRESULT CLevel_BossFight::Ready_Layer_Monster(const _wstring& strLayerTag)
{
	for (auto& iter : m_ObjectDescs->find(strLayerTag)->second)
	{
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(iter.iProtoLevel), iter.strProto,
			ENUM_CLASS(iter.iLayerLevel), iter.strLayer, &iter)))
			return E_FAIL;
	}

	m_pTerrain_Manager->Add_LandObject(LEVEL::BOSSFIGHT, TEXT("Layer_Monster"));

	return S_OK;
}

HRESULT CLevel_BossFight::Ready_Layer_Boss(const _wstring& strLayerTag)
{
	for (size_t i = 0; i < 1; i++)
	{
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Prototype_GameObject_Monster_Boss"),
			ENUM_CLASS(LEVEL::BOSSFIGHT), strLayerTag)))
			return E_FAIL;
	}

	m_pBoss = static_cast<CBoss*>(m_pGameInstance->Find_GameObject_ToLayer(ENUM_CLASS(LEVEL::BOSSFIGHT), strLayerTag));
	Safe_AddRef(m_pBoss);
	//m_pTerrain_Manager->Add_LandObject(LEVEL::BOSSFIGHT, TEXT("Layer_Boss"));

	return S_OK;
}

HRESULT CLevel_BossFight::Ready_Layer_UI(const _wstring& strLayerTag)
{
	_wstring ws{};

	/* 보스 체력 */
	CUIObject::UIOBJECT_DESC Desc_BossHpBar{};
	//Desc_Hp.iTextLength = 3;
	Desc_BossHpBar.fSizeX = 1000.f;
	Desc_BossHpBar.fSizeY = 400.f;
	Desc_BossHpBar.fX = g_iWinSizeX * 0.5f;
	Desc_BossHpBar.fY = 65.f;
	Desc_BossHpBar.iLayerLevelIndex = ENUM_CLASS(LEVEL::BOSSFIGHT);
	Desc_BossHpBar.strLayerTag = strLayerTag;
	//Desc_Hp.strFontType = TEXT("Default");

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Prototype_GameObject_UIBossHpBar"),
		ENUM_CLASS(LEVEL::BOSSFIGHT), Desc_BossHpBar.strLayerTag, &Desc_BossHpBar)))
		return E_FAIL;

	m_pUIBossHpBar = dynamic_cast<CUIBossHpBar*>(m_pGameInstance->Find_GameObject_ToLayer(ENUM_CLASS(LEVEL::BOSSFIGHT), Desc_BossHpBar.strLayerTag));
	Safe_AddRef(m_pUIBossHpBar);

	/* 보스 체력 필 */
	CUIObject::UIOBJECT_DESC Desc_BossHpFill{};
	//Desc_Hp.iTextLength = 3;
	Desc_BossHpFill.fSizeX = 2018.f;
	Desc_BossHpFill.fSizeY = 600.f;
	//Desc_BossHpFill.fX = g_iWinSizeX * 0.5f - 11.f;
	Desc_BossHpFill.fX = g_iWinSizeX - 25.f;
	Desc_BossHpFill.fY = 82.f;
	Desc_BossHpFill.iLayerLevelIndex = ENUM_CLASS(LEVEL::BOSSFIGHT);
	Desc_BossHpFill.strLayerTag = strLayerTag;
	//Desc_Hp.strFontType = TEXT("Default");

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Prototype_GameObject_UIBossHpFill"),
		ENUM_CLASS(LEVEL::BOSSFIGHT), Desc_BossHpFill.strLayerTag, &Desc_BossHpFill)))
		return E_FAIL;

	m_pUIBossHpFill = dynamic_cast<CUIBossHpFill*>(m_pGameInstance->Find_GameObject_ToLayer(ENUM_CLASS(LEVEL::BOSSFIGHT), Desc_BossHpBar.strLayerTag));
	Safe_AddRef(m_pUIBossHpFill);

	/* 보스 이름 */
	CUIObject::UIOBJECT_DESC Desc_BossName{};
	//Desc_Hp.iTextLength = 3;
	Desc_BossName.fSizeX = 400.f;
	Desc_BossName.fSizeY = 250.f;
	Desc_BossName.fX = g_iWinSizeX * 0.5f;
	Desc_BossName.fY = 42.f;
	Desc_BossName.iLayerLevelIndex = ENUM_CLASS(LEVEL::BOSSFIGHT);
	Desc_BossName.strLayerTag = strLayerTag;
	//Desc_Hp.strFontType = TEXT("Default");

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Prototype_GameObject_UIBossName"),
		ENUM_CLASS(LEVEL::BOSSFIGHT), Desc_BossName.strLayerTag, &Desc_BossName)))
		return E_FAIL;

	m_pUIBossName = dynamic_cast<CUIBossName*>(m_pGameInstance->Find_GameObject_ToLayer(ENUM_CLASS(LEVEL::BOSSFIGHT), Desc_BossHpBar.strLayerTag));
	Safe_AddRef(m_pUIBossName);

	/* 체력 */
	CUIObject::UIOBJECT_DESC Desc_Hp{};
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Find_GameObject_ToLayer(ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Layer_Player")));
	Safe_AddRef(pPlayer);

	ws = to_wstring(pPlayer->Get_Player_Info().iHp);

	// UI전체 크기 및 위치
	Desc_Hp.iTextLength = 3;
	Desc_Hp.fSizeX = 70.f + (70.f * 2);
	Desc_Hp.fSizeY = 70.f;
	Desc_Hp.fX = 10.f;
	Desc_Hp.fY = g_iWinSizeY - (Desc_Hp.fSizeY * 0.5f) - 10.f;
	Desc_Hp.iLayerLevelIndex = ENUM_CLASS(LEVEL::BOSSFIGHT);
	Desc_Hp.strLayerTag = strLayerTag;
	Desc_Hp.strFontType = TEXT("Default");

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Prototype_GameObject_UIHp"),
		ENUM_CLASS(LEVEL::BOSSFIGHT), Desc_Hp.strLayerTag, &Desc_Hp)))
		return E_FAIL;

	m_pUIHp = dynamic_cast<CUIHp*>(m_pGameInstance->Find_GameObject_ToLayer(ENUM_CLASS(LEVEL::BOSSFIGHT), Desc_Hp.strLayerTag));
	Safe_AddRef(m_pUIHp);

	/* 방어력 */
	CUIObject::UIOBJECT_DESC Desc_Armor{};

	ws = to_wstring(pPlayer->Get_Player_Info().iArmor);

	// UI전체 크기 및 위치
	Desc_Armor.iTextLength = 3;
	Desc_Armor.fSizeX = 70.f + (70.f * 2);
	Desc_Armor.fSizeY = 70.f;
	Desc_Armor.fX = Desc_Hp.fX + Desc_Hp.fSizeX;
	Desc_Armor.fY = g_iWinSizeY - (Desc_Hp.fSizeY * 0.5f) - 10.f;
	Desc_Armor.iLayerLevelIndex = ENUM_CLASS(LEVEL::BOSSFIGHT);
	Desc_Armor.strLayerTag = strLayerTag;
	Desc_Armor.strFontType = TEXT("Default");

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Prototype_GameObject_UIArmor"),
		ENUM_CLASS(LEVEL::BOSSFIGHT), Desc_Armor.strLayerTag, &Desc_Armor)))
		return E_FAIL;

	m_pUIArmor = dynamic_cast<CUIArmor*>(m_pGameInstance->Find_GameObject_ToLayer(ENUM_CLASS(LEVEL::BOSSFIGHT), Desc_Armor.strLayerTag));
	Safe_AddRef(m_pUIArmor);

	/* 총알 */
	CUIObject::UIOBJECT_DESC Desc_Bullets{};

	ws = to_wstring(pPlayer->Get_Player_Info().iBullets);

	Desc_Bullets.iTextLength = 6;
	Desc_Bullets.fSizeX = 70.f + (70.f * Desc_Bullets.iTextLength);
	Desc_Bullets.fSizeY = 70.f;
	Desc_Bullets.fX = g_iWinSizeX - 10.f;
	Desc_Bullets.fY = g_iWinSizeY - (Desc_Bullets.fSizeY * 0.5f) - 10.f;
	Desc_Bullets.iLayerLevelIndex = ENUM_CLASS(LEVEL::BOSSFIGHT);
	Desc_Bullets.strLayerTag = strLayerTag;
	Desc_Bullets.strFontType = TEXT("Default");

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Prototype_GameObject_UIBullets"),
		ENUM_CLASS(LEVEL::BOSSFIGHT), Desc_Bullets.strLayerTag, &Desc_Bullets)))
		return E_FAIL;

	m_pUIBullets = dynamic_cast<CUIBullets*>(m_pGameInstance->Find_GameObject_ToLayer(ENUM_CLASS(LEVEL::BOSSFIGHT), strLayerTag));
	Safe_AddRef(m_pUIBullets);

	/* 상호작용 키 */
	CUIObject::UIOBJECT_DESC Desc_Interaction{};

	Desc_Interaction.iTextLength = wcslen(TEXT("Press [E] Key"));
	Desc_Interaction.fSizeX = 30.f * Desc_Interaction.iTextLength;
	Desc_Interaction.fSizeY = 30.f;
	Desc_Interaction.fX = g_iWinSizeX * 0.5f - (Desc_Interaction.fSizeX * 0.5f);
	Desc_Interaction.fY = g_iWinSizeY * 0.8f;
	Desc_Interaction.iLayerLevelIndex = ENUM_CLASS(LEVEL::BOSSFIGHT);
	Desc_Interaction.strLayerTag = strLayerTag;
	Desc_Interaction.strFontType = TEXT("Primary");

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Prototype_GameObject_UIInteraction"),
		ENUM_CLASS(LEVEL::BOSSFIGHT), Desc_Interaction.strLayerTag, &Desc_Interaction)))
		return E_FAIL;

	m_pUIInteraction = dynamic_cast<CUIInteraction*>(m_pGameInstance->Find_GameObject_ToLayer(ENUM_CLASS(LEVEL::BOSSFIGHT), strLayerTag));
	Safe_AddRef(m_pUIInteraction);

	/* 아이템 스택 */
	for (size_t i = 0; i < 20; ++i)
	{
		CUIObject::UIOBJECT_DESC Desc_ItemQueue{};

		Desc_ItemQueue.iTextLength = 50;
		Desc_ItemQueue.fSizeX = 16.f * Desc_ItemQueue.iTextLength;
		Desc_ItemQueue.fSizeY = 16.f;
		Desc_ItemQueue.fX = 10.f;
		Desc_ItemQueue.fY = 10.f + (Desc_ItemQueue.fSizeY * 0.5f) + (i * 20);
		Desc_ItemQueue.iLayerLevelIndex = ENUM_CLASS(LEVEL::BOSSFIGHT);
		Desc_ItemQueue.strLayerTag = strLayerTag;
		Desc_ItemQueue.strFontType = TEXT("Primary");

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Prototype_GameObject_UIItemQueue"),
			ENUM_CLASS(LEVEL::BOSSFIGHT), Desc_ItemQueue.strLayerTag, &Desc_ItemQueue)))
			return E_FAIL;

		CUIItemQueue* pUIItemQueue = dynamic_cast<CUIItemQueue*>(m_pGameInstance->Find_GameObject_ToLayer(ENUM_CLASS(LEVEL::BOSSFIGHT), Desc_ItemQueue.strLayerTag));
		Safe_AddRef(pUIItemQueue);

		pUIItemQueue->Set_ItemQueue(TEXT(" "));

		m_pUIItemQueues.push_back(pUIItemQueue);
	}

	/* 조준선 */
	CUIObject::UIOBJECT_DESC Desc_Aim{};

	Desc_Aim.fSizeX = 30.f;
	Desc_Aim.fSizeY = 30.f;
	Desc_Aim.iLayerLevelIndex = ENUM_CLASS(LEVEL::BOSSFIGHT);
	Desc_Aim.fX = g_iWinSizeX * 0.5f - (Desc_Aim.fSizeX * 0.5f);
	Desc_Aim.fY = g_iWinSizeY * 0.5f - (Desc_Aim.fSizeY * 0.5f);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Prototype_GameObject_UIAim"),
		ENUM_CLASS(LEVEL::BOSSFIGHT), strLayerTag, &Desc_Aim)))
		return E_FAIL;

	m_pUIAim = dynamic_cast<CUIAim*>(m_pGameInstance->Find_GameObject_ToLayer(ENUM_CLASS(LEVEL::BOSSFIGHT), strLayerTag));
	Safe_AddRef(m_pUIAim);

	/* 카드키 */
	CUIObject::UIOBJECT_DESC Desc_CardKey{};

	Desc_CardKey.fSizeX = 92.f;
	Desc_CardKey.fSizeY = 56.f;
	Desc_CardKey.iLayerLevelIndex = ENUM_CLASS(LEVEL::BOSSFIGHT);
	Desc_CardKey.fX = g_iWinSizeX - 20.f - (Desc_CardKey.fSizeX * 0.5f);
	Desc_CardKey.fY = g_iWinSizeY - 100.f - (Desc_CardKey.fSizeY * 0.5f);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Prototype_GameObject_UICardKey"),
		ENUM_CLASS(LEVEL::BOSSFIGHT), strLayerTag, &Desc_CardKey)))
		return E_FAIL;

	m_pUICardKey = dynamic_cast<CUICardKey*>(m_pGameInstance->Find_GameObject_ToLayer(ENUM_CLASS(LEVEL::BOSSFIGHT), strLayerTag));
	Safe_AddRef(m_pUICardKey);

	/* 사용 가능 아이템 */
	CUIObject::UIOBJECT_DESC Desc_UseableItems{};

	ws = to_wstring(pPlayer->Get_Player_Info().iHealpacks);

	// UI전체 크기 및 위치
	Desc_UseableItems.iTextLength = 3;
	Desc_UseableItems.fSizeX = 50.f + (50.f * 2);
	Desc_UseableItems.fSizeY = 50.f;
	Desc_UseableItems.fX = Desc_Hp.fX + 10;
	Desc_UseableItems.fY = g_iWinSizeY - (Desc_Hp.fSizeY) - 45.f;
	Desc_UseableItems.iLayerLevelIndex = ENUM_CLASS(LEVEL::BOSSFIGHT);
	Desc_UseableItems.strLayerTag = strLayerTag;
	Desc_UseableItems.strFontType = TEXT("Default");

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Prototype_GameObject_UIUseableItems"),
		ENUM_CLASS(LEVEL::BOSSFIGHT), strLayerTag, &Desc_UseableItems)))
		return E_FAIL;

	m_pUIUseableItems = dynamic_cast<CUIUseableItems*>(m_pGameInstance->Find_GameObject_ToLayer(ENUM_CLASS(LEVEL::BOSSFIGHT), strLayerTag));
	Safe_AddRef(m_pUIUseableItems);

	Safe_Release(pPlayer);

	return S_OK;
}

HRESULT CLevel_BossFight::Ready_Layer_Effect(const _wstring& strLayerTag)
{
	m_pEffect_Manager = CEffect_Manager::GetInstance();
	m_pEffect_Manager->Initialize(LEVEL::BOSSFIGHT); 
	Safe_AddRef(m_pEffect_Manager);

	return S_OK;
}

HRESULT CLevel_BossFight::Ready_Layer_Particle(const _wstring& strLayerTag)
{
	m_pParticle_Manager = CParticle_Manager::GetInstance();
	m_pParticle_Manager->Initialize(LEVEL::BOSSFIGHT);
	Safe_AddRef(m_pParticle_Manager);

	return S_OK;

	//ParticleDesc.iNumParticles = 1000;
	//ParticleDesc.vBoundaryMin = _float3(-10.f, -5.f, -10.f);
	//ParticleDesc.vBoundaryMax = _float3(10.f, 10.f, 10.f);

	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Prototype_GameObject_Snow"),
	//	ENUM_CLASS(LEVEL::BOSSFIGHT), strLayerTag, &ParticleDesc)))
	//	return E_FAIL;
}

HRESULT CLevel_BossFight::Ready_Layer_Items(const _wstring& strLayerTag)
{
	for (auto& iter : m_ObjectDescs->find(strLayerTag)->second)
	{
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(iter.iProtoLevel), iter.strProto,
			ENUM_CLASS(iter.iLayerLevel), iter.strLayer, &iter)))
			return E_FAIL;

		dynamic_cast<CTransform*>(
			m_pGameInstance->Get_Component(
				ENUM_CLASS(iter.iLayerLevel), iter.strLayer, TEXT("Com_Transform"), iter.iObjectID)
			)->Set_State(STATE::RIGHT, iter.matWorld.m[0]);
		dynamic_cast<CTransform*>(
			m_pGameInstance->Get_Component(
				ENUM_CLASS(iter.iLayerLevel), iter.strLayer, TEXT("Com_Transform"), iter.iObjectID)
			)->Set_State(STATE::UP, iter.matWorld.m[1]);
		dynamic_cast<CTransform*>(
			m_pGameInstance->Get_Component(
				ENUM_CLASS(iter.iLayerLevel), iter.strLayer, TEXT("Com_Transform"), iter.iObjectID)
			)->Set_State(STATE::LOOK, iter.matWorld.m[2]);
		dynamic_cast<CTransform*>(
			m_pGameInstance->Get_Component(
				ENUM_CLASS(iter.iLayerLevel), iter.strLayer, TEXT("Com_Transform"), iter.iObjectID)
			)->Set_State(STATE::POSITION, iter.matWorld.m[3]);
	}

	/*if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Prototype_GameObject_Item_Armor"),
		ENUM_CLASS(LEVEL::BOSSFIGHT), strLayerTag)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Prototype_GameObject_Item_Healpack"),
		ENUM_CLASS(LEVEL::BOSSFIGHT), strLayerTag)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Prototype_GameObject_Item_Pistol_Bullet"),
		ENUM_CLASS(LEVEL::BOSSFIGHT), strLayerTag)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Prototype_GameObject_Item_ShootGun_Bullet"),
		ENUM_CLASS(LEVEL::BOSSFIGHT), strLayerTag)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Prototype_GameObject_Item_CardKey"),
		ENUM_CLASS(LEVEL::BOSSFIGHT), strLayerTag)))
		return E_FAIL;*/

	return S_OK;
}

HRESULT CLevel_BossFight::Ready_Layer_Map_Objects_AABB(const _wstring& strLayerTag)
{

	for (auto& iter : m_ObjectDescs->find(strLayerTag)->second)
	{
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(iter.iProtoLevel), iter.strProto,
			ENUM_CLASS(iter.iLayerLevel), iter.strLayer, &iter)))
			return E_FAIL;

		dynamic_cast<CTransform*>(
			m_pGameInstance->Get_Component(
				ENUM_CLASS(iter.iLayerLevel), iter.strLayer, TEXT("Com_Transform"), iter.iObjectID)
			)->Set_State(STATE::RIGHT, iter.matWorld.m[0]);
		dynamic_cast<CTransform*>(
			m_pGameInstance->Get_Component(
				ENUM_CLASS(iter.iLayerLevel), iter.strLayer, TEXT("Com_Transform"), iter.iObjectID)
			)->Set_State(STATE::UP, iter.matWorld.m[1]);
		dynamic_cast<CTransform*>(
			m_pGameInstance->Get_Component(
				ENUM_CLASS(iter.iLayerLevel), iter.strLayer, TEXT("Com_Transform"), iter.iObjectID)
			)->Set_State(STATE::LOOK, iter.matWorld.m[2]);
		dynamic_cast<CTransform*>(
			m_pGameInstance->Get_Component(
				ENUM_CLASS(iter.iLayerLevel), iter.strLayer, TEXT("Com_Transform"), iter.iObjectID)
			)->Set_State(STATE::POSITION, iter.matWorld.m[3]);

		/* 콜라이더 꺼내와서 세팅 */
		CBoxCollider* pCollider = dynamic_cast<CBoxCollider*>(m_pGameInstance->Get_Component(ENUM_CLASS(iter.iLayerLevel), iter.strLayer, TEXT("Com_BoxCollider"), iter.iObjectID));
		pCollider->Set_Matrix(iter.matWorld);
	}
	return S_OK;
}

HRESULT CLevel_BossFight::Ready_Layer_Map_Objects_AABB_Ride(const _wstring& strLayerTag)
{

	for (auto& iter : m_ObjectDescs->find(strLayerTag)->second)
	{
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(iter.iProtoLevel), iter.strProto,
			ENUM_CLASS(iter.iLayerLevel), iter.strLayer, &iter)))
			return E_FAIL;

		dynamic_cast<CTransform*>(
			m_pGameInstance->Get_Component(
				ENUM_CLASS(iter.iLayerLevel), iter.strLayer, TEXT("Com_Transform"), iter.iObjectID)
			)->Set_State(STATE::RIGHT, iter.matWorld.m[0]);
		dynamic_cast<CTransform*>(
			m_pGameInstance->Get_Component(
				ENUM_CLASS(iter.iLayerLevel), iter.strLayer, TEXT("Com_Transform"), iter.iObjectID)
			)->Set_State(STATE::UP, iter.matWorld.m[1]);
		dynamic_cast<CTransform*>(
			m_pGameInstance->Get_Component(
				ENUM_CLASS(iter.iLayerLevel), iter.strLayer, TEXT("Com_Transform"), iter.iObjectID)
			)->Set_State(STATE::LOOK, iter.matWorld.m[2]);
		dynamic_cast<CTransform*>(
			m_pGameInstance->Get_Component(
				ENUM_CLASS(iter.iLayerLevel), iter.strLayer, TEXT("Com_Transform"), iter.iObjectID)
			)->Set_State(STATE::POSITION, iter.matWorld.m[3]);

		/* 콜라이더 꺼내와서 세팅 */
		CBoxCollider* pCollider = dynamic_cast<CBoxCollider*>(m_pGameInstance->Get_Component(ENUM_CLASS(iter.iLayerLevel), iter.strLayer, TEXT("Com_BoxCollider"), iter.iObjectID));
		pCollider->Set_Matrix(iter.matWorld);
	}

	return S_OK;
}

HRESULT CLevel_BossFight::Ready_Layer_Map_Objects_OBB(const _wstring& strLayerTag)
{
	for (auto& iter : m_ObjectDescs->find(strLayerTag)->second)
	{
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(iter.iProtoLevel), iter.strProto,
			ENUM_CLASS(iter.iLayerLevel), iter.strLayer, &iter)))
			return E_FAIL;

		dynamic_cast<CTransform*>(
			m_pGameInstance->Get_Component(
				ENUM_CLASS(iter.iLayerLevel), iter.strLayer, TEXT("Com_Transform"), iter.iObjectID)
			)->Set_State(STATE::RIGHT, iter.matWorld.m[0]);
		dynamic_cast<CTransform*>(
			m_pGameInstance->Get_Component(
				ENUM_CLASS(iter.iLayerLevel), iter.strLayer, TEXT("Com_Transform"), iter.iObjectID)
			)->Set_State(STATE::UP, iter.matWorld.m[1]);
		dynamic_cast<CTransform*>(
			m_pGameInstance->Get_Component(
				ENUM_CLASS(iter.iLayerLevel), iter.strLayer, TEXT("Com_Transform"), iter.iObjectID)
			)->Set_State(STATE::LOOK, iter.matWorld.m[2]);
		dynamic_cast<CTransform*>(
			m_pGameInstance->Get_Component(
				ENUM_CLASS(iter.iLayerLevel), iter.strLayer, TEXT("Com_Transform"), iter.iObjectID)
			)->Set_State(STATE::POSITION, iter.matWorld.m[3]);
	}

	return S_OK;
}

HRESULT CLevel_BossFight::Ready_Layer_Map_Objects_OBB_Ride(const _wstring& strLayerTag)
{
	for (auto& iter : m_ObjectDescs->find(strLayerTag)->second)
	{
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(iter.iProtoLevel), iter.strProto,
			ENUM_CLASS(iter.iLayerLevel), iter.strLayer, &iter)))
			return E_FAIL;

		dynamic_cast<CTransform*>(
			m_pGameInstance->Get_Component(
				ENUM_CLASS(iter.iLayerLevel), iter.strLayer, TEXT("Com_Transform"), iter.iObjectID)
			)->Set_State(STATE::RIGHT, iter.matWorld.m[0]);
		dynamic_cast<CTransform*>(
			m_pGameInstance->Get_Component(
				ENUM_CLASS(iter.iLayerLevel), iter.strLayer, TEXT("Com_Transform"), iter.iObjectID)
			)->Set_State(STATE::UP, iter.matWorld.m[1]);
		dynamic_cast<CTransform*>(
			m_pGameInstance->Get_Component(
				ENUM_CLASS(iter.iLayerLevel), iter.strLayer, TEXT("Com_Transform"), iter.iObjectID)
			)->Set_State(STATE::LOOK, iter.matWorld.m[2]);
		dynamic_cast<CTransform*>(
			m_pGameInstance->Get_Component(
				ENUM_CLASS(iter.iLayerLevel), iter.strLayer, TEXT("Com_Transform"), iter.iObjectID)
			)->Set_State(STATE::POSITION, iter.matWorld.m[3]);
	}

	return S_OK;
}

HRESULT CLevel_BossFight::Ready_Layer_Map_Objects_Ray(const _wstring& strLayerTag)
{
	for (auto& iter : m_ObjectDescs->find(strLayerTag)->second)
	{
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(iter.iProtoLevel), iter.strProto,
			ENUM_CLASS(iter.iLayerLevel), iter.strLayer, &iter)))
			return E_FAIL;

		dynamic_cast<CTransform*>(
			m_pGameInstance->Get_Component(
				ENUM_CLASS(iter.iLayerLevel), iter.strLayer, TEXT("Com_Transform"), iter.iObjectID)
			)->Set_State(STATE::RIGHT, iter.matWorld.m[0]);
		dynamic_cast<CTransform*>(
			m_pGameInstance->Get_Component(
				ENUM_CLASS(iter.iLayerLevel), iter.strLayer, TEXT("Com_Transform"), iter.iObjectID)
			)->Set_State(STATE::UP, iter.matWorld.m[1]);
		dynamic_cast<CTransform*>(
			m_pGameInstance->Get_Component(
				ENUM_CLASS(iter.iLayerLevel), iter.strLayer, TEXT("Com_Transform"), iter.iObjectID)
			)->Set_State(STATE::LOOK, iter.matWorld.m[2]);
		dynamic_cast<CTransform*>(
			m_pGameInstance->Get_Component(
				ENUM_CLASS(iter.iLayerLevel), iter.strLayer, TEXT("Com_Transform"), iter.iObjectID)
			)->Set_State(STATE::POSITION, iter.matWorld.m[3]);
	}

	return S_OK;
}

HRESULT CLevel_BossFight::Ready_Layer_Map_Objects_Ride(const _wstring& strLayerTag)
{
	for (auto& iter : m_ObjectDescs->find(strLayerTag)->second)
	{
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(iter.iProtoLevel), iter.strProto,
			ENUM_CLASS(iter.iLayerLevel), iter.strLayer, &iter)))
			return E_FAIL;

		dynamic_cast<CTransform*>(
			m_pGameInstance->Get_Component(
				ENUM_CLASS(iter.iLayerLevel), iter.strLayer, TEXT("Com_Transform"), iter.iObjectID)
			)->Set_State(STATE::RIGHT, iter.matWorld.m[0]);
		dynamic_cast<CTransform*>(
			m_pGameInstance->Get_Component(
				ENUM_CLASS(iter.iLayerLevel), iter.strLayer, TEXT("Com_Transform"), iter.iObjectID)
			)->Set_State(STATE::UP, iter.matWorld.m[1]);
		dynamic_cast<CTransform*>(
			m_pGameInstance->Get_Component(
				ENUM_CLASS(iter.iLayerLevel), iter.strLayer, TEXT("Com_Transform"), iter.iObjectID)
			)->Set_State(STATE::LOOK, iter.matWorld.m[2]);
		dynamic_cast<CTransform*>(
			m_pGameInstance->Get_Component(
				ENUM_CLASS(iter.iLayerLevel), iter.strLayer, TEXT("Com_Transform"), iter.iObjectID)
			)->Set_State(STATE::POSITION, iter.matWorld.m[3]);
	}

	return S_OK;
}

HRESULT CLevel_BossFight::Ready_Layer_Map_Objects_Gate(const _wstring& strLayerTag)
{
	for (auto& iter : m_ObjectDescs->find(strLayerTag)->second)
	{
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(iter.iProtoLevel), iter.strProto,
			ENUM_CLASS(iter.iLayerLevel), iter.strLayer, &iter)))
			return E_FAIL;

		dynamic_cast<CTransform*>(
			m_pGameInstance->Get_Component(
				ENUM_CLASS(iter.iLayerLevel), iter.strLayer, TEXT("Com_Transform"), iter.iObjectID)
			)->Set_State(STATE::RIGHT, iter.matWorld.m[0]);
		dynamic_cast<CTransform*>(
			m_pGameInstance->Get_Component(
				ENUM_CLASS(iter.iLayerLevel), iter.strLayer, TEXT("Com_Transform"), iter.iObjectID)
			)->Set_State(STATE::UP, iter.matWorld.m[1]);
		dynamic_cast<CTransform*>(
			m_pGameInstance->Get_Component(
				ENUM_CLASS(iter.iLayerLevel), iter.strLayer, TEXT("Com_Transform"), iter.iObjectID)
			)->Set_State(STATE::LOOK, iter.matWorld.m[2]);
		dynamic_cast<CTransform*>(
			m_pGameInstance->Get_Component(
				ENUM_CLASS(iter.iLayerLevel), iter.strLayer, TEXT("Com_Transform"), iter.iObjectID)
			)->Set_State(STATE::POSITION, iter.matWorld.m[3]);
	}

	return S_OK;
}

HRESULT CLevel_BossFight::Ready_Layer_Map_Objects_Deco(const _wstring& strLayerTag)
{
	for (auto& iter : m_ObjectDescs->find(strLayerTag)->second)
	{
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(iter.iProtoLevel), iter.strProto,
			ENUM_CLASS(iter.iLayerLevel), iter.strLayer, &iter)))
			return E_FAIL;

		dynamic_cast<CTransform*>(
			m_pGameInstance->Get_Component(
				ENUM_CLASS(iter.iLayerLevel), iter.strLayer, TEXT("Com_Transform"), iter.iObjectID)
			)->Set_State(STATE::RIGHT, iter.matWorld.m[0]);
		dynamic_cast<CTransform*>(
			m_pGameInstance->Get_Component(
				ENUM_CLASS(iter.iLayerLevel), iter.strLayer, TEXT("Com_Transform"), iter.iObjectID)
			)->Set_State(STATE::UP, iter.matWorld.m[1]);
		dynamic_cast<CTransform*>(
			m_pGameInstance->Get_Component(
				ENUM_CLASS(iter.iLayerLevel), iter.strLayer, TEXT("Com_Transform"), iter.iObjectID)
			)->Set_State(STATE::LOOK, iter.matWorld.m[2]);
		dynamic_cast<CTransform*>(
			m_pGameInstance->Get_Component(
				ENUM_CLASS(iter.iLayerLevel), iter.strLayer, TEXT("Com_Transform"), iter.iObjectID)
			)->Set_State(STATE::POSITION, iter.matWorld.m[3]);
	}

	return S_OK;
}

HRESULT CLevel_BossFight::Ready_Layer_Interaction_Objects(const _wstring& strLayerTag)
{
	for (auto& iter : m_ObjectDescs->find(strLayerTag)->second)
	{
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(iter.iProtoLevel), iter.strProto,
			ENUM_CLASS(iter.iLayerLevel), iter.strLayer, &iter)))
			return E_FAIL;

		dynamic_cast<CTransform*>(
			m_pGameInstance->Get_Component(
				ENUM_CLASS(iter.iLayerLevel), iter.strLayer, TEXT("Com_Transform"), iter.iObjectID)
			)->Set_State(STATE::RIGHT, iter.matWorld.m[0]);
		dynamic_cast<CTransform*>(
			m_pGameInstance->Get_Component(
				ENUM_CLASS(iter.iLayerLevel), iter.strLayer, TEXT("Com_Transform"), iter.iObjectID)
			)->Set_State(STATE::UP, iter.matWorld.m[1]);
		dynamic_cast<CTransform*>(
			m_pGameInstance->Get_Component(
				ENUM_CLASS(iter.iLayerLevel), iter.strLayer, TEXT("Com_Transform"), iter.iObjectID)
			)->Set_State(STATE::LOOK, iter.matWorld.m[2]);
		dynamic_cast<CTransform*>(
			m_pGameInstance->Get_Component(
				ENUM_CLASS(iter.iLayerLevel), iter.strLayer, TEXT("Com_Transform"), iter.iObjectID)
			)->Set_State(STATE::POSITION, iter.matWorld.m[3]);
	}
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Prototype_GameObject_Interaction_DoorLock"),
	//	ENUM_CLASS(LEVEL::BOSSFIGHT), strLayerTag)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_BossFight::Ready_Layer_Bullet(const _wstring& strLayerTag)
{
	m_pBullet_Manager = CBullet_Manager::GetInstance();
	m_pBullet_Manager->Initialize();
	Safe_AddRef(m_pBullet_Manager);

	return S_OK;
}

void CLevel_BossFight::Ending()
{
	// 3초 하고 난 뒤에 1초 동안 재생 
	// 
	m_pGameInstance->StopAll();

	//암전 효과 내부에서 블렌딩 처리해줌.
	m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Effect_Black_Sight"), ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Layer_Effect"), nullptr);

	//타임델타 쌓아서 3~4초 뒤면 레벨 엔딩으로 바뀌게끔 해줄 수 있을 듯?
}

CLevel_BossFight* CLevel_BossFight::Create(LPDIRECT3DDEVICE9 pGraphic_Device, LEVEL eLevelID, void* pArg)
{
	CLevel_BossFight* pInstance = new CLevel_BossFight(pGraphic_Device, eLevelID);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CLevel_BossFight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_BossFight::Free()
{
	__super::Free();

	Safe_Release(m_pUIHp);
	Safe_Release(m_pUIArmor);
	Safe_Release(m_pUIBullets);
	Safe_Release(m_pUIInteraction);
	Safe_Release(m_pUIAim);
	Safe_Release(m_pUICardKey);
	Safe_Release(m_pUIUseableItems);
	Safe_Release(m_pFileMgr);
	Safe_Release(m_pUIBossHpBar);
	Safe_Release(m_pUIBossHpFill);
	Safe_Release(m_pUIBossName);
	Safe_Release(m_pBoss);

	Safe_Release(m_pEffect_Manager);
	m_pEffect_Manager->Release_Effect_Manager();

	Safe_Release(m_pParticle_Manager);
	m_pParticle_Manager->Release_Particle_Manager();

	Safe_Release(m_pBullet_Manager);
	m_pBullet_Manager->Release_Bullet_Manager();

	m_pTerrain_Manager->Release_Terrain_Manager();
	Safe_Release(m_pTerrain_Manager);

	for (auto& iter : m_pUIItemQueues)
	{
		Safe_Release(iter);
	}
}
