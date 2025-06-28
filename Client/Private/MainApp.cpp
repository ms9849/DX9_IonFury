#include "MainApp.h"

#include "GameInstance.h"

#include "Level_Loading.h"
#include "Camera.h"
#include "CubeObject.h"
/*
대재훈의 은총 이 얼마나 관대한가
*/
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
	
	//m_pGameInstance->PlayBGM(L"TEST.mp3", 1.f);
	m_pGameInstance->PlayBGM(L"TEST.mp3", 0.f);
>>>>>>> Stashed changes

	return S_OK;
}

HRESULT CMainApp::Ready_Prototypes()
{
	/* For.Prototype_Component_BoxCollider*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_BoxCollider"),
		CBoxCollider::Create(m_pGraphic_Device))))
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

	/* For.Prototype_GameObject_CubeObject */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_CubeObject"),
		CCubeObject::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_Component_Animation */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Animation"),
		CAnimation::Create(m_pGraphic_Device))))
		return E_FAIL;

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
	/* For.Prototype_Component_Player_Pistol_Idle */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Player_Pistol_Idle"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Player/Pistol/Idle_%d.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Player_Pistol_Reload */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Player_Pistol_Reload"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Player/Pistol/Reload_%d.png"), 19))))
		return E_FAIL;

	/* For.Prototype_Component_Player_Pistol_Shoot */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Player_Pistol_Shoot"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Player/Pistol/Shoot_%d.png"), 5))))
		return E_FAIL;
#pragma endregion

#pragma region 몬스터 애니메이션 텍스처
	/* For.Prototype_Component_Texture_Monster */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Soldier_Front"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Soldier/Front_%d.png"), 4))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Soldier_Back"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Soldier/Back_%d.png"), 4))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Soldier_Rotate"),
		CTexture::Create(m_pGraphic_Device, TEXTURE::PLANE, TEXT("../Bin/Resources/Textures/Monster/Soldier/Rotate_%d.png"), 12))))
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
}
