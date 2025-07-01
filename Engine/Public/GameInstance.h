#pragma once

#include "Prototype_Manager.h"

NS_BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance);

private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
	HRESULT Initialize_Engine(const ENGINE_DESC& EngineDesc, LPDIRECT3DDEVICE9* ppOut);
	void Update_Engine(_float fTimeDelta);
	HRESULT Draw();
	void Clear_Resources(_uint iLevelIndex);

	_float Random_Normal();
	_float Random(_float fMin, _float fMax);
	_bool CCW(_float3 vSrc, _float3 vDst);

#pragma region GRAPHIC_DEVICE
public:
	void Render_Begin(D3DXCOLOR Color);
	void Render_End(HWND hWnd = nullptr);
#pragma endregion

#pragma region TIMER_MANAGER
	_float	Get_TimeDelta(const _wstring& strTimerTag);
	HRESULT	Add_Timer(const _wstring& strTimerTag);
	void	Compute_TimeDelta(const _wstring& strTimerTag);
#pragma endregion

#pragma region LEVEL_MANAGER
public:
	HRESULT Change_Level(class CLevel* pNewLevel);
#pragma endregion

#pragma region PROTOTYPE_MANAGER
	HRESULT Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, class CBase* pPrototype);
	class CBase* Clone_Prototype(PROTOTYPE ePrototype, _uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg = nullptr);
#pragma endregion

#pragma region OBJECT_MANAGER
	CComponent* Get_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex = 0);
	HRESULT Add_GameObject_ToLayer(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iLayerLevelIndex, const _wstring& strLayerTag, void* pArg = nullptr);
	class CGameObject* Find_GameObject_ToLayer(_uint iLayerLevelIndex, const _wstring& strLayerTag, void* pArg = nullptr);
	class CLayer* Find_Layer(_uint iLayerLevelIndex, const _wstring& strLayerTag);
	map<const _wstring, class CLayer*> Get_Layers_InLevel(_uint iLayerLevelIndex);
#pragma endregion

#pragma region RENDERER
	HRESULT Add_RenderGroup(RENDER eRenderGroup, class CGameObject* pRenderObject);
#pragma endregion

#pragma region KEY_MANAGER
	bool		Key_Pressing(_uint _iKey);
	// 눌렀다가 뗐을 때
	bool		Key_Up(_uint _iKey);
	// 누른 타이밍 딱 한 번 만
	bool		Key_Down(_uint _iKey);
#pragma endregion

#pragma region SOUND_MANAGER
	void PlaySoundOnce(const TCHAR* pSoundKey, CHANNELID eID, float fVolume);
	void PlayBGM(const TCHAR* pSoundKey, float fVolume);
	void StopSound(CHANNELID eID);
	void StopAll();
	void SetChannelVolume(CHANNELID eID, float fVolume);
#pragma endregion

#pragma region COLLISION_MANAGER
	void	Check_OBBCollision(const _wstring& strLayerTagSrc, const _wstring& strLayerTagDst, _uint iLayerLevel, _float fTimeDelta);
	void	Check_RayCollision(_float3 vPos, _float3 vLook, const _wstring& strLayerTagDst, _uint iLayerLevel, _float3* vColisionPos);
	void	Check_SphereCollision(const _wstring& strLayerTagSrc, const _wstring& strLayerTagDst, _uint iLayerLevel, _float fTimeDelta);
#pragma endregion

#pragma region PICKING 
	_bool Picking_InWorldSpace(const _float3& vRayPos, const _float3& vRayDir, const _float3& vPointA, const _float3& vPointB, const _float3& vPointC, _float3* pOut);
	_bool Picking_InLocalSpace(const _float4x4* pWorldMatrixInverse, const _float3& vRayPos, const _float3& vRayDir, const _float3& vPointA, const _float3& vPointB, const _float3& vPointC, _float3* pOut);

#pragma endregion
private:
	class CGraphic_Device*			m_pGraphic_Device = { nullptr };
	class CTimer_Manager*			m_pTimer_Manager = { nullptr };
	class CLevel_Manager*			m_pLevel_Manager = { nullptr };
	class CPrototype_Manager*		m_pPrototype_Manager = { nullptr };
	class CObject_Manager*			m_pObject_Manager = { nullptr };
	class CRenderer*				m_pRenderer = { nullptr };
	class CKey_Manager* m_pKey_Manager = { nullptr };
	class CSound_Manager* m_pSound_Manager = { nullptr };
	class CCollision_Manager* m_pCollision_Manager = { nullptr };
	class CPicking* m_pPicking = { nullptr };

public:
	void Release_Engine();
	virtual void Free() override;
};

NS_END