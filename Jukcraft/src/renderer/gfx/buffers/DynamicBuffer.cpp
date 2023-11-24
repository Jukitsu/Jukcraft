#include "pch.h"
#include "renderer/gfx/buffers/DynamicBuffer.h"
#include "core/App.h"
#include "core/util.h"

namespace Jukcraft {
	namespace gfx {
		Window& GetMainWindow() noexcept {
			return App::GetWindow();
		}
	}
}