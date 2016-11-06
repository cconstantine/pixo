#pragma once

#include <shader.hpp>

class Drawable {
public:
	virtual void Draw(Shader shader) = 0;
};