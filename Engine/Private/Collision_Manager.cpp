#include "Collision_Manager.h"

#include "GameInstance.h"
#include "Layer.h"
#include "GameObject.h"
#include "BoxCollider.h"

CCollision_Manager::CCollision_Manager() :
	m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CCollision_Manager::Initialize()
{
	return S_OK;
}

void CCollision_Manager::Check_OBBCollision(const _wstring& strLayerTagSrc, const _wstring& strLayerTagDst, _uint iLayerLevel, _float fTimeDelta)
{
	CLayer* pSrcLayer = m_pGameInstance->Find_Layer(iLayerLevel, strLayerTagSrc);
	CLayer* pDstLayer = m_pGameInstance->Find_Layer(iLayerLevel, strLayerTagDst);

	if (pSrcLayer == nullptr || pDstLayer == nullptr)
		return;

	list<CGameObject*> GameObjectSrc = pSrcLayer->Get_GameObjects();
	list<CGameObject*> GameObjectDst = pDstLayer->Get_GameObjects();

	if (GameObjectSrc.empty() || GameObjectDst.empty())
		return;

	for (auto& pSrc : GameObjectSrc)
	{
		for (auto& pDst : GameObjectDst)
		{
			_float3 vMTV = {};
			if (OBB_Collision(pSrc, pDst, &vMTV))
			{
 				CTransform* pTransformDst = static_cast<CTransform*>(pDst->Find_Component(TEXT("Com_Transform")));
				pTransformDst->Add_Pos(vMTV);

                pSrc->OnCollision(pDst, COLLISION::BOX, fTimeDelta);
                pDst->OnCollision(pSrc, COLLISION::BOX, fTimeDelta);
			}
		}
	}
}

void CCollision_Manager::Check_AABBCollision(const _wstring& strLayerTagSrc, const _wstring& strLayerTagDst, _uint iLayerLevel, _float fTimeDelta)
{
    CLayer* pSrcLayer = m_pGameInstance->Find_Layer(iLayerLevel, strLayerTagSrc);
    CLayer* pDstLayer = m_pGameInstance->Find_Layer(iLayerLevel, strLayerTagDst);

    if (pSrcLayer == nullptr || pDstLayer == nullptr)
        return;

    list<CGameObject*> GameObjectSrc = pSrcLayer->Get_GameObjects();
    list<CGameObject*> GameObjectDst = pDstLayer->Get_GameObjects();

    if (GameObjectSrc.empty() || GameObjectDst.empty())
        return;

    for (auto& pSrc : GameObjectSrc)
    {
        for (auto& pDst : GameObjectDst)
        {
            _float3 vMTV = {0.f, 0.f, 0.f};
            if (AABB_Collision(pSrc, pDst, &vMTV))
            {
                //Src쪽이 보정을 받게된다.
                CTransform* pTransformSrc = static_cast<CTransform*>(pSrc->Find_Component(TEXT("Com_Transform")));
                pTransformSrc->Add_Pos(vMTV);

                pSrc->OnCollision(pDst, COLLISION::BOX, fTimeDelta);
                pDst->OnCollision(pSrc, COLLISION::BOX, fTimeDelta);
            }
        }
    }
}

void CCollision_Manager::Check_SphereCollision(const _wstring& strLayerTagSrc, const _wstring& strLayerTagDst, _uint iLayerLevel, _float fTimeDelta)
{
    CLayer* pSrcLayer = m_pGameInstance->Find_Layer(iLayerLevel, strLayerTagSrc);
    CLayer* pDstLayer = m_pGameInstance->Find_Layer(iLayerLevel, strLayerTagDst);

    if (pSrcLayer == nullptr || pDstLayer == nullptr)
        return;

    list<CGameObject*> GameObjectSrc = pSrcLayer->Get_GameObjects();
    list<CGameObject*> GameObjectDst = pDstLayer->Get_GameObjects();

    if (GameObjectSrc.empty() || GameObjectDst.empty())
        return;

    for (auto& pSrc : GameObjectSrc)
    {
        for (auto& pDst : GameObjectDst)
        {
            if (Sphere_Collision(pSrc, pDst))
            {
                pSrc->OnCollision(pDst, COLLISION::SPHERE, fTimeDelta);
                pDst->OnCollision(pSrc, COLLISION::SPHERE, fTimeDelta);
            }
        }
    }
}

/*
현재 프레임, 그리고 다음 프레임에 레이가 안에 있다면
*/
void CCollision_Manager::Check_RayCollision(const _float3& vPos, const _float3& vLook, const _wstring& strLayerTagDst, _uint iLayerLevel, _float3* vColisionPos)
{
    CLayer* pDstLayer = m_pGameInstance->Find_Layer(iLayerLevel, strLayerTagDst);

    if (pDstLayer == nullptr)
        return;

    list<CGameObject*> GameObjectDst = pDstLayer->Get_GameObjects();

    if (GameObjectDst.empty())
        return;

    for (auto& pDst : GameObjectDst)
    {
        /*
        * vCollisionPos가 이전 값과 같으면 그냥 좀 길게 쏘도록 해야지?
        */
        if (Look_Collision(pDst, vColisionPos, vPos, vLook))
            int a = 10;
    }
}

void CCollision_Manager::Check_RayToAABBCollision(const _wstring& strLayerTagRay, const _wstring& strLayerTagAABB, _uint iLayerLevel, _float fTimeDelta, _float3* vColisionPos)
{
    CLayer* pRayLayer = m_pGameInstance->Find_Layer(iLayerLevel, strLayerTagRay);
    CLayer* pAABBLayer = m_pGameInstance->Find_Layer(iLayerLevel, strLayerTagAABB);

    if (pRayLayer == nullptr || pAABBLayer == nullptr)
        return;

    list<CGameObject*> GameObjectRay = pRayLayer->Get_GameObjects();
    list<CGameObject*> GameObjectAABB = pAABBLayer->Get_GameObjects();

    if (GameObjectRay.empty() || GameObjectAABB.empty())
        return;

    _float3 vPos = {};
    _float3 vPlaneNormal = {};
    CComponent* pCollider = {};

    for (auto& pSrc : GameObjectRay)
    {
        for (auto& pDst : GameObjectAABB)
        {
            if (RayToAABB_Collision(pSrc, pDst, &vPos, &pCollider, fTimeDelta, &vPlaneNormal))
            {
                pSrc->OnCollision(pDst, COLLISION::RAY, fTimeDelta, pCollider, vPos, vPlaneNormal);
                pDst->OnCollision(pSrc, COLLISION::RAY, fTimeDelta, pCollider, vPos);
            }
        }
    }
}

_bool CCollision_Manager::Sphere_Collision(CGameObject* pSrc, CGameObject* pDst)
{
    COLLISION_DESC DescSrc = pSrc->Get_CollisionDesc(COLLISION::SPHERE);
    COLLISION_DESC DescDst = pDst->Get_CollisionDesc(COLLISION::SPHERE);

    CTransform* pTransformSrc = DescSrc.pTransform;
    CSphereCollider* pColliderSrc = static_cast<CSphereCollider*>(DescSrc.pCollider);

    CTransform* pTransformDst = DescDst.pTransform;
    CSphereCollider* pColliderDst = static_cast<CSphereCollider*>(DescDst.pCollider);

    _float4x4 matWorldSrc = *pTransformSrc->Get_WorldMatrixPtr();
    _float4x4 matWorldDst = *pTransformDst->Get_WorldMatrixPtr();

    // 두 정점 간의 거리 계산
    _float3 vPosDiff = *(_float3*)&matWorldDst.m[3][0] - *(_float3*)&matWorldSrc.m[3][0];


    return (pColliderSrc->Get_Radius() + pColliderDst->Get_Radius()) > D3DXVec3Length(&vPosDiff);
}

_bool CCollision_Manager::OBB_Collision(CGameObject* pSrc, CGameObject* pDst, _float3* vMTV)
{
    /*
    현재 문제점 ->
    
    1, 콜라이더에 객체 자신의 Scale 값이 반영됨
       월드 행렬 normalize 하면 되긴 하는데 굳이? 싶긴함..
       객체 덩치 커지면 콜라이더도 커지는게 맞고..

    2. 연산량이 엄청남..
       축을 줄이던가 미리 가지고 있을 순 없나?
    */
    COLLISION_DESC DescSrc = pSrc->Get_CollisionDesc(COLLISION::BOX);
    COLLISION_DESC DescDst = pDst->Get_CollisionDesc(COLLISION::BOX);

    CTransform* pTransformSrc = DescSrc.pTransform;
    CBoxCollider* pColliderSrc = static_cast<CBoxCollider*>(DescSrc.pCollider);

    CTransform* pTransformDst = DescDst.pTransform;
    CBoxCollider* pColliderDst = static_cast<CBoxCollider*>(DescDst.pCollider);

    _float4x4 matWorldSrc = *pTransformSrc->Get_WorldMatrixPtr();
    _float4x4 matWorldDst = *pTransformDst->Get_WorldMatrixPtr();

    // 두 정점 간의 거리 계산
    _float3 vPosDiff = *(_float3*)&matWorldDst.m[3][0] - *(_float3*)&matWorldSrc.m[3][0];

    /*
        Src의 right up look 3개 
        Dst의 right up look 3개
        Src 축과 Dst 축 외적한 9개의 축을 추가하여 검사. MVT의 방향과 길이를 구해낸다.
    */
    vector<_float3> vAxises;

    _float3 vAxisSrc[3], vAxisDst[3];

    for (int i = 0; i < 3; ++i)
    {
        vAxisSrc[i] = *(_float3*)&matWorldSrc.m[i][0];
        vAxisDst[i] = *(_float3*)&matWorldDst.m[i][0];
        D3DXVec3Normalize(&vAxisSrc[i], &vAxisSrc[i]);
        D3DXVec3Normalize(&vAxisDst[i], &vAxisDst[i]);

        vAxises.push_back(vAxisSrc[i]);
        vAxises.push_back(vAxisDst[i]); 
    }

    // 외적 축 9개 추가
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            _float3 crossAxis;
            D3DXVec3Cross(&crossAxis, &vAxisSrc[i], &vAxisDst[j]);

            /*
            벡터의 길이가 0이라면 투영이 의미 없는거니까..
            */
            if(D3DXVec3Length(&crossAxis) != 0)
            {
                D3DXVec3Normalize(&crossAxis, &crossAxis);
                vAxises.push_back(crossAxis);
            }
        }
    }

    // 콜라이더 월드 변환
    _float3 vWorldSrc[8], vWorldDst[8];
    for (_uint i = 0; i < 8; ++i)
    {
        D3DXVec3TransformCoord(&vWorldSrc[i], &pColliderSrc->Get_LocalPos(i), &matWorldSrc);
        D3DXVec3TransformCoord(&vWorldDst[i], &pColliderDst->Get_LocalPos(i), &matWorldDst);
    }

    // SAT
    _float vMinMTVDist = FLT_MAX;
    _float3 vMTVDir = {};

    for (auto& pAxis : vAxises)
    {
        _float fMinDistSrc = FLT_MAX, fMaxDistSrc = -FLT_MAX;
        _float fMinDistDst = FLT_MAX, fMaxDistDst = -FLT_MAX;

        _float fProjLength;

        // Src 
        for (_uint i = 0; i < 8; ++i)
        {
            fProjLength = D3DXVec3Dot(&vWorldSrc[i], &pAxis);
            fMinDistSrc = min(fMinDistSrc, fProjLength);
            fMaxDistSrc = max(fMaxDistSrc, fProjLength);
        }

        // Dst
        for (int i = 0; i < 8; ++i)
        {
            fProjLength = D3DXVec3Dot(&vWorldDst[i], &pAxis);
            fMinDistDst = min(fMinDistDst, fProjLength);
            fMaxDistDst = max(fMaxDistDst, fProjLength);
        }

        // 겹치지 않으면 충돌 X
        if (fMaxDistSrc < fMinDistDst || fMaxDistDst < fMinDistSrc)
            return false;

        // 겹치는 길이 계산
        _float vMTVDist = min(fMaxDistSrc, fMaxDistDst) - max(fMinDistSrc, fMinDistDst);
        if (vMTVDist < vMinMTVDist)
        {
            vMinMTVDist = vMTVDist;
            vMTVDir = pAxis;

            // 방향 결정 (vPosDiff에 따라 MTV 방향 정해줌)
            if (D3DXVec3Dot(&vPosDiff, &vMTVDir) < 0.f)
                vMTVDir *= -1.f;
        }
    }

    *vMTV = vMTVDir * vMinMTVDist;
    return true;
}

_bool CCollision_Manager::AABB_Collision(CGameObject* pSrc, CGameObject* pDst, _float3* vMTV)
{
    /*
    AABB 충돌 구현.
    */
    if (pSrc == pDst) return false;

    COLLISION_DESC DescSrc = pSrc->Get_CollisionDesc(COLLISION::BOX);
    COLLISION_DESC DescDst = pDst->Get_CollisionDesc(COLLISION::BOX);

    CTransform* pTransformSrc = DescSrc.pTransform;
    CBoxCollider* pColliderSrc = static_cast<CBoxCollider*>(DescSrc.pCollider);

    CTransform* pTransformDst = DescDst.pTransform;
    CBoxCollider* pColliderDst = static_cast<CBoxCollider*>(DescDst.pCollider);

    /*
    Min Max 꺼내와서 Pos값만 더해줘서, 축 정렬 된 상태로 월드 스페이스까지 끌어올림.
    회전이나 스케일값은 따로 적용시키지 않는다.
    */
    _float3 vMinSrc = pColliderSrc->Get_Min();
    _float3 vMaxSrc = pColliderSrc->Get_Max();

    _float3 vMinDst = pColliderDst->Get_Min();
    _float3 vMaxDst = pColliderDst->Get_Max();

    _float3 vPosSrc = pTransformSrc->Get_State(STATE::POSITION);
    _float3 vPosDst = pTransformDst->Get_State(STATE::POSITION);

    vMinSrc += vPosSrc;
    vMaxSrc += vPosSrc;
    
    vMinDst += vPosDst;
    vMaxDst += vPosDst;

    /*
    AABB 로직? -> OBB 하위호환. 가장 큰 fMin과 가장 작은 fMax를 비교해서 부등호가 > 라면 충돌.
    보정값도 fMin - fMax, MTV도 구해낼 수 있다.
    (fMin과 fMax를 벡터의 형태로 가지고 있다면 보정값의 부호도 알아서 바뀌니까 신경 안써도 됨)
    */
    
    _bool isXOverlap = (vMinSrc.x <= vMaxDst.x && vMaxSrc.x >= vMinDst.x);
    _float fXDist = vPosSrc.x < vPosDst.x ? vMinDst.x - vMaxSrc.x : vMaxDst.x - vMinSrc.x;

    _bool isYOverlap = (vMinSrc.y <= vMaxDst.y && vMaxSrc.y >= vMinDst.y);
    _float fYDist = vPosSrc.y < vPosDst.y ? vMaxSrc.y - vMinDst.y : vMaxDst.y - vMinSrc.y;

    _bool isZOverlap = (vMinSrc.z <= vMaxDst.z && vMaxSrc.z >= vMinDst.z);
    _float fZDist = vPosSrc.z < vPosDst.z ? vMinDst.z - vMaxSrc.z : vMaxDst.z - vMinSrc.z;


    /*  보정이 들어가는 부분. 
        비교하는 두 대상 중 어느 대상이 상대적으로 더 x, z가 
        큰지 확인하여 보정값의 방향을 설정해준다.
    */
    if (isXOverlap && isYOverlap && isZOverlap)
    {
        if (fabsf(fXDist) < fabsf(fYDist) && fabsf(fXDist) < fabsf(fZDist))
            vMTV->x = fXDist;
        
        else if (fabsf(fZDist) < fabsf(fYDist) && fabsf(fZDist) < fabsf(fXDist))
            vMTV->z = fZDist;

        return true;
    }
  
    return false;
}

_bool CCollision_Manager::Look_Collision(CGameObject* pDst, _float3* vCollisionPos, _float3 vPos, _float3 vLook)
{
    CVIBuffer* pBuffer = static_cast<CVIBuffer*>(pDst->Find_Component(TEXT("Com_VIBuffer")));
    CTransform* pTransform = static_cast<CTransform*>(pDst->Find_Component(TEXT("Com_Transform")));

    return (pBuffer->Picking(pTransform, vCollisionPos, vPos, vLook));
}

/*
레이가 해당 AABB를 통과하는지만을 체크하기 때문에, 나오자마자 바로 사라지는 문제가 있다

Ray의 Pos와 AABB간의 거리를 계산하는 방법도 필요할 것이다.

=============================
총알과 몬스터 콜라이더 간의 충돌만 고려한 상태임.

총알과 벽 충돌은 어떻게 할지 고민좀 해봐야 함..
*/
_bool CCollision_Manager::RayToAABB_Collision(CGameObject* pRay, CGameObject* pAABB, _float3* vPos, CComponent** pCollider, _float fTimeDelta, _float3* vPlaneNormal)
{
    /*
    레이 정보 가져오기

    현 프레임과 다음 프레임의 레이 정보를 비교하여 둘 중 하나만 들어가 있어도 충돌로 체크해주면,
    어느정도 고속으로 발사하더라도 괜찮을 것이다.
    */

    //상수값을 주는게 진짜 안전한지 고민좀 해봐야 함 
    RAY_DESC RayDesc = pRay->Get_RayDesc();
    _float3 vRayDir = RayDesc.vDir;
    _float3 vRayPos = RayDesc.vPos; 
    _float3 vRayPosAfter = vRayPos + vRayDir * RayDesc.fSpeed * fTimeDelta;

    // 정규화된 상태니까, 스피드까지 곱해줘서 처리해버리자
    // -> 스피드 곱해버리니까 너무 빠름, 그냥 짧게 쏘자.

    /*
    AABB 충돌체 정보 가져오기.
    */
    COLLISION_DESC CollisionDesc = pAABB->Get_CollisionDesc(COLLISION::BOX);
    CTransform* pColliderTransform;
    CBoxCollider* pColliderBox;
    _float3 vPosDst;
    _float3 vColliderMin, vColliderMax;
    _float t1, t2, t3, t4, t5, t6;
    _float fMin, fMax;
    _float fDistance;

    /* 두번째 콜라이더가 nullptr이 아니라면 두번째 콜라이더에 대해서도 검사한다.*/
    if (CollisionDesc.pColliderSecond != nullptr)
    {
        pColliderTransform = CollisionDesc.pTransform;
        pColliderBox = static_cast<CBoxCollider*>(CollisionDesc.pColliderSecond);
        vPosDst = pColliderTransform->Get_State(STATE::POSITION);

        vColliderMin = pColliderBox->Get_Min();
        vColliderMax = pColliderBox->Get_Max();

        vColliderMin += vPosDst;
        vColliderMax += vPosDst;

        t1 = (vColliderMin.x - vRayPos.x) / vRayDir.x;
        t2 = (vColliderMax.x - vRayPos.x) / vRayDir.x;

        t3 = (vColliderMin.y - vRayPos.y) / vRayDir.y;
        t4 = (vColliderMax.y - vRayPos.y) / vRayDir.y;

        t5 = (vColliderMin.z - vRayPos.z) / vRayDir.z;
        t6 = (vColliderMax.z - vRayPos.z) / vRayDir.z;

        /* 가장 큰 min 값을 구해낸다. */
        fMin = fmaxf(fmaxf(fminf(t1, t2), fminf(t3, t4)), fminf(t5, t6));

        /* 가장 작은 max 값을 구해낸다. */
        fMax = fminf(fminf(fmaxf(t1, t2), fmaxf(t3, t4)), fmaxf(t5, t6));

        /* 머리 우선 판정 해줘서 조금 널널하게 .*/
        fDistance = fMin < 0.f ? fMax : fMin;

        //if (fMax < 0.f || fMin > fMax)
        //    return false;
        if (!(fMax < 0.f || fMin > fMax) && fDistance < 0.5f)
        {
            *pCollider = CollisionDesc.pColliderSecond;
            *vPos = vRayPos + vRayDir * fDistance; // 충돌 위치
            return true;
        }
    }

    /* 첫 콜라이더에 대한 검사 수행 */
    pColliderTransform = CollisionDesc.pTransform;
    pColliderBox = static_cast<CBoxCollider*>(CollisionDesc.pCollider);
    vPosDst = pColliderTransform->Get_State(STATE::POSITION);

    vColliderMin = pColliderBox->Get_Min();
    vColliderMax = pColliderBox->Get_Max();

    vColliderMin += vPosDst;
    vColliderMax += vPosDst;

    // 콜라이더의 Min Max는 구할 수 있었는데..
    // 레이 정보를 어떻게 가져오지?

    t1 = (vColliderMin.x - vRayPos.x) / vRayDir.x;
    t2 = (vColliderMax.x - vRayPos.x) / vRayDir.x;

    t3 = (vColliderMin.y - vRayPos.y) / vRayDir.y;
    t4 = (vColliderMax.y - vRayPos.y) / vRayDir.y;

    t5 = (vColliderMin.z - vRayPos.z) / vRayDir.z;
    t6 = (vColliderMax.z - vRayPos.z) / vRayDir.z;

    /* 가장 큰 min 값을 구해낸다. */
    fMin = fmaxf(fmaxf(fminf(t1, t2), fminf(t3,t4)), fminf(t5, t6));

    /* 가장 큰 max 값을 구해낸다. */
    fMax = fminf(fminf(fmaxf(t1, t2), fmaxf(t3, t4)), fmaxf(t5, t6));

    if (fMax < 0.f || fMin > fMax)
        return false;

    // 길이 구하는 함수. 아직 이해 못함..
    fDistance = fMin < 0.f ? fMax : fMin;

    if (fDistance < 0.5f)
    {
        _float3 vFirst, vSecond;
        *pCollider = CollisionDesc.pCollider;
        *vPos = vRayPos + vRayDir * fDistance; // 충돌 위치
        
        _float3 vAABBPoints[8] = {
            { vColliderMin.x, vColliderMax.y, vColliderMin.z },
            { vColliderMax.x, vColliderMax.y, vColliderMin.z },
            { vColliderMax.x, vColliderMin.y, vColliderMin.z },
            { vColliderMin.x, vColliderMin.y, vColliderMin.z },
            { vColliderMin.x, vColliderMax.y, vColliderMax.z },
            { vColliderMax.x, vColliderMax.y, vColliderMax.z },
            { vColliderMax.x, vColliderMin.y, vColliderMax.z },
            { vColliderMin.x, vColliderMin.y, vColliderMax.z }
        };

        /* 면 1번 0,1,2  */
        if (vPos->z == vColliderMin.z)
        {
            vFirst = vAABBPoints[0] - vAABBPoints[1];
            vSecond = vAABBPoints[1] - vAABBPoints[2];
        }

        /* 면 2번 1,5,6 */
        else if (vPos->x == vColliderMax.x)
        {
            vFirst = vAABBPoints[1] - vAABBPoints[5];
            vSecond = vAABBPoints[5] - vAABBPoints[6];
        }

        /* 면 3번 2,6,7 */
        else if (vPos->y == vColliderMin.y)
        {
            vFirst = vAABBPoints[2] - vAABBPoints[6];
            vSecond = vAABBPoints[6] - vAABBPoints[7];
        }

        /* 면 4번 7,4,0, */
        else if (vPos->x == vColliderMin.x)
        {
            vFirst = vAABBPoints[7] - vAABBPoints[4];
            vSecond = vAABBPoints[4] - vAABBPoints[0];
        }

        /* 면 5번 6,5,4 */
        else if (vPos->z == vColliderMax.z)
        {
            vFirst = vAABBPoints[6] - vAABBPoints[5];
            vSecond = vAABBPoints[5] - vAABBPoints[4];
        }

        /* 면 6번 4,5,1 */
        else if (vPos->y == vColliderMax.y)
        {
            vFirst = vAABBPoints[4] - vAABBPoints[5];
            vSecond = vAABBPoints[5] - vAABBPoints[1];
        }

        //평면의 법선을 구해냈다. 
        // 자신의 위치 + 법선 쪽을 바라보도록 총흔을 생성하면 될 것이다.
        D3DXVec3Cross(vPlaneNormal, &vFirst, &vSecond);
        return true;
    }

    return false;
}

_bool CCollision_Manager::Check_inAABB(const _float3& vPos, const _float3& vMin, const _float3 vMax)
{
    return   (vPos.x >= vMin.x && vPos.x <= vMax.x) &&
             (vPos.y >= vMin.y && vPos.y <= vMax.y) &&
             (vPos.z >= vMin.z && vPos.z <= vMax.z);
}

CCollision_Manager* CCollision_Manager::Create()
{
	CCollision_Manager* pInstance = new CCollision_Manager();

	if (FAILED(pInstance->Initialize()))
		Safe_Release(pInstance);

	return pInstance;
}

void CCollision_Manager::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
}

/*
_bool CCollision_Manager::RayToAABB_Collision(CGameObject* pRay, CGameObject* pAABB, _float3* vPos)
{
    레이 정보 가져오기
RAY_DESC RayDesc = pRay->Get_RayDesc();
_float3 vRayPos = RayDesc.vPos;
_float3 vRayDir = RayDesc.vDir;
// 정규화된 상태니까, 스피드까지 곱해줘서 처리해버리자
// -> 스피드 곱해버리니까 너무 빠름, 그냥 짧게 쏘자.

AABB 충돌체 정보 가져오기.
COLLISION_DESC CollisionDesc = pAABB->Get_CollisionDesc(COLLISION::BOX);
CTransform* pColliderTransform = CollisionDesc.pTransform;
CBoxCollider* pColliderBox = static_cast<CBoxCollider*>(CollisionDesc.pCollider);
_float3 vPosDst = pColliderTransform->Get_State(STATE::POSITION);

_float3 vColliderMin = pColliderBox->Get_Min();
_float3 vColliderMax = pColliderBox->Get_Max();

vColliderMin += vPosDst;
vColliderMax += vPosDst;

// 콜라이더의 Min Max는 구할 수 있었는데..
// 레이 정보를 어떻게 가져오지?

_float t1 = (vColliderMin.x - vRayPos.x) / vRayDir.x;
_float t2 = (vColliderMax.x - vRayPos.x) / vRayDir.x;

_float t3 = (vColliderMin.y - vRayPos.y) / vRayDir.y;
_float t4 = (vColliderMax.y - vRayPos.y) / vRayDir.y;

_float t5 = (vColliderMin.z - vRayPos.z) / vRayDir.z;
_float t6 = (vColliderMax.z - vRayPos.z) / vRayDir.z;

가장 큰 min 값을 구해낸다.
_float fMin = fmaxf(fmaxf(fmin(t1, t2), fmin(t3, t4)), fminf(t5, t6));

가장 큰 max 값을 구해낸다.
_float fMax = fminf(fminf(fmax(t1, t2), fmax(t3, t4)), fmaxf(t5, t6));


if (fMax < 0.f)
    return false;

if (fMin > fMax)
return false;

if (fMin < 0.f)
    return true;

return true;
}

*/


/*
_bool CCollision_Manager::RayToAABB_Collision(CGameObject* pRay, CGameObject* pAABB, _float3* vPos, CComponent** pCollider, _float fTimeDelta)
{

RAY_DESC RayDesc = pRay->Get_RayDesc();
_float3 vRayDir = RayDesc.vDir;
_float3 vRayPos = RayDesc.vPos;
_float3 vRayPosAfter = vRayPos + vRayDir * RayDesc.fSpeed * fTimeDelta * RayDesc.fSpeed;



COLLISION_DESC CollisionDesc = pAABB->Get_CollisionDesc(COLLISION::BOX);

if (CollisionDesc.pColliderSecond != nullptr)
{
    CTransform* pColliderTransform = CollisionDesc.pTransform;
    CBoxCollider* pColliderBox = static_cast<CBoxCollider*>(CollisionDesc.pColliderSecond);
    _float3 vPosDst = pColliderTransform->Get_State(STATE::POSITION);

    _float3 vColliderMin = pColliderBox->Get_Min();
    _float3 vColliderMax = pColliderBox->Get_Max();

    vColliderMin += vPosDst;
    vColliderMax += vPosDst;

    _float t1 = (vColliderMin.x - vRayPos.x) / vRayDir.x;
    _float t2 = (vColliderMax.x - vRayPos.x) / vRayDir.x;

    _float t3 = (vColliderMin.y - vRayPos.y) / vRayDir.y;
    _float t4 = (vColliderMax.y - vRayPos.y) / vRayDir.y;

    _float t5 = (vColliderMin.z - vRayPos.z) / vRayDir.z;
    _float t6 = (vColliderMax.z - vRayPos.z) / vRayDir.z;

    _float fMin = fmaxf(fmaxf(fminf(t1, t2), fminf(t3, t4)), fminf(t5, t6));

    _float fMax = fminf(fminf(fmaxf(t1, t2), fmaxf(t3, t4)), fmaxf(t5, t6));

    _float fDistance = fMin < 0.f ? fMax : fMin;

    if (!(fMax < 0.f || fMin > fMax) && (Check_inAABB(vRayPos, vColliderMin, vColliderMax) || Check_inAABB(vRayPosAfter, vColliderMin, vColliderMax)))
    {
        *pCollider = CollisionDesc.pColliderSecond;
        return true;
    }
}

CTransform* pColliderTransform = CollisionDesc.pTransform;
CBoxCollider* pColliderBox = static_cast<CBoxCollider*>(CollisionDesc.pCollider);
_float3 vPosDst = pColliderTransform->Get_State(STATE::POSITION);

_float3 vColliderMin = pColliderBox->Get_Min();
_float3 vColliderMax = pColliderBox->Get_Max();

vColliderMin += vPosDst;
vColliderMax += vPosDst;


_float t1 = (vColliderMin.x - vRayPos.x) / vRayDir.x;
_float t2 = (vColliderMax.x - vRayPos.x) / vRayDir.x;

_float t3 = (vColliderMin.y - vRayPos.y) / vRayDir.y;
_float t4 = (vColliderMax.y - vRayPos.y) / vRayDir.y;

_float t5 = (vColliderMin.z - vRayPos.z) / vRayDir.z;
_float t6 = (vColliderMax.z - vRayPos.z) / vRayDir.z;

_float fMin = fmaxf(fmaxf(fminf(t1, t2), fminf(t3, t4)), fminf(t5, t6));

_float fMax = fminf(fminf(fmaxf(t1, t2), fmaxf(t3, t4)), fmaxf(t5, t6));


if (fMax < 0.f || fMin > fMax)
return false;




if (Check_inAABB(vRayPos, vColliderMin, vColliderMax) || Check_inAABB(vRayPosAfter, vColliderMin, vColliderMax))
{
    *pCollider = CollisionDesc.pCollider;
    return true;
}

return false;
}
*/