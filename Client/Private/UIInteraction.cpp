#include "UIInteraction.h"

#include "GameInstance.h"
#include "UIFont.h"
#include "UIText.h"
#include "Player.h"
#include "DoorLock.h"

CUIInteraction::CUIInteraction(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUIObject{ pGraphic_Device }
{
}

CUIInteraction::CUIInteraction(const CUIInteraction& Prototype)
	: CUIObject(Prototype)
{
}

HRESULT CUIInteraction::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIInteraction::Initialize(void* pArg)
{
	UIOBJECT_DESC* pTemp = static_cast<UIOBJECT_DESC*>(pArg);

	m_tagDesc.fSizeX = pTemp->fSizeX;
	m_tagDesc.fSizeY = pTemp->fSizeY;
	m_tagDesc.fX = pTemp->fX;
	m_tagDesc.fY = pTemp->fY;
	m_tagDesc.iTextLength = pTemp->iTextLength;
	m_tagDesc.iLayerLevelIndex = pTemp->iLayerLevelIndex;
	m_tagDesc.strLayerTag = pTemp->strLayerTag;
	m_tagDesc.strFontType = pTemp->strFontType;

	if (FAILED(__super::Initialize(&m_tagDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	UIOBJECT_DESC Desc{};

	Desc.fSizeX = m_tagDesc.fSizeX / m_tagDesc.iTextLength;
	Desc.fSizeY = m_tagDesc.fSizeY;
	Desc.fX = m_tagDesc.fX - (Desc.fSizeX);
	Desc.fY = m_tagDesc.fY - (Desc.fSizeY * 0.5f);
	Desc.iTextLength = m_tagDesc.iTextLength;
	Desc.iLayerLevelIndex = m_tagDesc.iLayerLevelIndex;
	Desc.strLayerTag = m_tagDesc.strLayerTag;
	Desc.strFontType = m_tagDesc.strFontType;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(Desc.iLayerLevelIndex, TEXT("Prototype_GameObject_UIText"),
		Desc.iLayerLevelIndex, Desc.strLayerTag, &Desc)))
		return E_FAIL;

	m_pText = dynamic_cast<CUIText*>(m_pGameInstance->Find_GameObject_ToLayer(Desc.iLayerLevelIndex, Desc.strLayerTag));
	Safe_AddRef(m_pText);

	m_pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Find_GameObject_ToLayer(m_tagDesc.iLayerLevelIndex, TEXT("Layer_Player")));
	Safe_AddRef(m_pPlayer);

	return S_OK;
}

void CUIInteraction::Priority_Update(_float fTimeDelta)
{
}

void CUIInteraction::Update(_float fTimeDelta)
{
	__super::Update_Transform(m_pTransformCom);
}

void CUIInteraction::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
}

HRESULT CUIInteraction::Render()
{
	m_pTransformCom->Set_Transform();

	return S_OK;
}

void CUIInteraction::Set_Interaction()
{
	if(m_pPlayer->Get_Can_Open_Door()
		&& m_pPlayer->Get_CanUse_CardKey())
		m_pText->Set_Text(TEXT("Press [E] Key"));
	else
		m_pText->Set_Text(TEXT(" "));
}

HRESULT CUIInteraction::Ready_Components()
{
	/* Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{ 5.f, D3DXToRadian(90.0f) };

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;
	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CUIInteraction* CUIInteraction::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CUIInteraction* pInstance = new CUIInteraction(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIInteraction::Clone(void* pArg)
{
	CUIInteraction* pInstance = new CUIInteraction(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIInteraction");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIInteraction::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pText);
	Safe_Release(m_pPlayer);
}
