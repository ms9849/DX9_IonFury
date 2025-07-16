#include "ItemShootGun.h"

#include "GameInstance.h"

CItemShootGun::CItemShootGun(LPDIRECT3DDEVICE9 pGraphicDev) :
    CItem { pGraphicDev }
{
}

CItemShootGun::CItemShootGun(const CItemShootGun& Prototype) :
    CItem { Prototype }
{
}

HRESULT CItemShootGun::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CItemShootGun::Initialize(void* pArg)
{
	m_pObjectDesc = *static_cast<CGameObject::GAMEOBJECT_DESC*>(pArg);

	CLandObject::LANDOBJECT_DESC			Desc{};
	Desc.pLandTransform = static_cast<CTransform*>(m_pGameInstance->Get_Component(m_pObjectDesc.iLayerLevel, TEXT("Layer_BackGround"), TEXT("Com_Transform")));
	Desc.pLandVIBuffer = static_cast<CVIBuffer*>(m_pGameInstance->Get_Component(m_pObjectDesc.iLayerLevel, TEXT("Layer_BackGround"), TEXT("Com_VIBuffer")));

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	//m_pTransformCom->Set_Scale({ 0.2f, 0.2f, 0.2f });

	m_pTransformCom->Set_State(STATE::RIGHT, m_pObjectDesc.matWorld.m[0]);
	m_pTransformCom->Set_State(STATE::UP, m_pObjectDesc.matWorld.m[1]);
	m_pTransformCom->Set_State(STATE::LOOK, m_pObjectDesc.matWorld.m[2]);
	m_pTransformCom->Set_State(STATE::POSITION, m_pObjectDesc.matWorld.m[3]);

	//m_pTransformCom->Set_State(STATE::POSITION, _float3(10.f, 0.f, 7.f));
	if (!m_bParabola)
		SetUp_OnTerrain(m_pTransformCom, 0.5f);

	m_fItemOriginPosY = m_pTransformCom->Get_State(STATE::POSITION).y;

	m_pPlayerTransformCom = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(
		m_pObjectDesc.iLayerLevel, TEXT("Layer_Player"), TEXT("Com_Transform")));
	Safe_AddRef(m_pPlayerTransformCom);

	return S_OK;
}

void CItemShootGun::Priority_Update(_float fTimeDelta)
{
}

void CItemShootGun::Update(_float fTimeDelta)
{
	if (!m_bParabola)
		Item_Animation(fTimeDelta);
	else
		Parabola(2.f, fTimeDelta);

	m_pTransformCom->LookAt(m_pPlayerTransformCom->Get_State(STATE::POSITION));
}

void CItemShootGun::Late_Update(_float fTimeDelta)
{
	Compute_CamDistance(m_pTransformCom->Get_State(STATE::POSITION));
	m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);
}

HRESULT CItemShootGun::Render()
{
	m_pTransformCom->Set_Transform();

	m_pTextureCom->Set_Texture(0);

	if (FAILED(Begin_RenderState()))
		return E_FAIL;

	m_pVIBufferCom->Render();

	if (FAILED(End_RenderState()))
		return E_FAIL;

	return S_OK;
}

HRESULT CItemShootGun::Ready_Components()
{
	/* Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{ 5.f, D3DXToRadian(90.0f) };
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(m_pObjectDesc.iLayerLevel, TEXT("Prototype_Component_Texture_Item_ShootGun"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CItemShootGun::Begin_RenderState()
{
	/* 알파 테스트 : 픽셀의 알파를 비교해서 그린다 안그린다를 설정. */
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CItemShootGun::End_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

void CItemShootGun::OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta)
{
	if (eColType == COLLISION::SPHERE)
	{
		m_isDead = true;
		m_pGameInstance->PlaySoundOnce(TEXT("Get_Item.ogg"), CHANNELID::SOUND_EFFECT, 0.5f);
		m_pGameInstance->PlaySoundOnce(TEXT("Gacha.ogg"), CHANNELID::SOUND_EFFECT, 0.5f);
	}
}

CItemShootGun* CItemShootGun::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CItemShootGun* pInstance = new CItemShootGun(pGraphicDev);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CItemShootGun");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CItemShootGun::Clone(void* pArg)
{
	CItemShootGun* pInstance = new CItemShootGun(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CItemShootGun");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItemShootGun::Free()
{
	__super::Free();
}
