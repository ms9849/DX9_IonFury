#include "UIItemQueue.h"

#include "GameInstance.h"
#include "UIFont.h"
#include "UIText.h"

CUIItemQueue::CUIItemQueue(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUIObject{ pGraphic_Device }
{
}

CUIItemQueue::CUIItemQueue(const CUIItemQueue& Prototype)
	: CUIObject(Prototype)
{
}

HRESULT CUIItemQueue::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIItemQueue::Initialize(void* pArg)
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

	return S_OK;
}

void CUIItemQueue::Priority_Update(_float fTimeDelta)
{
}

void CUIItemQueue::Update(_float fTimeDelta)
{
	__super::Update_Transform(m_pTransformCom);
}

void CUIItemQueue::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
}

HRESULT CUIItemQueue::Render()
{
	m_pTransformCom->Set_Transform();

	return S_OK;
}

void CUIItemQueue::Set_ItemQueue(const _wstring strText)
{
	m_pText->Set_Text(strText.c_str());
}

HRESULT CUIItemQueue::Ready_Components()
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

CUIItemQueue* CUIItemQueue::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CUIItemQueue* pInstance = new CUIItemQueue(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIItemQueue::Clone(void* pArg)
{
	CUIItemQueue* pInstance = new CUIItemQueue(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIItemQueue");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIItemQueue::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pText);
}
