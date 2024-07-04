#include "pch.h"
#include "renderer/gfx/buffers/DynamicBuffer.h"
#include "core/App.h"
#include "core/util.h"

namespace Jukcraft {
	namespace gfx {
		SharedContext& GetSharedContext() {
			return App::GetWindow().getSharedContext();
		}
	}
}