#include "UIFont.h"

#include "GameInstance.h"

CUIFont::CUIFont(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUIObject{ pGraphic_Device }
{
}

CUIFont::CUIFont(const CUIFont& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CUIFont::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIFont::Initialize(void* pArg)
{
	UIOBJECT_DESC* pTemp = static_cast<UIOBJECT_DESC*>(pArg);

	m_tagDesc.fSizeX = pTemp->fSizeX;
	m_tagDesc.fSizeY = pTemp->fSizeY;
	m_tagDesc.fX = pTemp->fX + (m_tagDesc.fSizeX * 0.5f);
	m_tagDesc.fY = pTemp->fY + (m_tagDesc.fSizeY * 0.5f);
	m_tagDesc.iTextLength = pTemp->iTextLength;
	m_tagDesc.iLayerLevelIndex = pTemp->iLayerLevelIndex;
	m_tagDesc.strLayerTag = pTemp->strLayerTag;
	m_tagDesc.strFontType = pTemp->strFontType;

	if (FAILED(__super::Initialize(&m_tagDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CUIFont::Priority_Update(_float fTimeDelta)
{
	int a = 10;
}

void CUIFont::Update(_float fTimeDelta)
{
	__super::Update_Transform(m_pTransformCom);
}

void CUIFont::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
}

HRESULT CUIFont::Render()
{
	m_pTransformCom->Set_Transform();

	/* 자식 클래스 만들어서 얘로 문자열 받아오고 자식가지고 글자 찍어줘야할듯? */
	//m_pFontsCom->Set_Font(static_cast<_uint>(m_pStrText));

	auto iter = m_pFontsComs.find(m_tagDesc.strFontType);
	iter->second->Set_Font(static_cast<_uint>(m_pStrText));

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

void CUIFont::Set_Font_Type(_tchar ch)
{
	m_pStrText = ch;
}

HRESULT CUIFont::Ready_Components()
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

	for (size_t i = 0; i < (sizeof(m_strFontKeys) / sizeof(m_strFontKeys[0])); ++i)
	{
		_tchar strPrototypeTag[256];
		_tchar strComponentTag[256];

		CFonts* pFontsCom{ nullptr };

		wsprintf(strPrototypeTag, TEXT("Prototype_Component_Fonts_%s"), m_strFontKeys[i].c_str());
		wsprintf(strComponentTag, TEXT("Com_%s_Font"), m_strFontKeys[i].c_str());

		/* Com_Texture */
		if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), strPrototypeTag,
			strComponentTag, reinterpret_cast<CComponent**>(&pFontsCom))))
			return E_FAIL;

		m_pFontsComs.emplace(m_strFontKeys[i], pFontsCom);
	}

	///* Com_Fonts */
	//if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Fonts_Default"),
	//	TEXT("Com_Fonts_Default"), reinterpret_cast<CComponent**>(&m_pFontsCom))))
	//	return E_FAIL;

	//if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Fonts_Gray"),
	//	TEXT("Com_Fonts_Gray"), reinterpret_cast<CComponent**>(&m_pFontsCom))))
	//	return E_FAIL;

	//if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Fonts_Primary"),
	//	TEXT("Com_Fonts_Primary"), reinterpret_cast<CComponent**>(&m_pFontsCom))))
	//	return E_FAIL;

	return S_OK;
}

CUIFont* CUIFont::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CUIFont* pInstance = new CUIFont(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIFont::Clone(void* pArg)
{
	CUIFont* pInstance = new CUIFont(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIFont");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIFont::Free()
{
	__super::Free();

	//Safe_Release(m_pFontsCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
}
