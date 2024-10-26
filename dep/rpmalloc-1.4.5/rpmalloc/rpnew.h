
#ifdef __cplusplus

#include <new>
#include "rpmalloc.h"

#ifndef __CRTDECL
#define __CRTDECL
#endif

inline extern void __CRTDECL
operator delete(void* p) noexcept {
	rpfree(p);
}

inline extern void __CRTDECL
operator delete[](void* p) noexcept {
	rpfree(p);
}

inline extern void* __CRTDECL
operator new(std::size_t size) noexcept(false) {
	return rpmalloc(size);
}

inline extern void* __CRTDECL
operator new[](std::size_t size) noexcept(false) {
	return rpmalloc(size);
}

inline extern void* __CRTDECL
operator new(std::size_t size, const std::nothrow_t& tag) noexcept {
	(void)sizeof(tag);
	return rpmalloc(size);
}

inline extern void* __CRTDECL
operator new[](std::size_t size, const std::nothrow_t& tag) noexcept {
	(void)sizeof(tag);
	return rpmalloc(size);
}

#if (__cplusplus >= 201402L || _MSC_VER >= 1916)

inline extern void __CRTDECL
operator delete(void* p, std::size_t size) noexcept {
	(void)sizeof(size);
	rpfree(p);
}

inline extern void __CRTDECL
operator delete[](void* p, std::size_t size) noexcept {
	(void)sizeof(size);
	rpfree(p);
}

#endif

#if (__cplusplus > 201402L || defined(__cpp_aligned_new))

inline extern void __CRTDECL
operator delete(void* p, std::align_val_t align) noexcept {
	(void)sizeof(align);
	rpfree(p);
}

inline extern void __CRTDECL
operator delete[](void* p, std::align_val_t align) noexcept {
	(void)sizeof(align);
	rpfree(p);
}

inline extern void __CRTDECL
operator delete(void* p, std::size_t size, std::align_val_t align) noexcept {
	(void)sizeof(size);
	(void)sizeof(align);
	rpfree(p);
}

inline extern void __CRTDECL
operator delete[](void* p, std::size_t size, std::align_val_t align) noexcept {
	(void)sizeof(size);
	(void)sizeof(align);
	rpfree(p);
}

inline extern void* __CRTDECL
operator new(std::size_t size, std::align_val_t align) noexcept(false) {
	return rpaligned_alloc(static_cast<size_t>(align), size);
}

inline extern void* __CRTDECL
operator new[](std::size_t size, std::align_val_t align) noexcept(false) {
	return rpaligned_alloc(static_cast<size_t>(align), size);
}

inline extern void* __CRTDECL
operator new(std::size_t size, std::align_val_t align, const std::nothrow_t& tag) noexcept {
	(void)sizeof(tag);
	return rpaligned_alloc(static_cast<size_t>(align), size);
}

inline extern void* __CRTDECL
operator new[](std::size_t size, std::align_val_t align, const std::nothrow_t& tag) noexcept {
	(void)sizeof(tag);
	return rpaligned_alloc(static_cast<size_t>(align), size);
}

#endif

#endif
