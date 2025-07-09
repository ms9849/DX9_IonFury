#include "TrashBox.h"

#include "GameInstance.h"

CTrashBox::CTrashBox(LPDIRECT3DDEVICE9 pGraphicDev) :
    CGameObject { pGraphicDev }
{
}

CTrashBox::CTrashBox(const CTrashBox& Prototype) :
    CGameObject{ Prototype }
{
}

void CTrashBox::OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta)
{
    //아마 레이충돌만 발생할거라서 여기에 뭐 안해도 됨
}

void CTrashBox::OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta, CComponent* pCollider)
{
    //여기서 레이랑 충돌판정 하고 실제로 체력이 닳아서 없어지거나 하는 표현을 보여줄 것
}

const COLLISION_DESC& CTrashBox::Get_CollisionDesc(COLLISION eColType)
{
	COLLISION_DESC Desc;
	Desc.pCollider = m_pBoxColliderCom;
	Desc.pTransform = m_pTransformCom;

	return Desc;
}

HRESULT CTrashBox::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTrashBox::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (pArg == nullptr)
		return S_OK;

	_float3* pDesc = reinterpret_cast<_float3*>(pArg);
	m_pTransformCom->Set_State(STATE::POSITION, *pDesc);

    return S_OK;
}

void CTrashBox::Priority_Update(_float fTimeDelta)
{
	//테스트용 추후 발견하시면 지워버리셔도 됩니다
	int a = 10;
}

void CTrashBox::Update(_float fTimeDelta)
{
	_float4x4 matWorldTemp = m_pGameInstance->Get_CameraWorld();

	_float3 vLook;
	memcpy(&vLook, *(_float3*)&matWorldTemp.m[2][0], sizeof(_float3));
	vLook *= -1;
	vLook.y = 0.f;
	D3DXVec3Normalize(&vLook, &vLook);

	_float3 vUp = { 0.f, 1.f, 0.f };

	_float3 vRight;
	D3DXVec3Cross(&vRight, &vUp, &vLook);
	D3DXVec3Normalize(&vRight, &vRight);

	_float4x4 matWorld = *m_pTransformCom->Get_WorldMatrixPtr();
	_float3 vPos = m_pTransformCom->Get_State(STATE::POSITION);

	memcpy(&matWorld.m[0][0], &vRight, sizeof(_float3));
	memcpy(&matWorld.m[1][0], &vUp, sizeof(_float3));
	memcpy(&matWorld.m[2][0], &vLook, sizeof(_float3));
	memcpy(&matWorld.m[3][0], &vPos, sizeof(_float3));

	m_pTransformCom->Set_Transform(matWorld);
}

void CTrashBox::Late_Update(_float fTimeDelta)
{
	Compute_CamDistance(m_pTransformCom->Get_State(STATE::POSITION));
	m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);
}

HRESULT CTrashBox::Render()
{
	Begin_RenderState();

	m_pTransformCom->Set_Transform();

	m_pTextureCom->Set_Texture(2 - m_iHp);

	m_pVIBufferCom->Render();

	End_RenderState();
	
    return S_OK;
}

HRESULT CTrashBox::Begin_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	return S_OK;
}

HRESULT CTrashBox::End_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return S_OK;
}

HRESULT CTrashBox::Ready_Components()
{
	/* 텍스쳐는 추후 세팅할 것 */
	/* Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{ 4.f, D3DXToRadian(90.0f) };
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_BoxCollider */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_BoxCollider"),
		TEXT("Com_BoxCollider"), reinterpret_cast<CComponent**>(&m_pBoxColliderCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_TrashBox"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

    return S_OK;
}

CTrashBox* CTrashBox::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTrashBox* pInstance = new CTrashBox(pGraphicDev);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

    return pInstance;
}

CGameObject* CTrashBox::Clone(void* pArg)
{
	CTrashBox* pInstance = new CTrashBox(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CTrashBox::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pBoxColliderCom);
}
