#include "Level_Ending.h"

#include "GameInstance.h"

#include "Level_Loading.h"
#include "BackGround.h"

CLevel_Ending::CLevel_Ending(LPDIRECT3DDEVICE9 pGraphic_Device, LEVEL eLevelID)
	: CLevel{ pGraphic_Device, ENUM_CLASS(eLevelID) }
{
}

HRESULT CLevel_Ending::Initialize()
{
	m_pGameInstance->StopAll();
	m_pGameInstance->PlayBGM(L"corroded_192.xm", 0.7f);

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_Ending::Update(_float fTimeDelta)
{
	/*if (m_pGameInstance->Key_Down(VK_RETURN))
	{
		if (FAILED(m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pGraphic_Device, LEVEL::LOADING, LEVEL::GAMEPLAY))))
			return;
	}*/
}

HRESULT CLevel_Ending::Render()
{
	SetWindowText(g_hWnd, TEXT("Ion Fury"));

	return S_OK;
}

HRESULT CLevel_Ending::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	_uint iTextureIndex{ 2 };

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_BackGround"),
		ENUM_CLASS(LEVEL::ENDING), strLayerTag, &iTextureIndex)))
		return E_FAIL;

	return S_OK;
}

CLevel_Ending* CLevel_Ending::Create(LPDIRECT3DDEVICE9 pGraphic_Device, LEVEL eLevelID)
{
	CLevel_Ending* pInstance = new CLevel_Ending(pGraphic_Device, eLevelID);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Ending");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Ending::Free()
{
	__super::Free();
}
