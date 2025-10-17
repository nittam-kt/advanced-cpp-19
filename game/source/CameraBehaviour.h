#pragma once

#include <UniDx.h>
#include <UniDx/Behaviour.h>


class CameraBehaviour : public UniDx::Behaviour
{
public:
    virtual void OnEnable() override;
    virtual void Update() override;
};