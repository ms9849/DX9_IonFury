#include "Level_GamePlay.h"
#include "GameInstance.h"
#include "UIHp.h"
#include "UIBullets.h"
#include "UIInteraction.h"
#include "UIAim.h"
#include "UIArmor.h"
#include "UIItemQueue.h"
#include "UICardKey.h"
#include "Terrain.h"
#include "Terrain_Manager.h"
#include "Effect_Manager.h"

#include "Player.h"

CLevel_GamePlay::CLevel_GamePlay(LPDIRECT3DDEVICE9 pGraphic_Device, LEVEL eLevelID)
	: CLevel { pGraphic_Device, ENUM_CLASS(eLevelID)}	
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Cube(TEXT("Layer_Cube"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Items(TEXT("Layer_Items"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Map_Objects(TEXT("Layer_Map_Objects"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Interaction_Objects(TEXT("Layer_Interaction_Objects"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_GamePlay::Update(_float fTimeDelta)
{
	//if (GetKeyState('1') & 0x8000)
	//{
	//	m_pCamera->Camera_Configure_Clear(m_CameraSettings);
	//	m_CameraSettings.pTarget = dynamic_cast<CPlayer*>(m_pGameInstance->Find_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Player")));
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
	//	m_CameraSettings.pTarget = dynamic_cast<CMonster*>(m_pGameInstance->Find_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Monster")));
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

	size_t iItemQueueLength = dynamic_cast<CPlayer*>(
		m_pGameInstance->Find_GameObject_ToLayer(
			ENUM_CLASS(LEVEL::GAMEPLAY),
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
				ENUM_CLASS(LEVEL::GAMEPLAY),
				TEXT("Layer_Player")
			))->Get_ItemText(i);

		m_pUIItemQueues[i]->Set_ItemQueue(strText);
	}
	
#pragma endregion

	//m_pGameInstance->Check_OBBCollision(TEXT("Layer_Cube"), TEXT("Layer_Player"), ENUM_CLASS(LEVEL::GAMEPLAY), fTimeDelta);
	m_pGameInstance->Check_SphereCollision(TEXT("Layer_Player"), TEXT("Layer_Items"), ENUM_CLASS(LEVEL::GAMEPLAY), fTimeDelta);
	m_pGameInstance->Check_SphereCollision(TEXT("Layer_Player"), TEXT("Layer_Interaction_Objects"), ENUM_CLASS(LEVEL::GAMEPLAY), fTimeDelta);
	m_pGameInstance->Check_SphereCollision(TEXT("Layer_Player"), TEXT("Layer_Monster_Bullet"), ENUM_CLASS(LEVEL::GAMEPLAY), fTimeDelta);
	m_pGameInstance->Check_SphereCollision(TEXT("Layer_Player"), TEXT("Layer_Melee_Attack"), ENUM_CLASS(LEVEL::GAMEPLAY), fTimeDelta);
	//m_pGameInstance->Check_SphereCollision(TEXT("Layer_Player"), TEXT("Layer_Monster"), ENUM_CLASS(LEVEL::GAMEPLAY), fTimeDelta);
	m_pGameInstance->Check_SphereCollision(TEXT("Layer_Monster"), TEXT("Layer_PlayerBullet"), ENUM_CLASS(LEVEL::GAMEPLAY), fTimeDelta);
	m_pTerrain_Manager->Check_Landing();

	m_fTimeDelta = fTimeDelta;
}

HRESULT CLevel_GamePlay::Render()
{
	//// FPS 및 애니메이션 체크
	m_fFPSTimer += m_fTimeDelta;
	++m_iFPSCount;
	
	_tchar strFPS[256];

	CAnimation* pPRAnimation = dynamic_cast<CAnimation*>(m_pGameInstance->Get_Component(
		ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Player_RightHand"),
		TEXT("Com_Animation")));

	CAnimation* pPLAnimation = dynamic_cast<CAnimation*>(m_pGameInstance->Get_Component(
		ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Player_LeftHand"),
		TEXT("Com_Animation")));

	CAnimation* pDLAnimation = dynamic_cast<CAnimation*>(m_pGameInstance->Get_Component(
		ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Interaction_Objects"),
		TEXT("Com_Animation")));

	Safe_AddRef(pPRAnimation);
	Safe_AddRef(pPLAnimation);
	Safe_AddRef(pDLAnimation);
	_tchar strAni[256];

	// 윈도우 타이틀에 FPS 표시
	if (pPRAnimation->Get_FrameKey() != TEXT("")
		&& pPLAnimation->Get_FrameKey() != TEXT("")
		&& pDLAnimation->Get_FrameKey() != TEXT(""))
	{
		if (m_fFPSTimer >= 1.0f) // 1초 지났을 때 FPS 계산
		{
			m_iCurrentFPS = m_iFPSCount;
			m_iFPSCount = 0;
			m_fFPSTimer = 0.f;
		}
		//
		wsprintf(strAni, TEXT("GamePlay FPS : %d | PRAnimation : %s / %d / %s | PLAnimation : %s / %d / %s | DLAnimation : %s / %d / %s"),
			m_iCurrentFPS,
			pPRAnimation->Get_FrameKey().c_str(),
			pPRAnimation->Get_Frame_Current_Index(pPRAnimation->Get_FrameKey()),
			pPRAnimation->Get_Frame_Desc(pPRAnimation->Get_FrameKey())->bFinish ? TEXT("true") : TEXT("false"),
			pPLAnimation->Get_FrameKey().c_str(),
			pPLAnimation->Get_Frame_Current_Index(pPLAnimation->Get_FrameKey()),
			pPLAnimation->Get_Frame_Desc(pPLAnimation->Get_FrameKey())->bFinish ? TEXT("true") : TEXT("false"),
			pDLAnimation->Get_FrameKey().c_str(),
			pDLAnimation->Get_Frame_Current_Index(pDLAnimation->Get_FrameKey()),
			pDLAnimation->Get_Frame_Desc(pDLAnimation->Get_FrameKey())->bFinish ? TEXT("true") : TEXT("false")
		);
		SetWindowText(g_hWnd, strAni);
	}

	Safe_Release(pPRAnimation);
	Safe_Release(pPLAnimation);
	Safe_Release(pDLAnimation);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Lights()
{
	return S_OK;
}
HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Terrain"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Terrain"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;
	
	dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_BackGround"), TEXT("Com_Transform"), 1))->Set_State(STATE::POSITION, _float3{ 5.f, 0.f, 5.f });
	dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_BackGround"), TEXT("Com_Transform"), 1))->Rotation(
		dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_BackGround"), TEXT("Com_Transform"), 1))->Get_State(STATE::RIGHT), D3DXToRadian(-20.f));
	
	m_pTerrain_Manager = CTerrain_Manager::Create();
	m_pTerrain_Manager->Add_Terrian(LEVEL::GAMEPLAY);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Sky"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const _wstring& strLayerTag)
{
	CCamera::CAMERA_DESC			CameraDesc{};
	CameraDesc.fFov = D3DXToRadian(60.0f);
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 300.f;
	CameraDesc.vEye = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Player"), TEXT("Com_Transform")))->Get_State(STATE::POSITION);
	CameraDesc.vAt = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Player"), TEXT("Com_Transform")))->Get_State(STATE::LOOK);
	CameraDesc.fSpeedPerSec = 4.f;
	CameraDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Camera"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, &CameraDesc)))
		return E_FAIL;

	CCamera* pCamera = dynamic_cast<CCamera*>(m_pGameInstance->Find_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Camera")));

	//pCamera->Camera_Configure_Clear(m_CameraSettings);
	m_CameraSettings.pTarget = dynamic_cast<CPlayer*>(m_pGameInstance->Find_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Player")));
	m_CameraSettings.isChaseTarget = true;
	m_CameraSettings.isSyncLook = true;
	m_CameraSettings.isMouseFixCenter = true;
	m_CameraSettings.isCanTurn = true;
	//m_CameraSettings.vLimitDistance = _float3{ 0.f, 0.f, -1.f };
	SetCursorPos(g_iWinSizeX * 0.5f, g_iWinSizeY * 0.5f);

	pCamera->Camera_Configure(m_CameraSettings);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Player"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	m_pTerrain_Manager->Add_LandObject(LEVEL::GAMEPLAY, TEXT("Layer_Player"));

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const _wstring& strLayerTag)
{
	for (size_t i = 0; i < 3; i++)
	{
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Monster_Soldier"),
			ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
			return E_FAIL;
	}

	for (size_t i = 0; i < 3; i++)
	{
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Monster_Zombie"),
			ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
			return E_FAIL;
	}

	for (size_t i = 0; i < 3; i++)
	{
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Monster_Spider"),
			ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
			return E_FAIL;
	}

	for (size_t i = 0; i < 1; i++)
	{
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Monster_Boss"),
			ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
			return E_FAIL;
	}

	m_pTerrain_Manager->Add_LandObject(LEVEL::GAMEPLAY, TEXT("Layer_Monster"));

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI(const _wstring& strLayerTag)
{
	_wstring ws{};

	/* 체력 */
	CUIObject::UIOBJECT_DESC Desc_Hp{};
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Find_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Player")));

	ws = to_wstring(pPlayer->Get_Player_Info().iHp);

	// UI전체 크기 및 위치
	Desc_Hp.iTextLength = 3;
	Desc_Hp.fSizeX = 50.f + (50.f * Desc_Hp.iTextLength);
	Desc_Hp.fSizeY = 50.f;
	Desc_Hp.fX = 10.f;
	Desc_Hp.fY = g_iWinSizeY - (Desc_Hp.fSizeY * 0.5f) - 10.f;
	Desc_Hp.iLayerLevelIndex = ENUM_CLASS(LEVEL::GAMEPLAY);
	Desc_Hp.strLayerTag = strLayerTag;
	Desc_Hp.strFontType = TEXT("Default");

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UIHp"),
		ENUM_CLASS(LEVEL::GAMEPLAY), Desc_Hp.strLayerTag, &Desc_Hp)))
		return E_FAIL;

	m_pUIHp = dynamic_cast<CUIHp*>(m_pGameInstance->Find_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), Desc_Hp.strLayerTag));

	/* 방어력 */
	CUIObject::UIOBJECT_DESC Desc_Armor{};

	ws = to_wstring(pPlayer->Get_Player_Info().iArmor);

	// UI전체 크기 및 위치
	Desc_Armor.iTextLength = 3;
	Desc_Armor.fSizeX = 50.f + (50.f * Desc_Hp.iTextLength);
	Desc_Armor.fSizeY = 50.f;
	Desc_Armor.fX = Desc_Hp.fX + Desc_Hp.fSizeX + 50.f;
	Desc_Armor.fY = g_iWinSizeY - (Desc_Hp.fSizeY * 0.5f) - 10.f;
	Desc_Armor.iLayerLevelIndex = ENUM_CLASS(LEVEL::GAMEPLAY);
	Desc_Armor.strLayerTag = strLayerTag;
	Desc_Armor.strFontType = TEXT("Default");

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UIArmor"),
		ENUM_CLASS(LEVEL::GAMEPLAY), Desc_Armor.strLayerTag, &Desc_Armor)))
		return E_FAIL;

	m_pUIArmor = dynamic_cast<CUIArmor*>(m_pGameInstance->Find_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), Desc_Armor.strLayerTag));

	/* 총알 */
	CUIObject::UIOBJECT_DESC Desc_Bullets{};

	ws = to_wstring(pPlayer->Get_Player_Info().iBullets);

	Desc_Bullets.iTextLength = 6;
	Desc_Bullets.fSizeX = 50.f + (50.f * Desc_Bullets.iTextLength);
	Desc_Bullets.fSizeY = 50.f;
	Desc_Bullets.fX = g_iWinSizeX - 10.f;
	Desc_Bullets.fY = g_iWinSizeY - (Desc_Bullets.fSizeY * 0.5f) - 10.f;
	Desc_Bullets.iLayerLevelIndex = ENUM_CLASS(LEVEL::GAMEPLAY);
	Desc_Bullets.strLayerTag = strLayerTag;
	Desc_Bullets.strFontType = TEXT("Default");

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UIBullets"),
		ENUM_CLASS(LEVEL::GAMEPLAY), Desc_Bullets.strLayerTag, &Desc_Bullets)))
		return E_FAIL;

	m_pUIBullets = dynamic_cast<CUIBullets*>(m_pGameInstance->Find_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag));

	/* 상호작용 키 */
	CUIObject::UIOBJECT_DESC Desc_Interaction{};

	Desc_Interaction.iTextLength = wcslen(TEXT("Press [E] Key"));
	Desc_Interaction.fSizeX = 30.f * Desc_Interaction.iTextLength;
	Desc_Interaction.fSizeY = 30.f;
	Desc_Interaction.fX = g_iWinSizeX * 0.5f - (Desc_Interaction.fSizeX * 0.5f);
	Desc_Interaction.fY = g_iWinSizeY * 0.8f;
	Desc_Interaction.iLayerLevelIndex = ENUM_CLASS(LEVEL::GAMEPLAY);
	Desc_Interaction.strLayerTag = strLayerTag;
	Desc_Interaction.strFontType = TEXT("Primary");

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UIInteraction"),
		ENUM_CLASS(LEVEL::GAMEPLAY), Desc_Interaction.strLayerTag, &Desc_Interaction)))
		return E_FAIL;

	m_pUIInteraction = dynamic_cast<CUIInteraction*>(m_pGameInstance->Find_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag));

	/* 아이템 스택 */
	for (size_t i = 0; i < 20; ++i)
	{
		CUIObject::UIOBJECT_DESC Desc_ItemQueue{};

		Desc_ItemQueue.iTextLength = 50;
		Desc_ItemQueue.fSizeX = 16.f * Desc_ItemQueue.iTextLength;
		Desc_ItemQueue.fSizeY = 16.f;
		Desc_ItemQueue.fX = 10.f;
		Desc_ItemQueue.fY = 10.f + (Desc_ItemQueue.fSizeY * 0.5f) + (i * 20);
		Desc_ItemQueue.iLayerLevelIndex = ENUM_CLASS(LEVEL::GAMEPLAY);
		Desc_ItemQueue.strLayerTag = strLayerTag;
		Desc_ItemQueue.strFontType = TEXT("Primary");

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UIItemQueue"),
			ENUM_CLASS(LEVEL::GAMEPLAY), Desc_ItemQueue.strLayerTag, &Desc_ItemQueue)))
			return E_FAIL;

		CUIItemQueue* pUIItemQueue = dynamic_cast<CUIItemQueue*>(m_pGameInstance->Find_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), Desc_ItemQueue.strLayerTag));
		Safe_AddRef(pUIItemQueue);

		pUIItemQueue->Set_ItemQueue(TEXT(" "));

		m_pUIItemQueues.push_back(pUIItemQueue);
	}

	/* 조준선 */
	CUIObject::UIOBJECT_DESC Desc_Aim{};

	Desc_Aim.fSizeX = 30.f;
	Desc_Aim.fSizeY = 30.f;
	Desc_Aim.fX = g_iWinSizeX * 0.5f - (Desc_Aim.fSizeX * 0.5f);
	Desc_Aim.fY = g_iWinSizeY * 0.5f - (Desc_Aim.fSizeY * 0.5f);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UIAim"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, &Desc_Aim)))
		return E_FAIL;

	m_pUIAim = dynamic_cast<CUIAim*>(m_pGameInstance->Find_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag));

	/* 카드키 */
	CUIObject::UIOBJECT_DESC Desc_CardKey{};

	Desc_CardKey.fSizeX = 92.f;
	Desc_CardKey.fSizeY = 56.f;
	Desc_CardKey.fX = g_iWinSizeX - 20.f - (Desc_CardKey.fSizeX * 0.5f);
	Desc_CardKey.fY = g_iWinSizeY - 80.f - (Desc_CardKey.fSizeY * 0.5f);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UICardKey"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, &Desc_CardKey)))
		return E_FAIL;

	m_pUICardKey = dynamic_cast<CUICardKey*>(m_pGameInstance->Find_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag));

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Cube(const _wstring& strLayerTag)
{
	/*if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_CubeObject"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	m_pTerrain_Manager->Add_Cube(LEVEL::GAMEPLAY);*/

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Effect(const _wstring& strLayerTag)
{
	m_pEffect_Manager = CEffect_Manager::GetInstance();
	m_pEffect_Manager->Initialize();
	Safe_AddRef(m_pEffect_Manager);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Items(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Item_Armor"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Item_Healpack"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Item_Pistol_Bullet"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Item_ShootGun_Bullet"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Item_CardKey"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Map_Objects(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Map_Gate"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Map_Building"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Map_Wall"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Map_Box"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Map_ContainerBox"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Interaction_Objects(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Interaction_DoorLock"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	return S_OK;
}

CLevel_GamePlay* CLevel_GamePlay::Create(LPDIRECT3DDEVICE9 pGraphic_Device, LEVEL eLevelID)
{
	CLevel_GamePlay* pInstance = new CLevel_GamePlay(pGraphic_Device, eLevelID);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_GamePlay");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();

	Safe_Release(m_pUIHp);
	Safe_Release(m_pUIArmor);
	Safe_Release(m_pUIBullets);
	Safe_Release(m_pUIInteraction);
	Safe_Release(m_pUIAim);
	Safe_Release(m_pUICardKey);
	Safe_Release(m_pTerrain_Manager);

	m_pEffect_Manager->Release_Effect_Manager();
	Safe_Release(m_pEffect_Manager);

	for (auto& iter : m_pUIItemQueues)
	{
		Safe_Release(iter);
	}
}
