#include "Hinge.h"

#include <UniDx/Input.h>
#include <UniDx/Time.h>

using namespace DirectX;
using namespace UniDx;


void Hinge::Update()
{
    const float rotateSpeed = XM_PI * 1.0f;

    Quaternion localRotation = transform->localRotation;
    if (Input::GetKey(Keyboard::A))
    {
        localRotation *= Quaternion::CreateFromYawPitchRoll(0, 0, rotateSpeed * -Time::deltaTime);
    }
    else if (Input::GetKey(Keyboard::D))
    {
        localRotation *= Quaternion::CreateFromYawPitchRoll(0, 0, rotateSpeed * Time::deltaTime);
    }

    transform->localRotation = localRotation;
}
