#include "BehaviorNode.h"
#include <functional>
#include <vector>

using std::function;
using std::vector;

BehaviorNode::BehaviorNode() {}
BehaviorNode::~BehaviorNode() {}

ConditionNode::ConditionNode(std::function<bool()> condition) 
{
    this->condition = [condition](float) { return condition(); };
}

ConditionNode::ConditionNode(std::function<bool(float)> condition)
    : condition(condition) {
}

bool ConditionNode::Run(float fTimeDelta)
{
    return condition(fTimeDelta);
}

ActionNode::ActionNode(std::function<void()> act)
    : action(act) {
}

bool ActionNode::Run(float fTimeDelta)
{
    action();
    return true;
}

void SequenceNode::AddChild(BehaviorNode* child)
{
    children.push_back(child);
}

bool SequenceNode::Run(float fTimeDelta)
{
    for (auto* child : children)
    {
        if (!child->Run(fTimeDelta))
            return false;
    }
    return true;
}

void SelectorNode::AddChild(BehaviorNode* child)
{
    children.push_back(child);
}

bool SelectorNode::Run(float fTimeDelta)
{
    for (auto* child : children)
    {
        if (child->Run(fTimeDelta))
            return true;
    }
    return false;
}