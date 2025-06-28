#include "Picking.h"

CPicking::CPicking(LPDIRECT3DDEVICE9 pGraphicDevice)
    : m_pGraphic_Device { pGraphicDevice }
{
    Safe_AddRef(m_pGraphic_Device);
}

HRESULT CPicking::Initialize(HWND hWnd)
{
    m_hWnd = hWnd;

    D3DVIEWPORT9        Viewport{};
    m_pGraphic_Device->GetViewport(&Viewport);

    m_iWinSizeX = Viewport.Width;
    m_iWinSizeY = Viewport.Height;    

    return S_OK;
}

_bool CPicking::Picking_InWorldSpace(const _float3& vRayPos, const _float3& vRayDir, const _float3& vPointA, const _float3& vPointB, const _float3& vPointC, _float3* pOut)
{
    _float      fU{}, fV{}, fDist{};

    _bool       isColl = D3DXIntersectTri(&vPointA, &vPointB, &vPointC, &vRayPos, &vRayDir, &fU, &fV, &fDist);

    if (true == isColl)
    {
        *pOut = vRayPos + vRayDir * fDist;
    }

    return isColl;
}

_bool CPicking::Picking_InLocalSpace(const _float4x4* pWorldMatrixInverse, const _float3& vRayPos, const _float3& vRayDir, const _float3& vPointA, const _float3& vPointB, const _float3& vPointC, _float3* pOut)
{
    _float      fU{}, fV{}, fDist{};
    _float3     m_vRayPos, m_vRayDir = {};

    D3DXVec3Normalize(&m_vRayDir, &vRayDir);

    m_vRayPos = *D3DXVec3TransformCoord(&m_vRayPos, &vRayPos, pWorldMatrixInverse);
    m_vRayDir = *D3DXVec3TransformNormal(&m_vRayDir, &vRayDir, pWorldMatrixInverse);

    _bool       isColl = D3DXIntersectTri(&vPointA, &vPointB, &vPointC, &m_vRayPos, &m_vRayDir, &fU, &fV, &fDist);

    if (true == isColl)
    {
        *pOut = m_vRayPos + m_vRayDir * fDist;
    }

    return isColl;
}

CPicking* CPicking::Create(LPDIRECT3DDEVICE9 pGraphicDevice, HWND hWnd)
{
    CPicking* pInstance = new CPicking(pGraphicDevice);

    if (FAILED(pInstance->Initialize(hWnd)))
    {
        MSG_BOX("Failed to Created : CPicking");
        Safe_Release(pInstance);
    }

    return pInstance;
}


void CPicking::Free()
{
    __super::Free();

    Safe_Release(m_pGraphic_Device);
}
