#include "UIBullets.h"

#include "GameInstance.h"

CUIBullets::CUIBullets(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUIObject{ pGraphic_Device }
{
}

CUIBullets::CUIBullets(const CUIBullets& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CUIBullets::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIBullets::Initialize(void* pArg)
{
	UIOBJECT_DESC* pTemp = static_cast<UIOBJECT_DESC*>(pArg);

	m_tagDesc.fSizeX = pTemp->fSizeX / (pTemp->iTextLength + 1);
	m_tagDesc.fSizeY = pTemp->fSizeY;
	m_tagDesc.fX = pTemp->fX - (m_tagDesc.fSizeX * 0.5f);
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

	Desc.fSizeX = m_tagDesc.fSizeX;
	Desc.fSizeY = m_tagDesc.fSizeY;
	Desc.fX = m_tagDesc.fX - (Desc.fSizeX * (m_tagDesc.iTextLength + 1));
	Desc.fY = m_tagDesc.fY - (Desc.fSizeY * 0.5f);
	Desc.iTextLength = m_tagDesc.iTextLength;
	Desc.iLayerLevelIndex = m_tagDesc.iLayerLevelIndex;
	Desc.strLayerTag = m_tagDesc.strLayerTag;
	Desc.strFontType = m_tagDesc.strFontType;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(Desc.iLayerLevelIndex, TEXT("Prototype_GameObject_UIText"),
		Desc.iLayerLevelIndex, Desc.strLayerTag, &Desc)))
		return E_FAIL;

	m_pText = dynamic_cast<CUIText*>(m_pGameInstance->Find_GameObject_ToLayer(Desc.iLayerLevelIndex, Desc.strLayerTag));

	return S_OK;
}

void CUIBullets::Priority_Update(_float fTimeDelta)
{
	int a = 10;
}

void CUIBullets::Update(_float fTimeDelta)
{
	__super::Update_Transform(m_pTransformCom);
}

void CUIBullets::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
}

HRESULT CUIBullets::Render()
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

void CUIBullets::Set_Bullets(_uint iNumber)
{
	m_iBullets = iNumber;

	_tchar ws[10];
	swprintf(ws, 10, L"%03d", m_iBullets);

	m_pText->Set_Text(ws);
}

HRESULT CUIBullets::Ready_Components()
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
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_Bullets"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

CUIBullets* CUIBullets::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CUIBullets* pInstance = new CUIBullets(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIBullets::Clone(void* pArg)
{
	CUIBullets* pInstance = new CUIBullets(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIBullets");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIBullets::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
}
