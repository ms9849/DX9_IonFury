#include "BossGrenade.h"
#include "GameInstance.h"
#include "Camera.h"

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

	if (FAILED(Ready_Animations()))
		return E_FAIL;

	if (m_pDesc == nullptr)
		return S_OK;
	
	m_vDir = pDesc->vDir;							// 몬스터 + 오프셋 to Player 방향벡터
	m_bPlayerBullet = pDesc->isPlayerBullet;
	m_vStartPos = pDesc->vStartPos;
	m_vPlayerPos = pDesc->vPlayerPos;
	m_fAngle = pDesc->fAngle;
	m_vScale = pDesc->vBulletScale;
	m_pPlayerTransform = pDesc->pPlayerTransform;
	m_fDuration = pDesc->fDuration;

	//Safe_AddRef(m_pPlayerTransform);

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

	m_strFrameKey = TEXT("Grenade_Explosion");

	return S_OK;
}

void CBossGrenade::Priority_Update(_float fTimeDelta)
{
}

void CBossGrenade::Update(_float fTimeDelta)
{
	m_fSumTime += fTimeDelta;
	if (m_fSumTime >= m_fDuration)
		m_isDead = true;

	// 0.5는 임시값 나중에 바꿔야함
	if (m_pTransformCom->Get_State(STATE::POSITION).y - 0.5f <= m_vPlayerPos.y || m_bAnimateionOn)			// 유탄이 플레이어의 시작 지점보다 낮아지면 폭발 모습 보이게
	{
		// 빌보드 효과를 줘서 플레이어가 어디서든 항상 터지는 애니메이션을 볼 수 있도록 하자
		if (!m_bScaleChange)
		{
			m_bScaleChange = true;
			m_pTransformCom->Set_Scale({ 7.f, 7.f, 7.f });			// 임시 적용 나중에 파티클로 변경할것
		}

		if (!m_bExplosion)
		{
			m_bExplosion = true;
			m_pGameInstance->PlaySoundOnce(TEXT("Grenade_Explosion.ogg"), CHANNELID::SOUND_EFFECT, 0.7f);
			static_cast<CCamera*>(m_pGameInstance->Find_GameObject_ToLayer(ENUM_CLASS(LEVEL::BOSSFIGHT), TEXT("Layer_Camera"), nullptr))->Start_Shaking(0.2f, 0.5f);
		}

		m_bAnimateionOn = true;
		m_pAnimationCom->Play_Animation(m_strFrameKey, fTimeDelta);
		if (m_pAnimationCom->Check_Animation_Finish(m_strFrameKey))
		{
			m_isDead = true;
		}

		return;
	}

	m_pTransformCom->Set_State(STATE::POSITION, { m_vStartPos.x + (m_vVelocity.x * m_fSumTime),
		m_vStartPos.y + (m_vVelocity.y * m_fSumTime) + ( - 0.5f * m_fGravity * m_fSumTime * m_fSumTime),
		m_vStartPos.z + (m_vVelocity.z * m_fSumTime) });
}

void CBossGrenade::Late_Update(_float fTimeDelta)
{
	Compute_CamDistance(m_pTransformCom->Get_State(STATE::POSITION));
	m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);
}

HRESULT CBossGrenade::Render()
{
	//m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	if (FAILED(Begin_RenderState()))
		return E_FAIL;
	//m_pTransformCom->Set_Transform();

	RotateToPlayer(m_pTransformCom);

	if(m_bPlayerBullet)
		m_pTextureCom->Set_Texture(0);
	else
		m_pTextureCom->Set_Texture(0);

	//m_pTransformCom->Set_Transform();

	if (m_bAnimateionOn)
	{
		auto iter = m_pTextureComs.find(m_strFrameKey);
		iter->second->Set_Texture(m_pAnimationCom->Get_Frame_Current_Index(m_strFrameKey));
		_uint num = m_pAnimationCom->Get_Frame_Current_Index(m_strFrameKey);
		//m_pVIBufferCom->Render();
		m_pVIBufferCom_Rect->Render();
	}
	else
		m_pVIBufferCom_Rect->Render();
	/*else
		m_pVIBufferCom->Render();*/

	/*auto iter = m_pTextureComs.find(m_strFrameKey);
	iter->second->Set_Texture(m_pAnimationCom->Get_Frame_Current_Index(m_strFrameKey));
	_uint num = m_pAnimationCom->Get_Frame_Current_Index(m_strFrameKey);
	m_pVIBufferCom->Render();*/

	if (FAILED(End_RenderState()))
		return E_FAIL;
	/*m_pVIBufferCom->Render();*/
	//m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	return S_OK;
}

void CBossGrenade::RotateToPlayer(CTransform* pTranform)
{
	_float4x4 matWorldTemp = m_pGameInstance->Get_CameraWorld();

	_float3 vLook = { matWorldTemp._31, matWorldTemp._32, matWorldTemp._33 };
	vLook *= -1;
	vLook.y = 0.f;
	D3DXVec3Normalize(&vLook, &vLook);

	_float3 vUp = { 0.f, 1.f, 0.f };

	_float3 vRight;
	D3DXVec3Cross(&vRight, &vUp, &vLook);
	D3DXVec3Normalize(&vRight, &vRight);

	_float3 scale = pTranform->Get_Scaled();

	vRight *= scale.x;
	vUp *= scale.y;
	vLook *= scale.z;

	_float4x4 matWorld = *pTranform->Get_WorldMatrixPtr();
	_float3 vPos = pTranform->Get_State(STATE::POSITION);

	memcpy(&matWorld.m[0][0], &vRight, sizeof(_float3));
	memcpy(&matWorld.m[1][0], &vUp, sizeof(_float3));
	memcpy(&matWorld.m[2][0], &vLook, sizeof(_float3));
	memcpy(&matWorld.m[3][0], &vPos, sizeof(_float3));

	pTranform->Set_Transform(matWorld);
}

void CBossGrenade::OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta)
{
	//m_isDead = true;
	// 충돌 했으면 애니메이션 출력을 하도록 bool값을 변경
	m_bAnimateionOn = true;
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
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Grenade"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* Com_Texture */
	for (size_t i = 0; i < (sizeof(m_strFrameKeys) / sizeof(m_strFrameKeys[0])); ++i)
	{
		_tchar strPrototypeTag[256];
		_tchar strComponentTag[256];

		CTexture* pTextureCom{ nullptr };

		wsprintf(strPrototypeTag, TEXT("Prototype_Component_Texture_%s"), m_strFrameKeys[i].c_str());
		wsprintf(strComponentTag, TEXT("Com_%s_Texture"), m_strFrameKeys[i].c_str());

		if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), strPrototypeTag,
			strComponentTag, reinterpret_cast<CComponent**>(&pTextureCom))))
			return E_FAIL;

		m_pTextureComs.emplace(m_strFrameKeys[i], pTextureCom);
	}

	/* Com_Animation */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Animation"),
		TEXT("Com_Animation"), reinterpret_cast<CComponent**>(&m_pAnimationCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	/*if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_VIBuffer_Cube"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;*/

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer_Rect"), reinterpret_cast<CComponent**>(&m_pVIBufferCom_Rect))))
		return E_FAIL;

	/* Com_SphereCollider */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_SphereCollider"),
		TEXT("Com_SphereCollider"), reinterpret_cast<CComponent**>(&m_pSphereColliderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CBossGrenade::Ready_Animations()
{
	CAnimation::FRAME_DESC Desc_0{};

	//Grenade_Explosion
	auto iter = m_pTextureComs.find(TEXT("Grenade_Explosion"));
	Desc_0.iFrameSpeed = 5;
	Desc_0.iEnd = iter->second->Get_Texture_Length();
	m_pAnimationCom->Set_Animation(TEXT("Grenade_Explosion"), Desc_0);

	return S_OK;
}

HRESULT CBossGrenade::Begin_RenderState()
{
	/* 렌더링할 때 알파값을 기준으로 섞어준다.*/

	/*
	float4		vSourColor, vDestColor;
	vSourColor.rgb * vSourColor.a + vDestColor.rgb * (1.f - vSourColor.a);
	*/


	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	//m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE);

	/* 알파 테스트 : 픽셀의 알파를 비교해서 그린다 안그린다를 설정. */
	/*m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);*/



	return S_OK;
}

HRESULT CBossGrenade::End_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	//m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	//m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

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

	for (auto& iter : m_pTextureComs)
	{
		Safe_Release(iter.second);
	}
	m_pTextureComs.clear();
	
	Safe_Release(m_pVIBufferCom_Rect);
	Safe_Release(m_pAnimationCom);
	//Safe_Release(m_pPlayerTransform);
}