#include "Client_Defines.h"
#include "UIObject.h"
#include "UIFont.h"
#include "UIText.h"


NS_BEGIN(Engine)
class CTransform;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CUIHp final : public CUIObject
{
private:
	CUIHp(LPDIRECT3DDEVICE9 pGraphic_Device);
	CUIHp(const CUIHp& Prototype);
	virtual ~CUIHp() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_Hp(_uint iNumber);

private:
	CTexture* m_pTextureCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	CUIText* m_pText{ nullptr };
	_uint m_iHp{};
	UIOBJECT_DESC	m_tagDesc{};

private:
	HRESULT Ready_Components();

public:
	static CUIHp* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END