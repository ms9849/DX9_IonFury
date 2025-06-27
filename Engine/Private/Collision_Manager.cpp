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

void CCollision_Manager::Check_OBBCollision(const _wstring& strLayerTagSrc, const _wstring& strLayerTagDst, _uint iLayerLevel)
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
			}
		}
	}

}

void CCollision_Manager::Check_RayCollision()
{
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
    CTransform* pTransformSrc = static_cast<CTransform*>(pSrc->Find_Component(TEXT("Com_Transform")));
    CTransform* pTransformDst = static_cast<CTransform*>(pDst->Find_Component(TEXT("Com_Transform")));

    CBoxCollider* pColliderSrc = static_cast<CBoxCollider*>(pSrc->Find_Component(TEXT("Com_BoxCollider")));
    CBoxCollider* pColliderDst = static_cast<CBoxCollider*>(pDst->Find_Component(TEXT("Com_BoxCollider")));

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
