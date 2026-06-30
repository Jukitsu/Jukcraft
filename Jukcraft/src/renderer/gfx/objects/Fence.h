#pragma once

namespace Jukcraft {

    struct Fence {
        Fence() {
            fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
        }

        ~Fence() {
            glClientWaitSync(fence, GL_SYNC_FLUSH_COMMANDS_BIT, GL_TIMEOUT_IGNORED);
            glDeleteSync(fence);
        }

        // Non-blocking check: returns true if the GPU has signaled this fence.
        [[nodiscard]] bool isSignaled() const noexcept {
            GLenum result = glClientWaitSync(fence, 0, 0);
            return (result == GL_ALREADY_SIGNALED || result == GL_CONDITION_SATISFIED);
        }

        GLsync fence;
    };

}