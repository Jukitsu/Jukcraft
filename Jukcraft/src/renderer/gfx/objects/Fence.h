#pragma once


namespace Jukcraft {
	struct Fence {
		Fence() {
			fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
		}
		
		~Fence() {
			glClientWaitSync(fence, GL_SYNC_FLUSH_COMMANDS_BIT, 2147483647);
			glDeleteSync(fence);
		}

		GLsync fence;
	};
}