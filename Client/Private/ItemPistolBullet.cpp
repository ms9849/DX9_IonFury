#include "ItemPistolBullet.h"
#include "GameInstance.h"

CItemPistolBullet::CItemPistolBullet(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CItem{ pGraphic_Device }
{
}

CItemPistolBullet::CItemPistolBullet(const CItemPistolBullet& Prototype)
	: CItem{ Prototype }
{
}

HRESULT CItemPistolBullet::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CItemPistolBullet::Initialize(void* pArg)
{
	CLandObject::LANDOBJECT_DESC			Desc{};
	Desc.pLandTransform = static_cast<CTransform*>(m_pGameInstance->Get_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_BackGround"), TEXT("Com_Transform")));
	Desc.pLandVIBuffer = static_cast<CVIBuffer*>(m_pGameInstance->Get_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_BackGround"), TEXT("Com_VIBuffer")));

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(STATE::POSITION, _float3(15.f, 0.f, 7.f));

	SetUp_OnTerrain(m_pTransformCom, 0.5f);

	m_fItemOriginPosY = m_pTransformCom->Get_State(STATE::POSITION).y;

	m_pPlayerTransformCom = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(
		ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Player"), TEXT("Com_Transform")));

	return S_OK;
}

void CItemPistolBullet::Priority_Update(_float fTimeDelta)
{
}

void CItemPistolBullet::Update(_float fTimeDelta)
{
	Item_Animation(fTimeDelta);
	m_pTransformCom->LookAt(m_pPlayerTransformCom->Get_State(STATE::POSITION));
}

void CItemPistolBullet::Late_Update(_float fTimeDelta)
{
	//// 애니메이션 프레임 증가
	//// 프레임 전체 런타임 -> 상수로 제어해서 처리하기
	//m_pAnimationCom->Play_Animation(fTimeDelta);


	m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);
}

HRESULT CItemPistolBullet::Render()
{
	m_pTransformCom->Set_Transform();

	m_pTextureCom->Set_Texture(ENUM_CLASS(BULLET::PISTOL));

	if (FAILED(Begin_RenderState()))
		return E_FAIL;

	m_pVIBufferCom->Render();

	if (FAILED(End_RenderState()))
		return E_FAIL;

	return S_OK;
}

HRESULT CItemPistolBullet::Ready_Components()
{
	/* Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{ 5.f, D3DXToRadian(90.0f) };
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_Item_Bullets"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CItemPistolBullet::Begin_RenderState()
{
	/* 알파 테스트 : 픽셀의 알파를 비교해서 그린다 안그린다를 설정. */
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CItemPistolBullet::End_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

void CItemPistolBullet::OnCollision(CGameObject* pDst, COLLISION eColType)
{
	if (eColType == COLLISION::SPHERE)
		m_isDead = true;
}

CItemPistolBullet* CItemPistolBullet::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CItemPistolBullet* pInstance = new CItemPistolBullet(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CItemPistolBullet::Clone(void* pArg)
{
	CItemPistolBullet* pInstance = new CItemPistolBullet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CItemHealpack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItemPistolBullet::Free()
{
	__super::Free();
}