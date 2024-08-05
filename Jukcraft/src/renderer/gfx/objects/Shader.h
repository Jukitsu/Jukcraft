#pragma once
#include <glad/glad.h>

namespace Jukcraft {
	namespace gfx {
		class Shader {
		public:
			Shader(const std::string_view& vertFilepath, const std::string_view& fragFilepath) {
				std::string vert = parseFile(vertFilepath);
				std::string frag = parseFile(fragFilepath);
				handle = glCreateProgram();

				GLuint vsh = compileShaderStage(GL_VERTEX_SHADER, vert.data());
				GLuint fsh = compileShaderStage(GL_FRAGMENT_SHADER, frag.data());

				glAttachShader(handle, vsh);
				glAttachShader(handle, fsh);
				glLinkProgram(handle);

				int result;
				glGetProgramiv(handle, GL_LINK_STATUS, &result);
				if (!result) {
					int logLength;
					glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &logLength);
					char* log = new char[logLength];
					glGetProgramInfoLog(handle, logLength, &logLength, log);
					THROW_ERROR("Error in linking stage: {}", log);
					delete[] log;
				}

				glDetachShader(handle, vsh);
				glDetachShader(handle, fsh);

				glDeleteShader(vsh);
				glDeleteShader(fsh);
			}
			~Shader() {
				glDeleteProgram(handle);
			}
			constexpr GLuint getHandle() const { return handle; }

			void bind() {
				glUseProgram(handle);
			}

			void setUniform3f(uint8_t location, const glm::vec3& value) {
				glProgramUniform3f(handle, location, value.x, value.y, value.z);
			}

			void setUniform4f(uint8_t location, const glm::vec4& value) {
				glProgramUniform4f(handle, location, value.x, value.y, value.z, value.w);
			}

			void setUniform1f(uint8_t location, float value) {
				glProgramUniform1f(handle, location, value);
			}

			void setUniformMat4f(uint8_t location, const float* value) {
				glProgramUniformMatrix4fv(handle, location, 1, GL_FALSE, value);
			}
		private:
			static GLuint compileShaderStage(GLenum stage, const char* src) {
				GLuint sh = glCreateShader(stage);
				glShaderSource(sh, 1, &src, nullptr);
				glCompileShader(sh);

				GLint result;
				glGetShaderiv(sh, GL_COMPILE_STATUS, &result);

				if (!result) {
					GLint logLength;
					glGetShaderiv(sh, GL_INFO_LOG_LENGTH, &logLength);
					GLchar* log = new GLchar[logLength];
					glGetShaderInfoLog(sh, logLength, &logLength, log);
					THROW_ERROR("Error in {} stage: {}", (stage == GL_VERTEX_SHADER ? "vertex" : "fragment"), log);
					delete[] log;
					return 0;
				}

				return sh;
			}
		private:
			GLuint handle;
			FORBID_COPY(Shader);
			FORBID_MOVE(Shader);
		};
	}
}