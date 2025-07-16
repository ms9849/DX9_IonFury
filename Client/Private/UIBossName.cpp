#include "UIBossName.h"

#include "GameInstance.h"
#include "UIFont.h"
#include "UIText.h"
#include "Boss.h"

CUIBossName::CUIBossName(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUIObject{ pGraphic_Device }
{
}

CUIBossName::CUIBossName(const CUIBossName& Prototype)
	: CUIObject(Prototype)
{
}

HRESULT CUIBossName::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIBossName::Initialize(void* pArg)
{
	UIOBJECT_DESC* pTemp = static_cast<UIOBJECT_DESC*>(pArg);

	// 보스 체력 바 이미지 위치 및 크기
	m_tagDesc.fSizeX = pTemp->fSizeX;
	m_tagDesc.fSizeY = pTemp->fSizeY;
	/*m_tagDesc.fX = pTemp->fX + (m_tagDesc.fSizeX * 0.5f);*/
	m_tagDesc.fX = pTemp->fX;
	m_tagDesc.fY = pTemp->fY;
	//m_tagDesc.iTextLength = pTemp->iTextLength;
	m_tagDesc.iLayerLevelIndex = pTemp->iLayerLevelIndex;
	m_tagDesc.strLayerTag = pTemp->strLayerTag;
	//m_tagDesc.strFontType = pTemp->strFontType;

	if (FAILED(__super::Initialize(&m_tagDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pBoss = dynamic_cast<CBoss*>(m_pGameInstance->Find_GameObject_ToLayer(ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Layer_Boss")));
	Safe_AddRef(m_pBoss);

	return S_OK;
}

void CUIBossName::Priority_Update(_float fTimeDelta)
{
}

void CUIBossName::Update(_float fTimeDelta)
{
	if (m_pBoss->isDead())
		m_isDead = true;
	__super::Update_Transform(m_pTransformCom);
}

void CUIBossName::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
}

HRESULT CUIBossName::Render()
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


HRESULT CUIBossName::Ready_Components()
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
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Boss_Name"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

CUIBossName* CUIBossName::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CUIBossName* pInstance = new CUIBossName(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIBossName::Clone(void* pArg)
{
	CUIBossName* pInstance = new CUIBossName(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIHp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIBossName::Free()
{
	__super::Free();

	Safe_Release(m_pBoss);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
}
