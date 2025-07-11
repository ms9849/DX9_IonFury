#include "UIArmor.h"

#include "GameInstance.h"
#include "UIFont.h"
#include "UIText.h"
#include "Player.h"

CUIArmor::CUIArmor(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUIObject{ pGraphic_Device }
{
}

CUIArmor::CUIArmor(const CUIArmor& Prototype)
	: CUIObject(Prototype)
{
}

HRESULT CUIArmor::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIArmor::Initialize(void* pArg)
{
	UIOBJECT_DESC* pTemp = static_cast<UIOBJECT_DESC*>(pArg);

	// 플레이어 얼굴 이미지 위치 및 크기
	m_tagDesc.fSizeX = pTemp->fSizeX / (pTemp->iTextLength);
	m_tagDesc.fSizeY = pTemp->fSizeY;
	m_tagDesc.fX = pTemp->fX + (m_tagDesc.fSizeX * 0.5f);
	m_tagDesc.fY = pTemp->fY;
	m_tagDesc.iTextLength = pTemp->iTextLength;
	m_tagDesc.iLayerLevelIndex = pTemp->iLayerLevelIndex;
	m_tagDesc.strLayerTag = pTemp->strLayerTag;
	m_tagDesc.strFontType = pTemp->strFontType;

	if (FAILED(__super::Initialize(&m_tagDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	// 전체 체력 텍스트 크기 및 위치
	UIOBJECT_DESC Desc{};

	Desc.fSizeX = 40.f;
	Desc.fSizeY = 40.f;
	Desc.fX = m_tagDesc.fX + 20.f;
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

void CUIArmor::Priority_Update(_float fTimeDelta)
{
}

void CUIArmor::Update(_float fTimeDelta)
{
	__super::Update_Transform(m_pTransformCom);
}

void CUIArmor::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
}

HRESULT CUIArmor::Render()
{
	m_pTransformCom->Set_Transform();

	m_pTextureCom->Set_Texture(0);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	__super::Begin();

	m_pVIBufferCom->Render();

	__super::End();

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return S_OK;
}

void CUIArmor::Set_Armor()
{
	m_iArmor = m_pPlayer->Get_Player_Info().iArmor;

	_tchar ws[10];
	swprintf(ws, 10, L"%03d", m_iArmor);

	m_pText->Set_Text(ws);
}

HRESULT CUIArmor::Ready_Components()
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
	/* Com_Texture */
	if (FAILED(__super::Add_Component(m_tagDesc.iLayerLevelIndex, TEXT("Prototype_Component_Texture_UI_Armor"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

CUIArmor* CUIArmor::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CUIArmor* pInstance = new CUIArmor(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIArmor::Clone(void* pArg)
{
	CUIArmor* pInstance = new CUIArmor(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIArmor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIArmor::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pText);
	Safe_Release(m_pPlayer);
}
