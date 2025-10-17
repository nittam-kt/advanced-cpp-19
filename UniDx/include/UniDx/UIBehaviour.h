#pragma once

#include "Behaviour.h"

namespace UniDx {

class Canvas;

// --------------------
// UIBehaviour���N���X
// --------------------
class UIBehaviour : public Behaviour
{
public:
	virtual void OnEnable() override;
	virtual void OnDisable() override;
	virtual void Render(const Matrix& proj) const {}

protected:
	Canvas* owner;
};

}
