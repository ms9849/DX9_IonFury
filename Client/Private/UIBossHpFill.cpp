#include "UIBossHpFill.h"

#include "GameInstance.h"
#include "UIFont.h"
#include "UIText.h"
#include "Boss.h"

CUIBossHpFill::CUIBossHpFill(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUIObject{ pGraphic_Device }
{
}

CUIBossHpFill::CUIBossHpFill(const CUIBossHpFill& Prototype)
	: CUIObject(Prototype)
{
}

HRESULT CUIBossHpFill::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIBossHpFill::Initialize(void* pArg)
{
	//UIOBJECT_DESC* m_pTemp = static_cast<UIOBJECT_DESC*>(pArg);
	m_pTemp = static_cast<UIOBJECT_DESC*>(pArg);

	// 보스 체력 바 이미지 위치 및 크기
	m_tagDesc.fSizeX = m_pTemp->fSizeX;
	m_tagDesc.fSizeY = m_pTemp->fSizeY;
	/*m_tagDesc.fX = pTemp->fX + (m_tagDesc.fSizeX * 0.5f);*/
	m_tagDesc.fX = m_pTemp->fX;
	m_tagDesc.fY = m_pTemp->fY;
	//m_tagDesc.iTextLength = pTemp->iTextLength;
	m_tagDesc.iLayerLevelIndex = m_pTemp->iLayerLevelIndex;
	m_tagDesc.strLayerTag = m_pTemp->strLayerTag;
	//m_tagDesc.strFontType = pTemp->strFontType;

	if (FAILED(__super::Initialize(&m_tagDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pBoss = dynamic_cast<CBoss*>(m_pGameInstance->Find_GameObject_ToLayer(ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Layer_Boss")));
	Safe_AddRef(m_pBoss);

	m_fMaxHp = m_pBoss->Get_MaxHp();

	return S_OK;
}

void CUIBossHpFill::Priority_Update(_float fTimeDelta)
{
}

void CUIBossHpFill::Update(_float fTimeDelta)
{
	if (m_pBoss->isDead())
		m_isDead = true;

	__super::Update_Transform(m_pTransformCom);
	Get_Hp();
}

void CUIBossHpFill::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
}

HRESULT CUIBossHpFill::Render()
{
	_float fHpRatio = max(0.f, min(1.f, m_fCurHp / m_fMaxHp));

	_float fHalfWidth = m_tagDesc.fSizeX * 0.5f;
	_float fBaseX = -(float)g_iWinSizeX * 0.5f + m_tagDesc.fX;
	_float fBaseY = (float)g_iWinSizeY * 0.5f - m_tagDesc.fY;

	_float fNewX = fBaseX - (1.f - fHpRatio) * fHalfWidth;

	m_pTransformCom->Set_Scale({ fHpRatio * m_tagDesc.fSizeX, m_tagDesc.fSizeY, 1.f });
	m_pTransformCom->Set_State(STATE::POSITION, { fNewX, fBaseY, 0.f });
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


HRESULT CUIBossHpFill::Ready_Components()
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
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Boss_HpFill"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

void CUIBossHpFill::Get_Hp()
{
	m_fCurHp = m_pBoss->Get_Hp();
}

void CUIBossHpFill::Set_HpRatio(float fRatio)
{
	
}

CUIBossHpFill* CUIBossHpFill::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CUIBossHpFill* pInstance = new CUIBossHpFill(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIBossHpFill::Clone(void* pArg)
{
	CUIBossHpFill* pInstance = new CUIBossHpFill(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIHp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIBossHpFill::Free()
{
	__super::Free();

	Safe_Release(m_pBoss);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
}
