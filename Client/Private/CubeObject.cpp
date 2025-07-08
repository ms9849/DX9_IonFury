#include "CubeObject.h"

#include "GameInstance.h"

CCubeObject::CCubeObject(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLandObject{ pGraphic_Device }
{
}

CCubeObject::CCubeObject(const CCubeObject& Prototype)
	: CLandObject ( Prototype )
{
}

/*
오브젝트 아이디 추가해야됨
*/

HRESULT CCubeObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCubeObject::Initialize(void* pArg)
{
	CLandObject::LANDOBJECT_DESC			Desc{};
	Desc.pLandTransform = static_cast<CTransform*>(m_pGameInstance->Get_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_BackGround"), TEXT("Com_Transform")));
	Desc.pLandVIBuffer = static_cast<CVIBuffer*>(m_pGameInstance->Get_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_BackGround"), TEXT("Com_VIBuffer")));

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(_float3{15.f, 15.f, 15.f});
	m_pTransformCom->Rotation(m_pTransformCom->Get_State(STATE::LOOK), D3DXToRadian(45.f));

	m_pTransformCom->Set_State(STATE::POSITION, { 15.f, -2.f, 15.f });

	return S_OK;
}

void CCubeObject::Priority_Update(_float fTimeDelta)
{
}

void CCubeObject::Update(_float fTimeDelta)
{
	SetUp_OnTerrain(m_pTransformCom, 0.f);
}

void CCubeObject::Late_Update(_float fTimeDelta)
{
	// 정점의 정보를 가져왔으니까
	 
	// 1. 월드행렬이랑 곱해줘서 실제 월드 좌표 구해주기
	// 2. OBB 로직 적용해서 충돌처리
	
	// 고민거리 
	// 1. 어디서부터 어디까지 매니저로 처리?
	// 2. 1,2번 다 매니저로 처리한다면 트랜스폼과 정점의 정보를 받아와야 하나?
	// 
	// 일단 콜리전 매니저 붙이기

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CCubeObject::Render()
{
	m_pTransformCom->Set_Transform();

	m_pTextureCom->Set_Texture(0);

	m_pVIBufferCom->Render();

	return S_OK;
}

void CCubeObject::OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta)
{
}

const COLLISION_DESC& CCubeObject::Get_CollisionDesc(COLLISION eColType)
{
	COLLISION_DESC Desc;
	Desc.pTransform = m_pTransformCom;
	Desc.pCollider = m_BoxColliderCom;
	// TODO: 여기에 return 문을 삽입합니다.

	return Desc;
}

HRESULT CCubeObject::Ready_Components()
{
	/* Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{ 5.f, D3DXToRadian(90.0f) };
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	/* Com_Collider */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_BoxCollider"),
		TEXT("Com_BoxCollider"), reinterpret_cast<CComponent**>(&m_BoxColliderCom), nullptr)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_Sky"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_VIBuffer_Cube"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CCubeObject::Begin_RenderState()
{
	return S_OK;
}

HRESULT CCubeObject::End_RenderState()
{
	return S_OK;
}

CCubeObject* CCubeObject::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCubeObject* pInstance = new CCubeObject(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCubeObject::Clone(void* pArg)
{
	CCubeObject* pInstance = new CCubeObject(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCubeObject::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_BoxColliderCom);
}
