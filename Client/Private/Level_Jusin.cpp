#include "Level_Jusin.h"

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
#include "Player.h"
#include "MapTrashBox.h"
#include "Spawner.h"

#include "Particle_Manager.h"
#include "Bullet_Manager.h"
#include "Terrain_Manager.h"
#include "Effect_Manager.h"

CLevel_Jusin::CLevel_Jusin(LPDIRECT3DDEVICE9 pGraphic_Device, LEVEL eLevelID, void* pArg)
	: CLevel{ pGraphic_Device, ENUM_CLASS(eLevelID) }
{
}

HRESULT CLevel_Jusin::Initialize(void* pArg)
{
	if (pArg != nullptr)
		m_tPlayerInfo = *static_cast<CPlayer::PLAYER_INFO*>(pArg);

	m_pGameInstance->PlayBGM(L"broken_system.xm", 0.7f);

	if (FAILED(Ready_Objects_By_JSON()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Map_Objects_Ride(TEXT("Layer_Map_Objects_Ride")))) // 엘베
	 	return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	// if (FAILED(Ready_Layer_Spawner(TEXT("Layer_Spawner"))))
	// 	return E_FAIL;

	 if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
	 	return E_FAIL;

	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Particle(TEXT("Layer_Particle"))))
		return E_FAIL;

	 if (FAILED(Ready_Layer_Items(TEXT("Layer_Items"))))
	 	return E_FAIL;

	 if (FAILED(Ready_Layer_EventBox(TEXT("Layer_EventBox"))))
		 return E_FAIL;
	 
	if (FAILED(Ready_Layer_Map_Objects_AABB(TEXT("Layer_Map_Objects_AABB")))) // 벽 같이 회전 안한 큐브
		return E_FAIL;

	if (FAILED(Ready_Layer_Map_Objects_AABB_Ride(TEXT("Layer_Map_Objects_AABB_Ride")))) // 회전 안한 탈 수 있는 큐브
		return E_FAIL;

	// if (FAILED(Ready_Layer_Map_Objects_OBB(TEXT("Layer_Map_Objects_OBB")))) // 회전 한 큐브
	// 	return E_FAIL;

	// if (FAILED(Ready_Layer_Map_Objects_OBB_Ride(TEXT("Layer_Map_Objects_OBB_Ride")))) // 회전 한 탈 수 있는 큐브
	// 	return E_FAIL;
	
	 
	if (FAILED(Ready_Layer_Map_Objects_Gate(TEXT("Layer_Map_Objects_Gate")))) // 문
		return E_FAIL;
	
	if (FAILED(Ready_Layer_Map_Objects_Ray(TEXT("Layer_Map_Objects_Ray"))))
		return E_FAIL;
	
	if (FAILED(Ready_Layer_Map_Objects_Deco(TEXT("Layer_Map_Objects_Deco")))) // 데코레이션
		return E_FAIL;
	
	if (FAILED(Ready_Layer_Interaction_Objects(TEXT("Layer_Interaction_Objects"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Bullet(TEXT("Layer_Bullet"))))
		return E_FAIL;

	m_pTerrain_Manager->Add_Terrian(LEVEL::JUSIN);
	m_pTerrain_Manager->Add_Cube(LEVEL::JUSIN);

	return S_OK;
}

void CLevel_Jusin::Update(_float fTimeDelta)
{
	//if (GetKeyState('1') & 0x8000)
	//{
	//	m_pCamera->Camera_Configure_Clear(m_CameraSettings);
	//	m_CameraSettings.pTarget = dynamic_cast<CPlayer*>(m_pGameInstance->Find_GameObject_ToLayer(ENUM_CLASS(LEVEL::JUSIN), TEXT("Layer_Player")));
	//	m_CameraSettings.isChaseTarget = true;
	//	m_CameraSettings.isSyncLook = true;
	//	m_CameraSettings.isMouseFixCenter = true;
	//	m_CameraSettings.isCanTurn = true;
	//	SetCursorPos(g_iWinSizeX * 0.5f, g_iWinSizeY * 0.5f);
	//	//m_CameraSettings.vLimitDistance = _float3{ 0.f, 0.f, -0.3f };
	//	// 플레이어의 파츠로 팔 표현해보기
	//}
	//if (GetKeyState('2') & 0x8000)
	//{
	//	m_pCamera->Camera_Configure_Clear(m_CameraSettings);
	//	m_CameraSettings.pTarget = dynamic_cast<CMonster*>(m_pGameInstance->Find_GameObject_ToLayer(ENUM_CLASS(LEVEL::JUSIN), TEXT("Layer_Monster")));
	//	m_CameraSettings.isChaseTarget = true;
	//	m_CameraSettings.isCanTurn = true;
	//	m_CameraSettings.vLimitDistance = _float3{ 0.f, 1.f, -1.f };
	//}
	// UI에 값 업데이트 해줘야함
#pragma region UI

	m_pUIHp->Set_Hp();
	m_pUIArmor->Set_Armor();
	m_pUIBullets->Set_Bullets();
	m_pUICardKey->Set_CardKey();
	m_pUIInteraction->Set_Interaction();
	m_pUIUseableItems->Set_UseableItems();

	size_t iItemQueueLength = dynamic_cast<CPlayer*>(
		m_pGameInstance->Find_GameObject_ToLayer(
			ENUM_CLASS(LEVEL::JUSIN),
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
				ENUM_CLASS(LEVEL::JUSIN),
				TEXT("Layer_Player")
			))->Get_ItemText(i);

		m_pUIItemQueues[i]->Set_ItemQueue(strText);
	}

#pragma endregion

	m_pGameInstance->Check_SphereCollision(TEXT("Layer_Player"), TEXT("Layer_Items"), ENUM_CLASS(LEVEL::JUSIN), fTimeDelta);
	m_pGameInstance->Check_SphereCollision(TEXT("Layer_Player"), TEXT("Layer_Interaction_Objects"), ENUM_CLASS(LEVEL::JUSIN), fTimeDelta);
	m_pGameInstance->Check_SphereCollision(TEXT("Layer_Player"), TEXT("Layer_Melee_Attack"), ENUM_CLASS(LEVEL::JUSIN), fTimeDelta);

	m_pGameInstance->Check_AABBCollision(TEXT("Layer_Player"), TEXT("Layer_Map_Objects_AABB"), ENUM_CLASS(LEVEL::JUSIN), fTimeDelta);
	m_pGameInstance->Check_AABBCollision(TEXT("Layer_Player"), TEXT("Layer_Map_Objects_AABB_Ride"), ENUM_CLASS(LEVEL::JUSIN), fTimeDelta);
	m_pGameInstance->Check_AABBCollision(TEXT("Layer_Player"), TEXT("Layer_Map_Objects_Gate"), ENUM_CLASS(LEVEL::JUSIN), fTimeDelta);

	m_pGameInstance->Check_AABBCollision(TEXT("Layer_Monster"), TEXT("Layer_Monster"), ENUM_CLASS(LEVEL::JUSIN), fTimeDelta);
	m_pGameInstance->Check_AABBCollision(TEXT("Layer_Monster"), TEXT("Layer_Map_Objects_AABB"), ENUM_CLASS(LEVEL::JUSIN), fTimeDelta);
	m_pGameInstance->Check_AABBCollision(TEXT("Layer_Monster"), TEXT("Layer_Map_Objects_Gate"), ENUM_CLASS(LEVEL::JUSIN), fTimeDelta);
	//
	//m_pGameInstance->Check_OBBCollision(TEXT("Layer_Map_Objects_OBB_Ride"), TEXT("Layer_Player"), ENUM_CLASS(LEVEL::JUSIN), fTimeDelta);
	//m_pGameInstance->Check_OBBCollision(TEXT("Layer_Map_Objects_OBB_Ride"), TEXT("Layer_Monster"), ENUM_CLASS(LEVEL::JUSIN), fTimeDelta);
	//
	m_pGameInstance->Check_AABBCollision(TEXT("Layer_Player"), TEXT("Layer_Spawner"), ENUM_CLASS(LEVEL::JUSIN), fTimeDelta);
	m_pGameInstance->Check_RayToAABBCollision(TEXT("Layer_PlayerBullet"), TEXT("Layer_Map_Objects_AABB"), ENUM_CLASS(LEVEL::JUSIN), fTimeDelta, nullptr);
	m_pGameInstance->Check_RayToAABBCollision(TEXT("Layer_PlayerBullet"), TEXT("Layer_Map_Objects_AABB_Ride"), ENUM_CLASS(LEVEL::JUSIN), fTimeDelta, nullptr);
	m_pGameInstance->Check_RayToAABBCollision(TEXT("Layer_PlayerBullet"), TEXT("Layer_Map_Objects_Ray"), ENUM_CLASS(LEVEL::JUSIN), fTimeDelta, nullptr);
	m_pGameInstance->Check_RayToAABBCollision(TEXT("Layer_PlayerBullet"), TEXT("Layer_Monster"), ENUM_CLASS(LEVEL::JUSIN), fTimeDelta, nullptr);
	m_pGameInstance->Check_RayToAABBCollision(TEXT("Layer_Monster_Bullet"), TEXT("Layer_Player"), ENUM_CLASS(LEVEL::JUSIN), fTimeDelta, nullptr);

	m_pTerrain_Manager->Check_Landing();

	m_fTimeDelta = fTimeDelta;

	//Layer_Map_Objects_AABB
	//m_pGameInstance->Check_SphereCollision(TEXT("Layer_Player"), TEXT("Layer_Monster"), ENUM_CLASS(LEVEL::JUSIN), fTimeDelta);
	//m_pGameInstance->Check_SphereCollision(TEXT("Layer_Player"), TEXT("Layer_Monster_Bullet"), ENUM_CLASS(LEVEL::JUSIN), fTimeDelta);
	//m_pGameInstance->Check_AABBCollision(TEXT("Layer_Player"), TEXT("Layer_Monster"), ENUM_CLASS(LEVEL::JUSIN), fTimeDelta);
	//m_pGameInstance->Check_SphereCollision(TEXT("Layer_Monster"), TEXT("Layer_PlayerBullet"), ENUM_CLASS(LEVEL::JUSIN), fTimeDelta);
	//m_pGameInstance->Check_OBBCollision(TEXT("Layer_Cube"), TEXT("Layer_Player"), ENUM_CLASS(LEVEL::JUSIN), fTimeDelta);	

	// m_pGameInstance->Check_RayToAABBCollision(TEXT("Layer_PlayerBullet"), TEXT("Layer_Boss1_Upper"), ENUM_CLASS(LEVEL::JUSIN), fTimeDelta, nullptr);
	// m_pGameInstance->Check_RayToAABBCollision(TEXT("Layer_PlayerBullet"), TEXT("Layer_Boss1_Lower"), ENUM_CLASS(LEVEL::JUSIN), fTimeDelta, nullptr);
	//m_pTerrain_Manager->Check_Landing();

	m_pGameInstance->Check_RayToAABBCollision(TEXT("Layer_PlayerBullet"), TEXT("Layer_Map_Objects_AABB"), ENUM_CLASS(LEVEL::JUSIN), fTimeDelta, nullptr);
	m_pGameInstance->Check_RayToAABBCollision(TEXT("Layer_PlayerBullet"), TEXT("Layer_Map_Objects_AABB_Ride"), ENUM_CLASS(LEVEL::JUSIN), fTimeDelta, nullptr);
	m_pGameInstance->Check_RayToAABBCollision(TEXT("Layer_PlayerBullet"), TEXT("Layer_Map_Objects_Ray"), ENUM_CLASS(LEVEL::JUSIN), fTimeDelta, nullptr);
	m_pGameInstance->Check_RayToAABBCollision(TEXT("Layer_PlayerBullet"), TEXT("Layer_Monster"), ENUM_CLASS(LEVEL::JUSIN), fTimeDelta, nullptr);
	m_pGameInstance->Check_RayToAABBCollision(TEXT("Layer_Monster_Bullet"), TEXT("Layer_Player"), ENUM_CLASS(LEVEL::JUSIN), fTimeDelta, nullptr);

	if (m_pGameInstance->Key_Down(VK_F9))
	{
		CPlayer::PLAYER_INFO Desc{};

		Desc = dynamic_cast<CPlayer*>(
			m_pGameInstance->Find_GameObject_ToLayer(
				ENUM_CLASS(LEVEL::JUSIN),
				TEXT("Layer_Player")))->Get_Player_Info();


		if (FAILED(m_pGameInstance->Change_Level(
			CLevel_Loading::Create(
				m_pGraphic_Device, LEVEL::LOADING, LEVEL::BOSSFIGHT, &Desc))))
			return;
	}
	else
		m_pGameInstance->Check_AABBCollision(TEXT("Layer_Player"), TEXT("Layer_EventBox"), ENUM_CLASS(LEVEL::GAMEPLAY), fTimeDelta);
}

HRESULT CLevel_Jusin::Render()
{
	//// FPS 및 애니메이션 체크
	m_fFPSTimer += m_fTimeDelta;
	++m_iFPSCount;

	_tchar strFPS[256];

	CAnimation* pPRAnimation = dynamic_cast<CAnimation*>(m_pGameInstance->Get_Component(
		ENUM_CLASS(LEVEL::JUSIN), TEXT("Layer_Player_RightHand"),
		TEXT("Com_Animation")));

	CAnimation* pPLAnimation = dynamic_cast<CAnimation*>(m_pGameInstance->Get_Component(
		ENUM_CLASS(LEVEL::JUSIN), TEXT("Layer_Player_LeftHand"),
		TEXT("Com_Animation")));

	//CAnimation* pDLAnimation = dynamic_cast<CAnimation*>(m_pGameInstance->Get_Component(
	//	ENUM_CLASS(LEVEL::JUSIN), TEXT("Layer_Interaction_Objects"),
	//	TEXT("Com_Animation")));

	_tchar strAni[256];

	// 윈도우 타이틀에 FPS 표시
	if (pPRAnimation->Get_FrameKey() != TEXT("")
		&& pPLAnimation->Get_FrameKey() != TEXT(""))
	{
		if (m_fFPSTimer >= 1.0f) // 1초 지났을 때 FPS 계산
		{
			m_iCurrentFPS = m_iFPSCount;
			m_iFPSCount = 0;
			m_fFPSTimer = 0.f;
		}
		wsprintf(strAni, TEXT("GamePlay FPS : %d | PRAnimation : %s / %d / %s | PLAnimation : %s / %d / %s"),
			m_iCurrentFPS,
			pPRAnimation->Get_FrameKey().c_str(),
			pPRAnimation->Get_Frame_Current_Index(pPRAnimation->Get_FrameKey()),
			pPRAnimation->Get_Frame_Desc(pPRAnimation->Get_FrameKey())->bFinish ? TEXT("true") : TEXT("false"),
			pPLAnimation->Get_FrameKey().c_str(),
			pPLAnimation->Get_Frame_Current_Index(pPLAnimation->Get_FrameKey()),
			pPLAnimation->Get_Frame_Desc(pPLAnimation->Get_FrameKey())->bFinish ? TEXT("true") : TEXT("false")
		);
		SetWindowText(g_hWnd, strAni);
	}

	return S_OK;
}

HRESULT CLevel_Jusin::Ready_Objects_By_JSON()
{
	// json 파일 로드
	m_pFileMgr = CFile_Manager::Create();
	if (m_pFileMgr == nullptr)
		return E_FAIL;

	if (FAILED(m_pFileMgr->Load_Layers(LEVEL::JUSIN)))
		return E_FAIL;

	m_ObjectDescs = m_pFileMgr->Get_ObjectDescs();

	return S_OK;
}

HRESULT CLevel_Jusin::Ready_Lights()
{
	return S_OK;
}
HRESULT CLevel_Jusin::Ready_Layer_BackGround(const _wstring& strLayerTag)
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

	m_pTerrain_Manager = CTerrain_Manager::GetInstance();



	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::JUSIN), TEXT("Prototype_GameObject_Terrain"),
	//	ENUM_CLASS(LEVEL::JUSIN), strLayerTag)))
	//	return E_FAIL;
	//
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::JUSIN), TEXT("Prototype_GameObject_Terrain"),
	//	ENUM_CLASS(LEVEL::JUSIN), strLayerTag)))
	//	return E_FAIL;
	//
	//dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(ENUM_CLASS(LEVEL::JUSIN), TEXT("Layer_BackGround"), TEXT("Com_Transform"), 1))->Set_State(STATE::POSITION, _float3{ 5.f, 0.f, 5.f });
	//dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(ENUM_CLASS(LEVEL::JUSIN), TEXT("Layer_BackGround"), TEXT("Com_Transform"), 1))->Rotation(
	//	dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(ENUM_CLASS(LEVEL::JUSIN), TEXT("Layer_BackGround"), TEXT("Com_Transform"), 1))->Get_State(STATE::RIGHT), D3DXToRadian(-20.f));
	//
	//m_pTerrain_Manager = CTerrain_Manager::Create();
	//m_pTerrain_Manager->Add_Terrian(LEVEL::JUSIN);
	//
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::JUSIN), TEXT("Prototype_GameObject_Sky"),
	//	ENUM_CLASS(LEVEL::JUSIN), strLayerTag)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Jusin::Ready_Layer_Camera(const _wstring& strLayerTag)
{
	CCamera::CAMERA_DESC			CameraDesc{};
	CameraDesc.fFov = D3DXToRadian(60.0f);
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 300.f;
	CameraDesc.vEye = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(ENUM_CLASS(LEVEL::JUSIN), TEXT("Layer_Player"), TEXT("Com_Transform")))->Get_State(STATE::POSITION);
	CameraDesc.vAt = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(ENUM_CLASS(LEVEL::JUSIN), TEXT("Layer_Player"), TEXT("Com_Transform")))->Get_State(STATE::LOOK);
	CameraDesc.fSpeedPerSec = 4.f;
	CameraDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Camera"),
		ENUM_CLASS(LEVEL::JUSIN), strLayerTag, &CameraDesc)))
		return E_FAIL;

	CCamera* pCamera = dynamic_cast<CCamera*>(m_pGameInstance->Find_GameObject_ToLayer(ENUM_CLASS(LEVEL::JUSIN), TEXT("Layer_Camera")));

	//pCamera->Camera_Configure_Clear(m_CameraSettings);
	m_CameraSettings.pTarget = dynamic_cast<CPlayer*>(m_pGameInstance->Find_GameObject_ToLayer(ENUM_CLASS(LEVEL::JUSIN), TEXT("Layer_Player")));
	m_CameraSettings.isChaseTarget = true;
	m_CameraSettings.isSyncLook = true;
	m_CameraSettings.isMouseFixCenter = true;
	m_CameraSettings.isCanTurn = true;
	//m_CameraSettings.vLimitDistance = _float3{ 0.f, 0.f, -1.f };
	SetCursorPos(g_iWinSizeX * 0.5f, g_iWinSizeY * 0.5f);

	pCamera->Camera_Configure(m_CameraSettings);

	return S_OK;
}

HRESULT CLevel_Jusin::Ready_Layer_Player(const _wstring& strLayerTag)
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

	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::JUSIN), TEXT("Prototype_GameObject_Player"),
	//	ENUM_CLASS(LEVEL::JUSIN), strLayerTag)))
	//	return E_FAIL;

	//for (int i = 0; i < 100; ++i)
	//{
	//	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::JUSIN), TEXT("Prototype_GameObject_Player"),
	//	ENUM_CLASS(LEVEL::JUSIN), strLayerTag)))
	//		return E_FAIL;
	//}

	m_pTerrain_Manager->Add_LandObject(LEVEL::JUSIN, TEXT("Layer_Player"));

	return S_OK;
}

HRESULT CLevel_Jusin::Ready_Layer_Spawner(const _wstring& strLayerTag)
{
	/*for (size_t i = 0; i < 30; i++)
	{
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::JUSIN), TEXT("Prototype_GameObject_Spawner"),
			ENUM_CLASS(LEVEL::JUSIN), strLayerTag, _float3(0.5f, 0.5f, 0.5f))))
			return E_FAIL;
	}*/

	/*CSpawner::SPAWNER_DESC desc;
	desc.vPos = _float3(22.5f, 1.f, 12.5f);
	desc.idx = 0;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::JUSIN), TEXT("Prototype_GameObject_Spawner"),
		ENUM_CLASS(LEVEL::JUSIN), strLayerTag, &desc)))
		return E_FAIL;

	desc.vPos = _float3(12.5f, 1.f, 5.5f);
	desc.idx = 1;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::JUSIN), TEXT("Prototype_GameObject_Spawner"),
		ENUM_CLASS(LEVEL::JUSIN), strLayerTag, &desc)))
		return E_FAIL;

	desc.vPos = _float3(15.5f, 1.f, 8.5f);
	desc.idx = 2;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::JUSIN), TEXT("Prototype_GameObject_Spawner"),
		ENUM_CLASS(LEVEL::JUSIN), strLayerTag, &desc)))
		return E_FAIL;

	desc.vPos = _float3(17.5f, 1.f, 10.5f);
	desc.idx = 3;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::JUSIN), TEXT("Prototype_GameObject_Spawner"),
		ENUM_CLASS(LEVEL::JUSIN), strLayerTag, &desc)))
		return E_FAIL;

	desc.vPos = _float3(20.5f, 1.f, 13.5f);
	desc.idx = 0;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::JUSIN), TEXT("Prototype_GameObject_Spawner"),
		ENUM_CLASS(LEVEL::JUSIN), strLayerTag, &desc)))
		return E_FAIL;

	desc.vPos = _float3(23.5f, 1.f, 17.5f);
	desc.idx = 1;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::JUSIN), TEXT("Prototype_GameObject_Spawner"),
		ENUM_CLASS(LEVEL::JUSIN), strLayerTag, &desc)))
		return E_FAIL;

	desc.vPos = _float3(26.5f, 1.f, 22.5f);
	desc.idx = 2;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::JUSIN), TEXT("Prototype_GameObject_Spawner"),
		ENUM_CLASS(LEVEL::JUSIN), strLayerTag, &desc)))
		return E_FAIL;

	desc.vPos = _float3(16.5f, 1.f, 30.5f);
	desc.idx = 3;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::JUSIN), TEXT("Prototype_GameObject_Spawner"),
		ENUM_CLASS(LEVEL::JUSIN), strLayerTag, &desc)))
		return E_FAIL;*/

		// 맵 우측 룸 스폰
		//CSpawner::SPAWNER_DESC desc;
		//desc.vPos = _float3(45.5f, 1.f, 28.5f);
		//desc.idx = 0;
		//desc.isLeft = false;
		//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::JUSIN), TEXT("Prototype_GameObject_Spawner"),
		//	ENUM_CLASS(LEVEL::JUSIN), strLayerTag, &desc)))
		//	return E_FAIL;

		//// 맵 좌측 룸 스폰
		//desc.vPos = _float3(45.5f, 1.f, 63.5f);
		//desc.idx = 0;
		//desc.isLeft = true;
		//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::JUSIN), TEXT("Prototype_GameObject_Spawner"),
		//	ENUM_CLASS(LEVEL::JUSIN), strLayerTag, &desc)))
	return E_FAIL;
}

HRESULT CLevel_Jusin::Ready_Layer_Monster(const _wstring& strLayerTag)
{
	for (auto& iter : m_ObjectDescs->find(strLayerTag)->second)
	{
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(iter.iProtoLevel), iter.strProto,
			ENUM_CLASS(iter.iLayerLevel), iter.strLayer, &iter)))
			return E_FAIL;

		/*dynamic_cast<CTransform*>(
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
			)->Set_State(STATE::POSITION, iter.matWorld.m[3]);*/
	}

	/*for (size_t i = 0; i < 2; i++)
	{
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::JUSIN), TEXT("Prototype_GameObject_Monster_EliteSoldier"),
			ENUM_CLASS(LEVEL::JUSIN), strLayerTag)))
			return E_FAIL;
	}*/

	//for (size_t i = 0; i < 2; i++)
	//{
	//	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::JUSIN), TEXT("Prototype_GameObject_Monster_Spider"),
	//		ENUM_CLASS(LEVEL::JUSIN), strLayerTag)))
	//		return E_FAIL;
	//}

	//for (size_t i = 0; i < 10; i++)
	//{
	//	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::JUSIN), TEXT("Prototype_GameObject_Monster_Zombie"),
	//		ENUM_CLASS(LEVEL::JUSIN), strLayerTag)))
	//		return E_FAIL;
	//}

	/*for (size_t i = 0; i < 1; i++)
	{
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::JUSIN), TEXT("Prototype_GameObject_Monster_Soldier"),
			ENUM_CLASS(LEVEL::JUSIN), strLayerTag)))
			return E_FAIL;
	}*/

	/*for (size_t i = 0; i < 1; i++)
	{
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::JUSIN), TEXT("Prototype_GameObject_Monster_Boss"),
			ENUM_CLASS(LEVEL::JUSIN), strLayerTag)))
			return E_FAIL;
	}*/

	m_pTerrain_Manager->Add_LandObject(LEVEL::JUSIN, TEXT("Layer_Monster"));

	return S_OK;
}

HRESULT CLevel_Jusin::Ready_Layer_UI(const _wstring& strLayerTag)
{
	_wstring ws{};

	/* 체력 */
	CUIObject::UIOBJECT_DESC Desc_Hp{};
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Find_GameObject_ToLayer(ENUM_CLASS(LEVEL::JUSIN), TEXT("Layer_Player")));
	Safe_AddRef(pPlayer);

	ws = to_wstring(pPlayer->Get_Player_Info().iHp);

	// UI전체 크기 및 위치
	Desc_Hp.iTextLength = 3;
	Desc_Hp.fSizeX = 70.f + (70.f * 2);
	Desc_Hp.fSizeY = 70.f;
	Desc_Hp.fX = 10.f;
	Desc_Hp.fY = g_iWinSizeY - (Desc_Hp.fSizeY * 0.5f) - 10.f;
	Desc_Hp.iLayerLevelIndex = ENUM_CLASS(LEVEL::JUSIN);
	Desc_Hp.strLayerTag = strLayerTag;
	Desc_Hp.strFontType = TEXT("Default");

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::JUSIN), TEXT("Prototype_GameObject_UIHp"),
		ENUM_CLASS(LEVEL::JUSIN), Desc_Hp.strLayerTag, &Desc_Hp)))
		return E_FAIL;

	m_pUIHp = dynamic_cast<CUIHp*>(m_pGameInstance->Find_GameObject_ToLayer(ENUM_CLASS(LEVEL::JUSIN), Desc_Hp.strLayerTag));
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
	Desc_Armor.iLayerLevelIndex = ENUM_CLASS(LEVEL::JUSIN);
	Desc_Armor.strLayerTag = strLayerTag;
	Desc_Armor.strFontType = TEXT("Default");

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::JUSIN), TEXT("Prototype_GameObject_UIArmor"),
		ENUM_CLASS(LEVEL::JUSIN), Desc_Armor.strLayerTag, &Desc_Armor)))
		return E_FAIL;

	m_pUIArmor = dynamic_cast<CUIArmor*>(m_pGameInstance->Find_GameObject_ToLayer(ENUM_CLASS(LEVEL::JUSIN), Desc_Armor.strLayerTag));
	Safe_AddRef(m_pUIArmor);

	/* 총알 */
	CUIObject::UIOBJECT_DESC Desc_Bullets{};

	ws = to_wstring(pPlayer->Get_Player_Info().iBullets);

	Desc_Bullets.iTextLength = 6;
	Desc_Bullets.fSizeX = 70.f + (70.f * Desc_Bullets.iTextLength);
	Desc_Bullets.fSizeY = 70.f;
	Desc_Bullets.fX = g_iWinSizeX - 10.f;
	Desc_Bullets.fY = g_iWinSizeY - (Desc_Bullets.fSizeY * 0.5f) - 10.f;
	Desc_Bullets.iLayerLevelIndex = ENUM_CLASS(LEVEL::JUSIN);
	Desc_Bullets.strLayerTag = strLayerTag;
	Desc_Bullets.strFontType = TEXT("Default");

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::JUSIN), TEXT("Prototype_GameObject_UIBullets"),
		ENUM_CLASS(LEVEL::JUSIN), Desc_Bullets.strLayerTag, &Desc_Bullets)))
		return E_FAIL;

	m_pUIBullets = dynamic_cast<CUIBullets*>(m_pGameInstance->Find_GameObject_ToLayer(ENUM_CLASS(LEVEL::JUSIN), strLayerTag));
	Safe_AddRef(m_pUIBullets);

	/* 상호작용 키 */
	CUIObject::UIOBJECT_DESC Desc_Interaction{};

	Desc_Interaction.iTextLength = wcslen(TEXT("Press [E] Key"));
	Desc_Interaction.fSizeX = 30.f * Desc_Interaction.iTextLength;
	Desc_Interaction.fSizeY = 30.f;
	Desc_Interaction.fX = g_iWinSizeX * 0.5f - (Desc_Interaction.fSizeX * 0.5f);
	Desc_Interaction.fY = g_iWinSizeY * 0.8f;
	Desc_Interaction.iLayerLevelIndex = ENUM_CLASS(LEVEL::JUSIN);
	Desc_Interaction.strLayerTag = strLayerTag;
	Desc_Interaction.strFontType = TEXT("Primary");

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::JUSIN), TEXT("Prototype_GameObject_UIInteraction"),
		ENUM_CLASS(LEVEL::JUSIN), Desc_Interaction.strLayerTag, &Desc_Interaction)))
		return E_FAIL;

	m_pUIInteraction = dynamic_cast<CUIInteraction*>(m_pGameInstance->Find_GameObject_ToLayer(ENUM_CLASS(LEVEL::JUSIN), strLayerTag));
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
		Desc_ItemQueue.iLayerLevelIndex = ENUM_CLASS(LEVEL::JUSIN);
		Desc_ItemQueue.strLayerTag = strLayerTag;
		Desc_ItemQueue.strFontType = TEXT("Primary");

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::JUSIN), TEXT("Prototype_GameObject_UIItemQueue"),
			ENUM_CLASS(LEVEL::JUSIN), Desc_ItemQueue.strLayerTag, &Desc_ItemQueue)))
			return E_FAIL;

		CUIItemQueue* pUIItemQueue = dynamic_cast<CUIItemQueue*>(m_pGameInstance->Find_GameObject_ToLayer(ENUM_CLASS(LEVEL::JUSIN), Desc_ItemQueue.strLayerTag));
		Safe_AddRef(pUIItemQueue);

		pUIItemQueue->Set_ItemQueue(TEXT(" "));

		m_pUIItemQueues.push_back(pUIItemQueue);
	}

	/* 조준선 */
	CUIObject::UIOBJECT_DESC Desc_Aim{};

	Desc_Aim.fSizeX = 30.f;
	Desc_Aim.fSizeY = 30.f;
	Desc_Aim.iLayerLevelIndex = ENUM_CLASS(LEVEL::JUSIN);
	Desc_Aim.fX = g_iWinSizeX * 0.5f - (Desc_Aim.fSizeX * 0.5f);
	Desc_Aim.fY = g_iWinSizeY * 0.5f - (Desc_Aim.fSizeY * 0.5f);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::JUSIN), TEXT("Prototype_GameObject_UIAim"),
		ENUM_CLASS(LEVEL::JUSIN), strLayerTag, &Desc_Aim)))
		return E_FAIL;

	m_pUIAim = dynamic_cast<CUIAim*>(m_pGameInstance->Find_GameObject_ToLayer(ENUM_CLASS(LEVEL::JUSIN), strLayerTag));
	Safe_AddRef(m_pUIAim);

	/* 카드키 */
	CUIObject::UIOBJECT_DESC Desc_CardKey{};

	Desc_CardKey.fSizeX = 92.f;
	Desc_CardKey.fSizeY = 56.f;
	Desc_CardKey.iLayerLevelIndex = ENUM_CLASS(LEVEL::JUSIN);
	Desc_CardKey.fX = g_iWinSizeX - 20.f - (Desc_CardKey.fSizeX * 0.5f);
	Desc_CardKey.fY = g_iWinSizeY - 100.f - (Desc_CardKey.fSizeY * 0.5f);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::JUSIN), TEXT("Prototype_GameObject_UICardKey"),
		ENUM_CLASS(LEVEL::JUSIN), strLayerTag, &Desc_CardKey)))
		return E_FAIL;

	m_pUICardKey = dynamic_cast<CUICardKey*>(m_pGameInstance->Find_GameObject_ToLayer(ENUM_CLASS(LEVEL::JUSIN), strLayerTag));
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
	Desc_UseableItems.iLayerLevelIndex = ENUM_CLASS(LEVEL::JUSIN);
	Desc_UseableItems.strLayerTag = strLayerTag;
	Desc_UseableItems.strFontType = TEXT("Default");

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::JUSIN), TEXT("Prototype_GameObject_UIUseableItems"),
		ENUM_CLASS(LEVEL::JUSIN), strLayerTag, &Desc_UseableItems)))
		return E_FAIL;

	m_pUIUseableItems = dynamic_cast<CUIUseableItems*>(m_pGameInstance->Find_GameObject_ToLayer(ENUM_CLASS(LEVEL::JUSIN), strLayerTag));
	Safe_AddRef(m_pUIUseableItems);

	Safe_Release(pPlayer);

	return S_OK;
}

/*
이펙트 매니저 세팅만 게임 플레이에서,

이펙트들의 생성은 이펙트 매니저의 Initialize 내부에서 동작합니다

추후 이펙트 추가하시려면 참고하세요
*/
HRESULT CLevel_Jusin::Ready_Layer_Effect(const _wstring& strLayerTag)
{
	m_pEffect_Manager = CEffect_Manager::GetInstance();
	m_pEffect_Manager->Initialize(LEVEL::JUSIN);
	Safe_AddRef(m_pEffect_Manager);

	return S_OK;
}

/*
이펙트가 아닌 파티클 시스템입니다.
건드리시면 안돼요.

추가하시고 싶은 파티클이 있다면 일단 디코 주세요
*/
HRESULT CLevel_Jusin::Ready_Layer_Particle(const _wstring& strLayerTag)
{
	m_pParticle_Manager = CParticle_Manager::GetInstance();
	m_pParticle_Manager->Initialize(LEVEL::JUSIN);
	Safe_AddRef(m_pParticle_Manager);

	return S_OK;

	//ParticleDesc.iNumParticles = 1000;
	//ParticleDesc.vBoundaryMin = _float3(-10.f, -5.f, -10.f);
	//ParticleDesc.vBoundaryMax = _float3(10.f, 10.f, 10.f);

	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::JUSIN), TEXT("Prototype_GameObject_Snow"),
	//	ENUM_CLASS(LEVEL::JUSIN), strLayerTag, &ParticleDesc)))
	//	return E_FAIL;
}

HRESULT CLevel_Jusin::Ready_Layer_Items(const _wstring& strLayerTag)
{
	for (auto& iter : m_ObjectDescs->find(strLayerTag)->second)
	{
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(iter.iProtoLevel), iter.strProto,
			ENUM_CLASS(iter.iLayerLevel), iter.strLayer, &iter)))
			return E_FAIL;

		/*dynamic_cast<CTransform*>(
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
			)->Set_State(STATE::POSITION, iter.matWorld.m[3]);*/
	}

	//m_pTerrain_Manager->Add_LandObject(LEVEL::JUSIN, TEXT("Layer_Items"));

	/*if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::JUSIN), TEXT("Prototype_GameObject_Item_Healpack"),
			ENUM_CLASS(LEVEL::JUSIN), strLayerTag)))
			return E_FAIL;*/

			//
			//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::JUSIN), TEXT("Prototype_GameObject_Item_Healpack"),
			//	ENUM_CLASS(LEVEL::JUSIN), strLayerTag)))
			//	return E_FAIL;
			//
			//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::JUSIN), TEXT("Prototype_GameObject_Item_Pistol_Bullet"),
			//	ENUM_CLASS(LEVEL::JUSIN), strLayerTag)))
			//	return E_FAIL;
			//
			//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::JUSIN), TEXT("Prototype_GameObject_Item_ShootGun_Bullet"),
			//	ENUM_CLASS(LEVEL::JUSIN), strLayerTag)))
			//	return E_FAIL;
			//
			/*if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::JUSIN), TEXT("Prototype_GameObject_Item_CardKey"),
				ENUM_CLASS(LEVEL::JUSIN), strLayerTag)))
				return E_FAIL;*/

	return S_OK;
}

HRESULT CLevel_Jusin::Ready_Layer_EventBox(const _wstring& strLayerTag)
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

		///* 콜라이더 꺼내와서 세팅 */
		//CBoxCollider* pCollider = dynamic_cast<CBoxCollider*>(m_pGameInstance->Get_Component(ENUM_CLASS(iter.iLayerLevel), iter.strLayer, TEXT("Com_BoxCollider"), iter.iObjectID));
		//pCollider->Set_Matrix(iter.matWorld);
	}

	return S_OK;
}

HRESULT CLevel_Jusin::Ready_Layer_Map_Objects_AABB(const _wstring& strLayerTag)
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

HRESULT CLevel_Jusin::Ready_Layer_Map_Objects_AABB_Ride(const _wstring& strLayerTag)
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

		CBoxCollider* pCollider = dynamic_cast<CBoxCollider*>(m_pGameInstance->Get_Component(ENUM_CLASS(iter.iLayerLevel), iter.strLayer, TEXT("Com_BoxCollider"), iter.iObjectID));
		pCollider->Set_Matrix(iter.matWorld);
	}

	/*if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::JUSIN), TEXT("Prototype_GameObject_Map_MachineGun_Bullet_Box"),
		ENUM_CLASS(LEVEL::JUSIN), strLayerTag)))
		return E_FAIL;*/

		/*dynamic_cast<CGameObject*>(m_pGameInstance->Find_GameObject_ToLayer(
			ENUM_CLASS(LEVEL::JUSIN), TEXT("Layer_Map_Objects_AABB_Ride")))->Set_ID(100);

		CBoxCollider* pCollider2 = dynamic_cast<CBoxCollider*>(m_pGameInstance->Get_Component(ENUM_CLASS(LEVEL::JUSIN), TEXT("Layer_Map_Objects_AABB_Ride"), TEXT("Com_BoxCollider")));

		pCollider2->Set_Matrix(
			*dynamic_cast<CTransform*>(
				m_pGameInstance->Get_GameObject_By_ID(
					ENUM_CLASS(LEVEL::JUSIN), TEXT("Layer_Map_Objects_AABB_Ride"), 100))->Get_WorldMatrixPtr()
		);*/

	return S_OK;
}

HRESULT CLevel_Jusin::Ready_Layer_Map_Objects_OBB(const _wstring& strLayerTag)
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

HRESULT CLevel_Jusin::Ready_Layer_Map_Objects_OBB_Ride(const _wstring& strLayerTag)
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

		CBoxCollider* pCollider = dynamic_cast<CBoxCollider*>(m_pGameInstance->Get_Component(ENUM_CLASS(iter.iLayerLevel), iter.strLayer, TEXT("Com_BoxCollider"), iter.iObjectID));
		pCollider->Set_Scale({ 0.9f, 0.9f, 0.9f });
	}

	return S_OK;
}

HRESULT CLevel_Jusin::Ready_Layer_Map_Objects_Ray(const _wstring& strLayerTag)
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

	/*_float3 vPos = { 3.f, 0.3f, 3.f };
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::JUSIN), TEXT("Prototype_GameObject_TrashBox"),
		ENUM_CLASS(LEVEL::JUSIN), strLayerTag, &vPos)))
		return E_FAIL;*/

	return S_OK;
}

HRESULT CLevel_Jusin::Ready_Layer_Map_Objects_Ride(const _wstring& strLayerTag)
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

	//m_pTerrain_Manager->Add_Cube(LEVEL::JUSIN);

	return S_OK;
}

HRESULT CLevel_Jusin::Ready_Layer_Map_Objects_Gate(const _wstring& strLayerTag)
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

		CBoxCollider* pCollider = dynamic_cast<CBoxCollider*>(m_pGameInstance->Get_Component(ENUM_CLASS(iter.iLayerLevel), iter.strLayer, TEXT("Com_BoxCollider"), iter.iObjectID));
		pCollider->Set_Matrix(iter.matWorld);
	}

	return S_OK;
}

HRESULT CLevel_Jusin::Ready_Layer_Map_Objects_Deco(const _wstring& strLayerTag)
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

HRESULT CLevel_Jusin::Ready_Layer_Interaction_Objects(const _wstring& strLayerTag)
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

	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::JUSIN), TEXT("Prototype_GameObject_Interaction_DoorLock"),
	//	ENUM_CLASS(LEVEL::JUSIN), strLayerTag)))
	//	return E_FAIL;
	//
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::JUSIN), TEXT("Prototype_GameObject_Interaction_Lever"),
	//	ENUM_CLASS(LEVEL::JUSIN), strLayerTag)))
	//	return E_FAIL;

	//m_pTerrain_Manager->Add_Cube(LEVEL::JUSIN);

	return S_OK;
}

HRESULT CLevel_Jusin::Ready_Layer_Bullet(const _wstring& strLayerTag)
{
	m_pBullet_Manager = CBullet_Manager::GetInstance();
	m_pBullet_Manager->Initialize();
	Safe_AddRef(m_pBullet_Manager);

	return S_OK;
}

CLevel_Jusin* CLevel_Jusin::Create(LPDIRECT3DDEVICE9 pGraphic_Device, LEVEL eLevelID, void* pArg)
{
	CLevel_Jusin* pInstance = new CLevel_Jusin(pGraphic_Device, eLevelID);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CLevel_Jusin");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Jusin::Free()
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
