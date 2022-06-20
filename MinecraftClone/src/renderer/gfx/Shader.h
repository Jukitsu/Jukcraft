#pragma once

class Shader {
public:
	Shader(const std::string_view& vert, const std::string_view& frag) {
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
private:
	static GLuint compileShaderStage(GLenum stage, const char* src) {
		GLuint sh = glCreateShader(stage);
		glShaderSource(sh, 1, &src, nullptr);
		glCompileShader(sh);

		int result;
		glGetShaderiv(sh, GL_COMPILE_STATUS, &result);
		if (!result) {
			int logLength;
			glGetShaderiv(sh, GL_INFO_LOG_LENGTH, &logLength);
			char* log = new char[logLength];
			glGetShaderInfoLog(sh, logLength, &logLength, log);
			THROW_ERROR("Error in {} stage: {}", (stage == GL_VERTEX_SHADER ? "vertex" : "fragment"), log);
			delete[] log;
			return 0;
		}
		return sh;
	}
	GLuint handle;
	FORBID_COPY(Shader);
	FORBID_MOVE(Shader);
};