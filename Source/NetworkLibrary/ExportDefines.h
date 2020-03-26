#pragma once

#ifdef NETWORKLIBRARY_EXPORT
	#if !defined(__cplusplus)
		#define EXPORT extern "C" __declspec(dllexport)
	#else
		#define EXPORT __declspec(dllexport)
	#endif
#else
	#define EXPORT __declspec(dllimport)
#endif