#include "UIText.h"

#include "GameInstance.h"
#include "UIFont.h"

CUIText::CUIText(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUIObject{pGraphic_Device}
{
}

CUIText::CUIText(const CUIText& Prototype)
	: CUIObject(Prototype)
{
}

HRESULT CUIText::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIText::Initialize(void* pArg)
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

	if (FAILED(__super::Initialize(&pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	UIOBJECT_DESC Desc{};

	// 각 텍스트 크기 및 위치
	Desc.fSizeX = m_tagDesc.fSizeX;
	Desc.fSizeY = m_tagDesc.fSizeY;
	Desc.fY = m_tagDesc.fY;
	Desc.iTextLength = m_tagDesc.iTextLength;
	Desc.iLayerLevelIndex = m_tagDesc.iLayerLevelIndex;
	Desc.strLayerTag = m_tagDesc.strLayerTag;
	Desc.strFontType = m_tagDesc.strFontType;

	m_vecFont.reserve(Desc.iTextLength);

	for (size_t i = 0; i < Desc.iTextLength; ++i)
	{
		Desc.fX = (m_tagDesc.fX + (Desc.fSizeX * 0.5f)) + (Desc.fSizeX * i);

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(Desc.iLayerLevelIndex, TEXT("Prototype_GameObject_UIFont"),
			Desc.iLayerLevelIndex, Desc.strLayerTag, &Desc)))
			return E_FAIL;

		CUIFont* pUIFont = dynamic_cast<CUIFont*>(m_pGameInstance->Find_GameObject_ToLayer(Desc.iLayerLevelIndex, Desc.strLayerTag));
		Safe_AddRef(pUIFont);

		m_vecFont.push_back(pUIFont);
	}

	return S_OK;
}

void CUIText::Priority_Update(_float fTimeDelta)
{
}

void CUIText::Update(_float fTimeDelta)
{
	__super::Update_Transform(m_pTransformCom);
}

void CUIText::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
}

HRESULT CUIText::Render()
{
	m_pTransformCom->Set_Transform();

	return S_OK;
}

void CUIText::Set_Text(const _tchar* strText)
{
	for (size_t i = 0; i < wcslen(strText); ++i)
	{
		_tchar ch = strText[i];
		m_vecFont[i]->Set_Font_Type(ch);
	}
	for (size_t i = wcslen(strText); i < m_tagDesc.iTextLength; ++i)
	{
		m_vecFont[i]->Set_Font_Type(' ');
	}
}

void CUIText::Set_Text(_uint iNumber)
{
	for (size_t i = 0; i < m_tagDesc.iTextLength; ++i)
	{
		_tchar ch = to_string(iNumber)[i];
		m_vecFont[i]->Set_Font_Type(ch);
	}
}

HRESULT CUIText::Ready_Components()
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

CUIText* CUIText::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CUIText* pInstance = new CUIText(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIText::Clone(void* pArg)
{
	CUIText* pInstance = new CUIText(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIText");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIText::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);

	for (auto& iter : m_vecFont)
	{
		Safe_Release(iter);
	}
}
