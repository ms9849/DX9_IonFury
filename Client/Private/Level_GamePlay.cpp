#include "Level_GamePlay.h"
#include "GameInstance.h"
#include "Monster.h"
#include "UIObject.h"

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

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Cube(TEXT("Layer_Cube"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_GamePlay::Update(_float fTimeDelta)
{
	/* 전환 하고나서 look벡터가 이상함 */
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
	m_pUIHp->Set_Hp(m_pPlayer->Get_Player_Info().iHp);
	m_pUIBullets->Set_Bullets(m_pPlayer->Get_Player_Info().iBullets);
	m_pUIInteraction->Set_Interaction(TEXT("Press [E] Key"));
#pragma endregion

	m_pGameInstance->Check_OBBCollision(TEXT("Layer_Cube"), TEXT("Layer_Player"), ENUM_CLASS(LEVEL::GAMEPLAY));
}

HRESULT CLevel_GamePlay::Render()
{
	SetWindowText(g_hWnd, TEXT("게임플레이레벨이빈다"));
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Lights()
{
	/* 빛의 정보 */
	D3DLIGHT9			LightDesc{};
	LightDesc.Type = D3DLIGHT_DIRECTIONAL;

	LightDesc.Direction = _float3(1.f, -1.f, 1.f);
	LightDesc.Diffuse = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.f);
	LightDesc.Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.f);
	LightDesc.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);

	m_pGraphic_Device->SetLight(0, &LightDesc);

	//D3DLIGHT9			LightDesc{};
	LightDesc.Type = D3DLIGHT_POINT;

	LightDesc.Position = _float3(10.0f, 5.0f, 10.f);
	LightDesc.Range = 30.0f;
	LightDesc.Attenuation1 = 1.f;
	LightDesc.Diffuse = D3DXCOLOR(1.f, 0.0f, 0.f, 1.f);
	LightDesc.Ambient = D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.f);
	LightDesc.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);

	m_pGraphic_Device->SetLight(1, &LightDesc);

	/* 받은 빛을 어떻게 반사할건지?(재질) */
	D3DMATERIAL9		MaterialDesc{};
	MaterialDesc.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	MaterialDesc.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	MaterialDesc.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);


	m_pGraphic_Device->SetMaterial(&MaterialDesc);

	m_pGraphic_Device->LightEnable(0, true);
	m_pGraphic_Device->LightEnable(1, true);

	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE);

	return S_OK;
}
HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Terrain"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

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
	CameraDesc.vEye = dynamic_cast<CTransform*>(m_pPlayer->Find_Component(TEXT("Com_Transform")))->Get_State(STATE::POSITION);
	CameraDesc.vAt = dynamic_cast<CTransform*>(m_pPlayer->Find_Component(TEXT("Com_Transform")))->Get_State(STATE::LOOK);
	CameraDesc.fSpeedPerSec = 5.f;
	CameraDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Camera"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, &CameraDesc)))
		return E_FAIL;

	m_pCamera = dynamic_cast<CCamera*>(m_pGameInstance->Find_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Camera")));
	Safe_AddRef(m_pCamera);

	m_pCamera->Camera_Configure_Clear(m_CameraSettings);
	m_CameraSettings.pTarget = m_pPlayer;
	m_CameraSettings.isChaseTarget = true;
	m_CameraSettings.isSyncLook = true;
	m_CameraSettings.isMouseFixCenter = true;
	m_CameraSettings.isCanTurn = true;
	//m_CameraSettings.vLimitDistance = _float3{ 0.f, 0.f, -1.f };
	SetCursorPos(g_iWinSizeX * 0.5f, g_iWinSizeY * 0.5f);

	m_pCamera->Camera_Configure(m_CameraSettings);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Player"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;
	
	m_pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Find_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Player")));
	Safe_AddRef(m_pPlayer);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const _wstring& strLayerTag)
{
	for (size_t i = 0; i < 20; i++)
	{
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Monster"),
			ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI(const _wstring& strLayerTag)
{
	_wstring ws{};

	/* 체력 */
	CUIObject::UIOBJECT_DESC Desc_Hp{};

	ws = to_wstring(m_pPlayer->Get_Player_Info().iHp);

	// UI전체 크기 및 위치
	Desc_Hp.iTextLength = wcslen(ws.c_str());
	Desc_Hp.fSizeX = 50.f + (50.f * Desc_Hp.iTextLength);
	Desc_Hp.fSizeY = 50.f;
	Desc_Hp.fX = 0.f;
	Desc_Hp.fY = g_iWinSizeY - (Desc_Hp.fSizeY * 0.5f);
	Desc_Hp.iLayerLevelIndex = ENUM_CLASS(LEVEL::GAMEPLAY);
	Desc_Hp.strLayerTag = strLayerTag;
	Desc_Hp.strFontType = TEXT("Default");

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UIHp"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, &Desc_Hp)))
		return E_FAIL;

	m_pUIHp = dynamic_cast<CUIHp*>(m_pGameInstance->Find_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag));

	/* 총알 */
	CUIObject::UIOBJECT_DESC Desc_Bullets{};

	ws = to_wstring(m_pPlayer->Get_Player_Info().iBullets);

	Desc_Bullets.iTextLength = wcslen(ws.c_str());
	Desc_Bullets.fSizeX = 50.f + (50.f * Desc_Bullets.iTextLength);
	Desc_Bullets.fSizeY = 50.f;
	Desc_Bullets.fX = g_iWinSizeX;
	Desc_Bullets.fY = g_iWinSizeY - (Desc_Bullets.fSizeY * 0.5f);
	Desc_Bullets.iLayerLevelIndex = ENUM_CLASS(LEVEL::GAMEPLAY);
	Desc_Bullets.strLayerTag = strLayerTag;
	Desc_Bullets.strFontType = TEXT("Default");

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UIBullets"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, &Desc_Bullets)))
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
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, &Desc_Interaction)))
		return E_FAIL;

	m_pUIInteraction = dynamic_cast<CUIInteraction*>(m_pGameInstance->Find_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag));

	/* 상호작용 키 */
	CUIObject::UIOBJECT_DESC Desc_Aim{};

	Desc_Aim.fSizeX = 30.f;
	Desc_Aim.fSizeY = 30.f;
	Desc_Aim.fX = g_iWinSizeX * 0.5f - (Desc_Aim.fSizeX * 0.5f);
	Desc_Aim.fY = g_iWinSizeY * 0.5f - (Desc_Aim.fSizeY * 0.5f);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_UIAim"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag, &Desc_Aim)))
		return E_FAIL;

	m_pUIAim = dynamic_cast<CUIAim*>(m_pGameInstance->Find_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag));

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Cube(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_CubeObject"),
		ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Effect(const _wstring& strLayerTag)
{
	for (size_t i = 0; i < 50; i++)
	{
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Effect"),
			ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag)))
			return E_FAIL;
	}

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

	Safe_Release(m_pCamera);
	Safe_Release(m_pPlayer);
}
