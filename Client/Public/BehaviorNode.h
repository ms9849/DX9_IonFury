#pragma once

#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Client)

class CBehaviorNode abstract : public CBase
{
public:
    CBehaviorNode();
    virtual ~CBehaviorNode() = default;

public:
    virtual _bool Run(_float) = 0;
};

class CConditionNode : public CBehaviorNode
{
public:
    CConditionNode(function<bool()> condition);
    CConditionNode(function<bool(_float)> condition);
    _bool Run(_float) override;

private:
    function<bool(_float)> m_bCondition;
};

class CActionNode : public CBehaviorNode
{
public:
    CActionNode(function<void()> action);
    _bool Run(_float) override;

private:
    function<void()> action;
};

class CSequenceNode : public CBehaviorNode
{
public:
    void AddChild(CBehaviorNode* child);
    _bool Run(_float) override;

private:
    vector<CBehaviorNode*> m_pChildrens;
};

class CSelectorNode : public CBehaviorNode
{
public:
    void AddChild(CBehaviorNode* child);
    _bool Run(_float) override;

private:
    vector<CBehaviorNode*> m_pChildrens;
};

NS_END