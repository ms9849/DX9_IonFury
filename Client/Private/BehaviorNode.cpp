#include "BehaviorNode.h"

CBehaviorNode::CBehaviorNode() 
{
}

CConditionNode::CConditionNode(function<bool()> condition) 
{
    this->m_bCondition = [condition](_float) { return condition(); };
}

CConditionNode::CConditionNode(function<bool(_float)> condition)
    : m_bCondition(condition) {
}


_bool CConditionNode::Run(_float fTimeDelta)
{
    return m_bCondition(fTimeDelta);
}

CActionNode::CActionNode(function<void()> act)
    : action(act) {
}

_bool CActionNode::Run(_float fTimeDelta)
{
    action();
    return true;
}

void CSequenceNode::AddChild(CBehaviorNode* child)
{
    m_pChildrens.push_back(child);
}

_bool CSequenceNode::Run(_float fTimeDelta)
{
    for (auto* child : m_pChildrens)
    {
        if (!child->Run(fTimeDelta))
            return false;
    }
    return true;
}

void CSelectorNode::AddChild(CBehaviorNode* child)
{
    m_pChildrens.push_back(child);
}

_bool CSelectorNode::Run(_float fTimeDelta)
{
    for (auto* child : m_pChildrens)
    {
        if (child->Run(fTimeDelta))
            return true;
    }
    return false;
}