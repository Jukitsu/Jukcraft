#pragma once


#define FORBID_COPY(CLS) CLS(const CLS&) = delete; CLS& operator=(const CLS&) = delete
#define FORBID_MOVE(CLS) CLS(CLS&&) noexcept = delete; CLS& operator=(CLS&&) noexcept = delete

#ifdef _DEBUG
#define DEBUGBREAK() __debugbreak()
#else
#define DEBUGBREAK()
#endif

#define ERROR(x) { std::cerr << x << std::endl; DEBUGBREAK(); }