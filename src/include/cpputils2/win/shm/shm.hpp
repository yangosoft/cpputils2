/**
 * @file shm.hpp
 * @brief Shared memory abstraction for POSIX systems.
 *
 *
 */

#pragma once

#include "cpputils2/common/types.hpp"

#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>

namespace CppUtils2
{

	
	class Shm
	{
	public:
		Shm()
			: hMapFile(nullptr), pBuf{nullptr}
		{
		}

		Shm(const std::string& file_mem_path)
			: mem_path(file_mem_path), hMapFile(nullptr), pBuf{ nullptr }
		{
		}

		/// @brief Open an existing shared memory
	/// @param file_mem_path Path to the shared memory
	/// @param mem_size Size of the shared memory
	/// @return 0 on success, -1 on error
		Result open_existing(const std::string& file_mem_path, std::size_t mem_size)
		{
			mem_path = file_mem_path;
			return open_existing(mem_size);
		}

		/// @brief Open an existing shared memory
		/// @param mem_size Size of the shared memory
		/// @return 0 on success, -1 on error
		Result open_existing(std::size_t mem_size)
		{
			if (hMapFile != nullptr)
			{
				return Result::RET_ERROR;
			}
			int flags = 0;
			return shared_open(flags, mem_size);
		}

		/// @brief Allocate a new shared memory
		/// @param mem_size Size of the shared memory
		/// @return 0 on success, -1 on error
		Result allocate(std::size_t mem_size)
		{
			if (hMapFile != nullptr)
			{
				return Result::RET_ERROR;
			}

			int flags = 1;

			return shared_open(flags, mem_size);
		}

		/// @brief Removes the shared memory file
		/// @return 0 on success, -1 on error
		Result unlink()
		{
			if (hMapFile != nullptr)
			{
				UnmapViewOfFile(pBuf);
				CloseHandle(hMapFile);
			}
			return Result::RET_ERROR;
		}

		/// @brief Closes the shared memory
		void close()
		{
			if (hMapFile != nullptr)
			{
				UnmapViewOfFile(pBuf);
				CloseHandle(hMapFile);
			}
			hMapFile = nullptr;
		}

		/// @brief Get the pointer to the shared memory
		/// @return Pointer to the shared memory
		void* get_raw_ptr()
		{
			return pBuf;
		}

		virtual ~Shm()
		{
			close();
		}

	private:
		std::string mem_path;
		HANDLE hMapFile;
		LPVOID pBuf;

		Result shared_open(int flags, std::size_t mem_size)
		{
			if (flags == 1) {

				//TCHAR szName[] = TEXT(mem_path.c_str());

				hMapFile = CreateFileMappingA(
					INVALID_HANDLE_VALUE,    // use paging file
					nullptr,                    // default security
					PAGE_READWRITE,          // read/write access
					0,                       // maximum object size (high-order DWORD)
					mem_size,                // maximum object size (low-order DWORD)
					mem_path.c_str());                 // name of mapping object
			
				if (hMapFile == NULL)
				{
					
					return Result::RET_ERROR;
				}

				pBuf = MapViewOfFile(hMapFile,   // handle to map object
					FILE_MAP_ALL_ACCESS, // read/write permission
					0,
					0,
					mem_size);

				if (pBuf == nullptr)
				{
					//_tprintf(TEXT("Could not map view of file (%d).\n"),		GetLastError());

					CloseHandle(hMapFile);

					return Result::RET_ERROR;
				}
			
			}
			else {

				hMapFile = OpenFileMappingA(
					FILE_MAP_ALL_ACCESS,   // read/write access
					FALSE,                 // do not inherit the name
					mem_path.c_str());               // name of mapping object

				if (hMapFile == NULL)
				{
					//_tprintf(TEXT("Could not open file mapping object (%d).\n"),
					//	GetLastError());
					return Result::RET_ERROR;
				}

				pBuf = MapViewOfFile(hMapFile, // handle to map object
					FILE_MAP_ALL_ACCESS,  // read/write permission
					0,
					0,
					mem_size);

				if (pBuf == nullptr)
				{
					//_tprintf(TEXT("Could not map view of file (%d).\n"),
					//	GetLastError());

					CloseHandle(hMapFile);

					return Result::RET_ERROR;
				}
			
			}
			return Result::RET_OK;
		}
	};
}