#pragma once

#include "Client_Defines.h"
#include "LandObject.h"
#include "Animation.h"

NS_BEGIN(Engine)
class CTexture;
class CAnimation;
class CTransform;
class CVIBuffer_Rect;
class CSight;
NS_END

NS_BEGIN(Client)

enum class MonsterState {
	FRONT,
	BACK,
	ROTATE,
	MOVE,
};

class CMonster final : public CLandObject
{
private:
	CMonster(LPDIRECT3DDEVICE9 pGraphic_Device);
	CMonster(const CMonster& Prototype);
	virtual ~CMonster() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTexture*				m_pTextureCom = { nullptr };
	CAnimation* m_pAnimationCom = { nullptr };
	CTransform*				m_pTransformCom = { nullptr };	
	CVIBuffer_Rect*			m_pVIBufferCom = { nullptr };
	CSight* m_pSightCom = { nullptr };
	CTransform* m_pPlayerTransform = { nullptr };
	
	int						m_iNum = 0;

private:
	HRESULT Ready_Components();
	HRESULT Ready_Animations();
	HRESULT Begin_RenderState();
	HRESULT End_RenderState();

	map<const _wstring, CTexture*> m_pTextureComs;

	_wstring m_strFrameKey{TEXT("Soldier_Front")};
	map<const _wstring, CAnimation::FRAME_DESC> m_Frames;

	const _wstring m_strFrameKeys[3] = {
		TEXT("Soldier_Back"), TEXT("Soldier_Front"), TEXT("Soldier_Rotate")
	};

public:
	static CMonster* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END