#include "BossGrenade.h"

#include "GameInstance.h"

CBossGrenade::CBossGrenade(LPDIRECT3DDEVICE9 pGraphicDev)
	: CBullet{ pGraphicDev }
{
}

CBossGrenade::CBossGrenade(const CBossGrenade& Prototype )
	: CBullet{ Prototype }
{
}

HRESULT CBossGrenade::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBossGrenade::Initialize(void* pArg)
{
	CBossGrenade::BULLET_DESC* pDesc = static_cast<CBossGrenade::BULLET_DESC*>(pArg);
	m_fBulletSpeed = pDesc->fBulletSpeed;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (pDesc == nullptr)
		return S_OK;
	
	m_vDir = pDesc->vDir;							// 몬스터 + 오프셋 to Player 방향벡터
	m_bPlayerBullet = pDesc->isPlayerBullet;
	m_vStartPos = pDesc->vStartPos;
	m_vPlayerPos = pDesc->vPlayerPos;
	m_fAngle = pDesc->fAngle;
	m_vScale = pDesc->vBulletScale;

	_float fAngle = D3DXToRadian(m_fAngle);

	m_pTransformCom->Set_State(STATE::POSITION, m_vStartPos);
	m_pTransformCom->LookAt(m_pTransformCom->Get_State(STATE::POSITION) + m_vDir);	
	m_pTransformCom->Set_Scale(m_vScale);

	_float hDist = sqrt(m_vDir.x * m_vDir.x + m_vDir.z * m_vDir.z);					// 수평거리
	_float height = m_vPlayerPos.y - m_vStartPos.y;													// 높이

	float v0 = sqrt((m_fGravity * hDist * hDist) / (2 * cos(fAngle) * cos(fAngle) * (hDist * tan(fAngle) - height)));
	v0 *= 7;																	// 속도 값 보정

	_float3 dirXZ = _float3(m_vDir.x, 0, m_vDir.z);								// xz평면 방향벡터
	D3DXVec3Normalize(&dirXZ, &dirXZ);

	m_vVelocity = dirXZ * v0 * cos(fAngle);
	m_vVelocity.y = v0 * sin(fAngle);
	/*CSphereCollider::SPHERECOLLIDER_DESC* pSdesc = new CSphereCollider::SPHERECOLLIDER_DESC;
	pSdesc->fRadius = 10.f;
	m_pSphereColliderCom->Initialize(pSdesc);*/

	return S_OK;
}

void CBossGrenade::Priority_Update(_float fTimeDelta)
{
}

void CBossGrenade::Update(_float fTimeDelta)
{
	//OutputDebugStringA("디버그 메시지: 유탄 업데이트 진입 완료\n");
	//m_pTransformCom->Go_Direction(m_vDir, fTimeDelta);
	m_fElapsedTime += fTimeDelta;  // 경과 시간

	/*m_pTransformCom->Set_State(STATE::POSITION, { (m_vStartPos.x + m_vOffSet.x + m_vVelocity.x) * m_fElapsedTime * 0.001f,
		(m_vStartPos.y + m_vOffSet.y + m_vVelocity.y) * m_fElapsedTime - 0.5f * m_fGravity * m_fElapsedTime * m_fElapsedTime * 0.001f,
		(m_vStartPos.z + m_vOffSet.z + m_vVelocity.z) * m_fElapsedTime * 0.001f });*/

	m_pTransformCom->Set_State(STATE::POSITION, { m_vStartPos.x + (m_vVelocity.x * m_fElapsedTime ),
		m_vStartPos.y + (m_vVelocity.y * m_fElapsedTime) + ( - 0.5f * m_fGravity * m_fElapsedTime * m_fElapsedTime),
		m_vStartPos.z + (m_vVelocity.z * m_fElapsedTime ) });

	//wchar_t szBuffer[128];
	//swprintf_s(szBuffer, 128, L"StartPos: X=%.2f Y=%.2f Z=%.2f\n", m_vStartPos.x, m_vStartPos.y, m_vStartPos.z);
	/*m_pTransformCom->Set_State(STATE::POSITION, { (m_vStartPos.x + m_vVelocity.x),
		(m_vStartPos.y + m_vVelocity.y),
		(m_vStartPos.z + m_vVelocity.z)});*/
}

void CBossGrenade::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CBossGrenade::Render()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTransformCom->Set_Transform();

	if(m_bPlayerBullet)
		m_pTextureCom->Set_Texture(0);
	else
		m_pTextureCom->Set_Texture(0);

	m_pVIBufferCom->Render();

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	return S_OK;
}

void CBossGrenade::OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta)
{
	m_isDead = true;
}

const COLLISION_DESC& CBossGrenade::Get_CollisionDesc(COLLISION eColType)
{
	COLLISION_DESC Desc;
	Desc.pTransform = m_pTransformCom;
	Desc.pCollider = m_pSphereColliderCom;

	return Desc;
}

HRESULT CBossGrenade::Ready_Components()
{
	/* Com_Transform */ //기본 속도 3
	CTransform::TRANSFORM_DESC		TransformDesc{ m_fBulletSpeed, D3DXToRadian(90.0f) };
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Bullet"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_VIBuffer_Cube"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_SphereCollider */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_SphereCollider"),
		TEXT("Com_SphereCollider"), reinterpret_cast<CComponent**>(&m_pSphereColliderCom))))
		return E_FAIL;

	return S_OK;
}

CBossGrenade* CBossGrenade::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CBossGrenade* pInstance = new CBossGrenade(pGraphicDev);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CREATE FAILED: Prototype_Boss_Grenade");
	}

	return pInstance;
}

CGameObject* CBossGrenade::Clone(void* pArg)
{
	CBossGrenade* pInstance = new CBossGrenade(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CREATE FAILED: Clone_Boss_Grenade");
	}

	return pInstance;
}

void CBossGrenade::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pSphereColliderCom);
}
