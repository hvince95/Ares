#include "Timer.h"

#include <GLFW\glfw3.h>

namespace glh {
	namespace Util {

		double Timer::GetTime()
		{
			return glfwGetTime();
		}
	}
}
