#include "Client_Defines.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class CTransform;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CUIBossHpFill final : public CUIObject
{
private:
	CUIBossHpFill(LPDIRECT3DDEVICE9 pGraphic_Device);
	CUIBossHpFill(const CUIBossHpFill& Prototype);
	virtual ~CUIBossHpFill() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_float m_fCurHp = 0.f;
	_float m_fMaxHp = 0.f;

private:
	class CBoss* m_pBoss = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	UIOBJECT_DESC	m_tagDesc{};
	UIOBJECT_DESC*	m_pTemp;

private:
	HRESULT Ready_Components();
	void Get_Hp();
	void Set_HpRatio(float fRatio);

public:
	static CUIBossHpFill* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END