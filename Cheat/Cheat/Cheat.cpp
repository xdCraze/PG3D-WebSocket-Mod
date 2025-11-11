#include "Cheat.hpp"

namespace Cheat
{
	void Initialize()
	{
		Pointers::Load();
		Hooks::Load();
	}
}