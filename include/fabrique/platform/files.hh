//! @file platform/files.h      Declarations of OS-specific file/path functions
/*
 * Copyright (c) 2014, 2018 Jonathan Anderson
 * All rights reserved.
 *
 * This software was developed by SRI International and the University of
 * Cambridge Computer Laboratory under DARPA/AFRL contract (FA8750-10-C-0237)
 * ("CTSRD"), as part of the DARPA CRASH research programme and at Memorial University
 * of Newfoundland under the NSERC Discovery program (RGPIN-2015-06048).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef FAB_PLATFORM_FILES_H_
#define FAB_PLATFORM_FILES_H_

#include <functional>
#include <string>
#include <vector>

namespace fabrique {
namespace platform {

//
// File- and path-related predicates:
//

//! Check whether a file is executable on this platform.
bool FileIsExecutable(std::string path);

//! Test whether this platform can load a file as a shared library.
bool FileIsSharedLibrary(std::string path);

//! The named path is absolute, whether or not the file actually exists.
bool PathIsAbsolute(const std::string&);

//! Does the named path exist, and is it a directory?
bool PathIsDirectory(std::string);

//! Does the named path exist, and is it a regular file?
bool PathIsFile(std::string);


//
// Other file- and path-related functions that might be used as arguments:
//

typedef std::function<std::string (std::string, const std::vector<std::string>&)>
	MissingFileReporter;

//! Create a @ref std::function that returns a default filename.
MissingFileReporter DefaultFilename(std::string name = "");

//! Throw a @ref UserError to report a missing file.
std::string FileNotFound(std::string name, const std::vector<std::string>& searchPaths);


//
// Filename and path manipulation:
//

//! Find the absolute version of a directory, optionally creating it.
std::string AbsoluteDirectory(std::string name, bool createIfMissing = false);

//! Find the absolute version of a path (file or directory).
std::string AbsolutePath(std::string path);

//! Get the basename of a path: 'foo/bar.c' -> 'bar'.
std::string BaseName(std::string path);

//! The command required to create a directory (if it doesn't already exist).
std::string CreateDirCommand(std::string directory);

//! Find the directory containing a file, optionally returning an absolute path.
std::string DirectoryOf(std::string filename, bool absolute = false);

//! Get the extension of a path: 'foo/bar.c' -> 'c'.
std::string FileExtension(std::string path);

//! Find the non-directory component of a path.
std::string FilenameComponent(std::string pathIncludingDirectory);

//! Search the current PATH (as well as @b extraPaths) for an executable file.
std::string FindExecutable(std::string name,
	std::vector<std::string> extraPaths = std::vector<std::string>(),
	MissingFileReporter report = FileNotFound);

/**
 * Find a file named @a filename within a set of @a directories.
 *
 * @param   test      A test to invoke on each file (e.g., PathIsFile, FileIsExecutable)
 *                    in order to confirm the applicability of a file.
 * @param   report    A function to call if the file is not found.
 */
std::string FindFile(std::string filename, const std::vector<std::string>& directories,
                     std::function<bool (const std::string&)> test = PathIsFile,
                     MissingFileReporter report = FileNotFound);

//! Join two path components (a directory and a filename).
std::string JoinPath(const std::string&, const std::string&);

//! Join an arbitrary number of path components (directories and maybe a filename).
std::string JoinPath(const std::vector<std::string>&);

//! Convert a library name (e.g., "foo") into a filename (e.g., "libfoo.so").
std::string LibraryFilename(std::string name);

/**
 * Where are plugins kept on this platform?
 *
 * @param   executablePath     absolute path to Fabrique executable file
 */
std::vector<std::string> PluginSearchPaths(std::string executablePath);

} // namespace platform
} // namespace fabrique

#endif  // FAB_PLATFORM_FILES_H_
