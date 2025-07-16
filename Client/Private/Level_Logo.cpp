#include "Level_Logo.h"

#include "GameInstance.h"

#include "Level_Loading.h"
#include "UIPressEnter.h"
#include "BackGround.h"

CLevel_Logo::CLevel_Logo(LPDIRECT3DDEVICE9 pGraphic_Device, LEVEL eLevelID)
	: CLevel { pGraphic_Device, ENUM_CLASS(eLevelID)}
	
{

}

HRESULT CLevel_Logo::Initialize()
{
	m_pGameInstance->PlayBGM(L"starters.xm", 0.7f);

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_Logo::Update(_float fTimeDelta)
{
	m_pUIPressEnter->Set_Guide(true);

	if(m_pGameInstance->Key_Down(VK_RETURN))
	{
		if (FAILED(m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pGraphic_Device, LEVEL::LOADING, LEVEL::GAMEPLAY))))
			return;
	}
}

HRESULT CLevel_Logo::Render()
{
	SetWindowText(g_hWnd, TEXT("Ion Fury"));

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	_uint iTextureIndex{ 1 };

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_BackGround"),
		ENUM_CLASS(LEVEL::LOGO), strLayerTag, &iTextureIndex)))
		return E_FAIL;

	/* 상호작용 키 */
	CUIObject::UIOBJECT_DESC Desk_PressEnter{};

	Desk_PressEnter.iTextLength = wcslen(TEXT("Press [Enter] Key"));
	Desk_PressEnter.fSizeX = 30.f * Desk_PressEnter.iTextLength;
	Desk_PressEnter.fSizeY = 30.f;
	Desk_PressEnter.fX = g_iWinSizeX * 0.5f - (Desk_PressEnter.fSizeX * 0.5f);
	Desk_PressEnter.fY = g_iWinSizeY * 0.8f;
	Desk_PressEnter.iLayerLevelIndex = ENUM_CLASS(LEVEL::LOGO);
	Desk_PressEnter.strLayerTag = strLayerTag;
	Desk_PressEnter.strFontType = TEXT("Primary");

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UIPressEnter"),
		ENUM_CLASS(LEVEL::LOGO), Desk_PressEnter.strLayerTag, &Desk_PressEnter)))
		return E_FAIL;

	m_pUIPressEnter = dynamic_cast<CUIPressEnter*>(m_pGameInstance->Find_GameObject_ToLayer(ENUM_CLASS(LEVEL::LOGO), strLayerTag));
	Safe_AddRef(m_pUIPressEnter);

	return S_OK;
}

CLevel_Logo* CLevel_Logo::Create(LPDIRECT3DDEVICE9 pGraphic_Device, LEVEL eLevelID)
{
	CLevel_Logo* pInstance = new CLevel_Logo(pGraphic_Device, eLevelID);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Logo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Logo::Free()
{
	__super::Free();

	Safe_Release(m_pUIPressEnter);
}
