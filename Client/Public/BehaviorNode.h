#pragma once

#include <functional>
#include <vector>

class BehaviorNode
{
public:
    BehaviorNode();
    virtual ~BehaviorNode();

public:
    virtual bool Run(float) = 0;
};

class ConditionNode : public BehaviorNode
{
public:
    ConditionNode(std::function<bool()> condition);
    ConditionNode(std::function<bool(float)> condition);
    bool Run(float) override;

private:
    std::function<bool(float)> condition;
};

class ActionNode : public BehaviorNode
{
public:
    ActionNode(std::function<void()> action);
    bool Run(float) override;

private:
    std::function<void()> action;
};

class SequenceNode : public BehaviorNode
{
public:
    void AddChild(BehaviorNode* child);
    bool Run(float) override;

private:
    std::vector<BehaviorNode*> children;
};

class SelectorNode : public BehaviorNode
{
public:
    void AddChild(BehaviorNode* child);
    bool Run(float) override;

private:
    std::vector<BehaviorNode*> children;
};