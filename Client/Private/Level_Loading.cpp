#include "Level_Loading.h"

#include "Loader.h"
#include "GameInstance.h"

#include "Level_Logo.h"
#include "Level_GamePlay.h"
#include "Level_Jusin.h"
#include "Level_BossFight.h"
#include "Level_Ending.h"
#include "Camera.h"
#include "UIPressEnter.h"
#include "BackGround.h"

CLevel_Loading::CLevel_Loading(LPDIRECT3DDEVICE9 pGraphic_Device, LEVEL eLevelID)
	: CLevel { pGraphic_Device, ENUM_CLASS(eLevelID)}
{

}

HRESULT CLevel_Loading::Initialize(LEVEL eNextLevelID, void* pArg)
{
	m_eNextLevelID = eNextLevelID;

	if(pArg != nullptr)
		m_tPlayerInfo = *static_cast<CPlayer::PLAYER_INFO*>(pArg);

	/* 다음 레벨에 대한 자원을 로드하여 준비해둔다. */
	m_pLoader = CLoader::Create(m_pGraphic_Device, eNextLevelID);
	if (nullptr == m_pLoader)
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_Loading::Update(_float fTimeDelta)
{
	m_pUIPressEnter->Set_Guide(m_pLoader->isFinished());

	if (true == m_pLoader->isFinished() &&
		m_pGameInstance->Key_Down(VK_RETURN))
	{
		m_pGameInstance->StopAll();
		CLevel* pNewLevel = { nullptr };

		switch (m_eNextLevelID)
		{
		case LEVEL::LOGO:
			pNewLevel = CLevel_Logo::Create(m_pGraphic_Device, m_eNextLevelID);
			break;
		case LEVEL::GAMEPLAY:
			pNewLevel = CLevel_GamePlay::Create(m_pGraphic_Device, m_eNextLevelID);
			break;
		case LEVEL::JUSIN:
			pNewLevel = CLevel_Jusin::Create(m_pGraphic_Device, m_eNextLevelID, &m_tPlayerInfo);
			break;
		case LEVEL::BOSSFIGHT:
			pNewLevel = CLevel_BossFight::Create(m_pGraphic_Device, m_eNextLevelID, &m_tPlayerInfo);
			break;
		case LEVEL::ENDING:
			pNewLevel = CLevel_Ending::Create(m_pGraphic_Device, m_eNextLevelID);
			break;
		}

		if (FAILED(m_pGameInstance->Change_Level(pNewLevel)))
			return;		
	}	
}

HRESULT CLevel_Loading::Render()
{
	m_pLoader->Output();

	return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	_uint iTextureIndex{ 0 };

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_BackGround"),
		ENUM_CLASS(LEVEL::LOADING), strLayerTag, &iTextureIndex)))
		return E_FAIL;

	/* 상호작용 키 */
	CUIObject::UIOBJECT_DESC Desk_PressEnter{};

	Desk_PressEnter.iTextLength = wcslen(TEXT("Press [Enter] Key"));
	Desk_PressEnter.fSizeX = 30.f * Desk_PressEnter.iTextLength;
	Desk_PressEnter.fSizeY = 30.f;
	Desk_PressEnter.fX = g_iWinSizeX * 0.5f - (Desk_PressEnter.fSizeX * 0.5f);
	Desk_PressEnter.fY = g_iWinSizeY * 0.8f;
	Desk_PressEnter.iLayerLevelIndex = ENUM_CLASS(LEVEL::LOADING);
	Desk_PressEnter.strLayerTag = strLayerTag;
	Desk_PressEnter.strFontType = TEXT("Primary");

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UIPressEnter"),
		ENUM_CLASS(LEVEL::LOADING), Desk_PressEnter.strLayerTag, &Desk_PressEnter)))
		return E_FAIL;

	m_pUIPressEnter = dynamic_cast<CUIPressEnter*>(m_pGameInstance->Find_GameObject_ToLayer(ENUM_CLASS(LEVEL::LOADING), strLayerTag));
	Safe_AddRef(m_pUIPressEnter);

	return S_OK;
}

CLevel_Loading* CLevel_Loading::Create(LPDIRECT3DDEVICE9 pGraphic_Device, LEVEL eLevelID, LEVEL eNextLevelID, void* pArg)
{
	CLevel_Loading* pInstance = new CLevel_Loading(pGraphic_Device, eLevelID);

	if (FAILED(pInstance->Initialize(eNextLevelID, pArg)))
	{
		MSG_BOX("Failed to Created : CLevel_Loading");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);
	Safe_Release(m_pUIPressEnter);
}
