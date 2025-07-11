#include "ItemShootGunBullet.h"
#include "GameInstance.h"

CItemShootGunBullet::CItemShootGunBullet(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CItem{ pGraphic_Device }
{
}

CItemShootGunBullet::CItemShootGunBullet(const CItemShootGunBullet& Prototype)
	: CItem(Prototype)
{
}

HRESULT CItemShootGunBullet::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CItemShootGunBullet::Initialize(void* pArg)
{
	m_pObjectDesc = *static_cast<CGameObject::GAMEOBJECT_DESC*>(pArg);

	CLandObject::LANDOBJECT_DESC			Desc{};
	Desc.pLandTransform = static_cast<CTransform*>(m_pGameInstance->Get_Component(m_pObjectDesc.iLayerLevel, TEXT("Layer_BackGround"), TEXT("Com_Transform")));
	Desc.pLandVIBuffer = static_cast<CVIBuffer*>(m_pGameInstance->Get_Component(m_pObjectDesc.iLayerLevel, TEXT("Layer_BackGround"), TEXT("Com_VIBuffer")));

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	//m_pTransformCom->Set_State(STATE::POSITION, _float3(10.f, 0.f, 7.f));

	SetUp_OnTerrain(m_pTransformCom, 0.5f);

	m_fItemOriginPosY = m_pTransformCom->Get_State(STATE::POSITION).y;

	m_pPlayerTransformCom = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(
		m_pObjectDesc.iLayerLevel, TEXT("Layer_Player"), TEXT("Com_Transform")));
	Safe_AddRef (m_pPlayerTransformCom);

	return S_OK;
}

void CItemShootGunBullet::Priority_Update(_float fTimeDelta)
{
}

void CItemShootGunBullet::Update(_float fTimeDelta)
{
	Item_Animation(fTimeDelta);
	m_pTransformCom->LookAt(m_pPlayerTransformCom->Get_State(STATE::POSITION));
}

void CItemShootGunBullet::Late_Update(_float fTimeDelta)
{
	//// 애니메이션 프레임 증가
	//// 프레임 전체 런타임 -> 상수로 제어해서 처리하기
	//m_pAnimationCom->Play_Animation(fTimeDelta);

	Compute_CamDistance(m_pTransformCom->Get_State(STATE::POSITION));
	m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);
}

HRESULT CItemShootGunBullet::Render()
{
	m_pTransformCom->Set_Transform();

	m_pTextureCom->Set_Texture(ENUM_CLASS(BULLET::SHOOTGUN));

	if (FAILED(Begin_RenderState()))
		return E_FAIL;

	m_pVIBufferCom->Render();

	if (FAILED(End_RenderState()))
		return E_FAIL;

	return S_OK;
}

HRESULT CItemShootGunBullet::Ready_Components()
{
	/* Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{ 5.f, D3DXToRadian(90.0f) };
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(m_pObjectDesc.iLayerLevel, TEXT("Prototype_Component_Texture_Item_Bullets"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CItemShootGunBullet::Begin_RenderState()
{
	/* 알파 테스트 : 픽셀의 알파를 비교해서 그린다 안그린다를 설정. */
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CItemShootGunBullet::End_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

void CItemShootGunBullet::OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta)
{
	if (eColType == COLLISION::BOX)
	{
		m_isDead = true;
		m_pGameInstance->PlaySoundOnce(TEXT("Get_Item.ogg"), CHANNELID::SOUND_EFFECT, 0.5f);
	}
}

CItemShootGunBullet* CItemShootGunBullet::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CItemShootGunBullet* pInstance = new CItemShootGunBullet(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CItemShootGunBullet::Clone(void* pArg)
{
	CItemShootGunBullet* pInstance = new CItemShootGunBullet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CItemShootGunBullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItemShootGunBullet::Free()
{
	__super::Free();
}