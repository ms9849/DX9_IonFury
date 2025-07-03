#include "Terrain_Manager.h"

#include "GameInstance.h"
#include "Layer.h"
#include "Terrain.h"
#include "CubeObject.h"
#include "LandObject.h"


/*
매니저에 접근할 수 있게 해야 하나?
하나의 슈퍼 클래스를 만드는게 정말 정답인지 생각해야함...

어쨌든 지형 정보들을 다 들고 하나하나 비교하는 방법은 필요하긴 하다.

CTerrain_Manager라고 가정하면

1. 매니저의 생성 자체는 클라이언트에서 하는게 확실하긴 함

2. 매니저에 담는건 터레인의 Initialize (클론생성) 에서 자기 자신을 포인터로 참조할 수 있게 한다.
-> 삭제될땐?
-> 터레인 삭제될 일 X. 애드 레프하고 릴리즈만 잘해주면 됨

3. 비교는 어떻게? ->

플레이어의 중점 아래에 레이를 쏴서 비교하도록, 그리고 레이에서 가장 가까운 지형에 타도록 한다.
*/
/*=========================== 보류  ==============================
3-1. 플레이어의 위치에 존재하는 터레인들 확인.
3-2. 현재 터레인보다 높다면 해당 터레인 탈 수 있게
3-3. 현재 터레인을 타다가 아주 미세하게 (0.03쯤?) 가까워지는
        터레인이 생긴다면 해당 터레인을 탈 수 있게 한다.
        -> 재훈이가 아마 선을 탈 수 있게 구현했던 것 같은데..

점프는 떨어지는 상태를 구분해야 할 듯

점프할 땐 y값 기준으로 소팅시켜서 가져다 세팅하고
=======================================================
1. 천장에 닿는 상황은 반드시 없어야 한다. -> 불문율임..
할거면 obb든 aabb든 콜라이더 달아서 밀어내야 한다.

7월 3일 현재 이슈

큐브에서 터레인으로 바뀌면 점프 상태로 바뀌지 않고 바로 달라붙는 문제가 있음
*/

CTerrain_Manager::CTerrain_Manager() : 
    m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

void CTerrain_Manager::Add_LandObject(LEVEL eLevelID, const _wstring& strLayerTag)
{
    list<CGameObject*> GameObjects = m_pGameInstance->Get_GameObjects_inLayer(ENUM_CLASS(eLevelID), strLayerTag);

    for (auto& iter : GameObjects)
    {
        m_LandObjects.push_back(reinterpret_cast<CLandObject*>(iter));
    }
}

void CTerrain_Manager::Add_Terrian(LEVEL eLevelID)
{
    list<CGameObject*> GameObjects = m_pGameInstance->Get_GameObjects_inLayer(ENUM_CLASS(eLevelID), TEXT("Layer_BackGround"));

    /* Terrain 담기 */
    for (auto& iter : GameObjects)
    {
        m_Terrains.push_back(reinterpret_cast<CTerrain*>(iter));
    }
}

void CTerrain_Manager::Add_Cube(LEVEL eLevelID)
{
    list<CGameObject*> GameObjects = m_pGameInstance->Get_GameObjects_inLayer(ENUM_CLASS(eLevelID), TEXT("Layer_Cube"));

    /* Cube 담기 */
    for (auto& iter : GameObjects)
    {
        m_CubeObjects.push_back(reinterpret_cast<CCubeObject*>(iter));
    }
}

HRESULT CTerrain_Manager::Initialize()
{
    return S_OK;
}

void CTerrain_Manager::Check_Landing()
{
    _float3 vDist, vRayDir = _float3{ 0.f, -1.f, 0.f };
    _float fMin = FLT_MAX;

    CGameObject* pNearestLand = {};

    for (auto& LandObj : m_LandObjects)
    {
        /*
        LandObject의 Transform 뽑아와서,
        아래 방향으로 레이 쏘게 한다음

        가장 가까운 거리를 가진 평면을 변수로 넘겨서,
        Change Terrain 해주면 됨
        */
        CTransform* pTransform = static_cast<CTransform*>(LandObj->Find_Component(TEXT("Com_Transform")));
        _float3 vPos = pTransform->Get_State(STATE::POSITION);

        /*Cube 체크*/
        for (auto& pCube : m_CubeObjects)
        {
            CCubeObject::CUBE_DESC CubeDesc = pCube->Get_CubeDesc();

            if (CubeDesc.pBuffer->Picking(CubeDesc.pTransform, &vDist, vPos, vRayDir))
            {
                _float3 vDiff = vDist - vPos;

                if (D3DXVec3Length(&vDiff) < fMin)
                {
                    fMin = D3DXVec3Length(&vDiff);
                    pNearestLand = pCube;
                }
            }
        }

        /* Terrain 체크 */
        for (auto& pTerrain : m_Terrains)
        {
            CTerrain::TERRAIN_DESC TerrainDesc = pTerrain->Get_TerrainDesc();

            if (TerrainDesc.pBuffer->Picking(TerrainDesc.pTransform, &vDist, vPos, vRayDir))
            {
                _float3 vDiff = vDist - vPos;

                if (D3DXVec3Length(&vDiff) < fMin)
                {
                    fMin = D3DXVec3Length(&vDiff);
                    pNearestLand = pTerrain;
                }
            }
        }

        if (fMin != FLT_MAX)
        {
            CLandObject::LANDOBJECT_DESC Desc;

            if (dynamic_cast<CTerrain*>(pNearestLand) != nullptr)
            {
                Desc = { dynamic_cast<CTerrain*>(pNearestLand)->Get_TerrainDesc().pBuffer, dynamic_cast<CTerrain*>(pNearestLand)->Get_TerrainDesc().pTransform };

                if (fMin > 0.6f && LandObj->Get_Jump() == false)
                {
                    LandObj->Set_Jump(true);
                    LandObj->Set_Time(0.133334f);
                }
            }
            else if (dynamic_cast<CCubeObject*>(pNearestLand) != nullptr)
            {
                Desc = { dynamic_cast<CCubeObject*>(pNearestLand)->Get_CubeDesc().pBuffer, dynamic_cast<CCubeObject*>(pNearestLand)->Get_CubeDesc().pTransform };

                if (fMin > 0.6f && LandObj->Get_Jump() == false)
                {
                    LandObj->Set_Jump(true);
                    LandObj->Set_Time(0.133334f);
                }
            }
            
            LandObj->Change_Land(&Desc);

            /*
            m_bJump가 false일때, ray의 거리가 멀다면 점프로 바꾸고 시간 세팅해주기
            예외사항?
            -> 없는거같은데?
            -> 게터 세터를 만들어주긴 해야함....
            */
            /* 0.133333이 돼야 떨어지기 시작. 0.134로 세팅하면 될듯 ? */
            /*
            이슈 ->
            1. 점프하거나 지형 바뀔때 약간씩 덜컹거리는 문제가 있음
            */
        }
    }
}

CTerrain_Manager* CTerrain_Manager::Create()
{
    CTerrain_Manager* pInstance = new CTerrain_Manager();

    if (FAILED(pInstance->Initialize()))
        Safe_Release(pInstance);

    return pInstance;
}

void CTerrain_Manager::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
}
