#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CTransform;
class CVIBuffer_Rect;
class CBoxCollider;
class CTexture;
NS_END

NS_BEGIN(Client)

class CTrashBox : public CGameObject
{
private:
	CTrashBox(LPDIRECT3DDEVICE9 pGraphicDev);
	CTrashBox(const CTrashBox& Prototype); 
	virtual ~CTrashBox() = default;

public:
	/* 충돌한 상대 객체, 충돌이 발생한 내 자신의 콜라이더, 충돌 타입을 리턴 */
	virtual void OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta);
	virtual void OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta, CComponent* pCollider);

	virtual const COLLISION_DESC& Get_CollisionDesc(COLLISION eColType);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

/* 구 콜라이더가 필요할까? 그건 아닌듯 ..*/
private:
	CTransform* m_pTransformCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CBoxCollider* m_pBoxColliderCom = { nullptr };
	/* 쓰레기 박스 이미지 3장 가져와서 세팅해줄 것 */
	CTexture* m_pTextureCom = { nullptr };
	// 2 -> 1, 1 -> 0 되고 0에서 한대 더맞으면 죽게 처리할 것.
	_int m_iHp = { 2 };

private:
	HRESULT Begin_RenderState();
	HRESULT End_RenderState();
	HRESULT Ready_Components();

public:
	static CTrashBox* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END
