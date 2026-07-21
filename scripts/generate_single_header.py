#!/usr/bin/env python3
"""
Single Header Generator for JsonReflect

This script generates a single header file that includes all JsonReflect headers
and extern dependencies (nlohmann/json, magic_enum, visit_struct, svh/tag_invoke).

Usage:
    python generate_single_header.py [output_path]
    
    Default output: single_header/JsonReflect.hpp
"""

import os
import re
import sys
from pathlib import Path
from datetime import datetime

# Get the project root directory (parent of scripts/)
SCRIPT_DIR = Path(__file__).parent.resolve()
PROJECT_ROOT = SCRIPT_DIR.parent

# Files to include in order (dependencies first)
# Order matters: each file should only depend on files listed before it
EXTERN_FILES = [
    "extern/nlohmann/json.hpp",
    "extern/svh/tag_invoke.hpp",
    "extern/visit_struct/visit_struct.hpp",
    "extern/magic_enum/magic_enum.hpp",
]

PROJECT_FILES = [
    "JsonReflect_defines.hpp",
    "JsonReflect_macro.hpp",
    "JsonReflect_helpers.hpp",
    "JsonReflect_entry.hpp",
    "JsonReflect_primitives.hpp",
    "JsonReflect_stl.hpp",
]

# Include patterns to remove (these will be stripped from the merged file)
INTERNAL_INCLUDE_PATTERNS = [
    # Macro-form dependency includes (from JsonReflect_defines.hpp)
    r'#include\s+JSON_REFLECT_NLOHMANN_JSON_HPP',
    r'#include\s+JSON_REFLECT_SVH_TAG_INVOKE_HPP',
    r'#include\s+JSON_REFLECT_VISIT_STRUCT_HPP',
    r'#include\s+JSON_REFLECT_MAGIC_ENUM_HPP',
    # Literal-form dependency includes
    r'#include\s*[<"]nlohmann/json\.hpp[>"]',
    r'#include\s*[<"]svh/tag_invoke\.hpp[>"]',
    r'#include\s*[<"]visit_struct/visit_struct\.hpp[>"]',
    r'#include\s*[<"]magic_enum/magic_enum\.hpp[>"]',
    r'#include\s*"JsonReflect_defines\.hpp"',
    r'#include\s*"JsonReflect_macro\.hpp"',
    r'#include\s*"JsonReflect_helpers\.hpp"',
    r'#include\s*"JsonReflect_entry\.hpp"',
    r'#include\s*"JsonReflect_primitives\.hpp"',
    r'#include\s*"JsonReflect_stl\.hpp"',
    r'#include\s*"JsonReflect\.hpp"',
]

# Pattern to match the header comment block at the top of JsonReflect files
JSONREFLECT_HEADER_PATTERN = re.compile(
    r'^// =+\n'
    r'// JsonRefelect.*?\n'
    r'// By Sven van Huessen.*?\n'
    r'// Made as self-study project.*?\n'
    r'// Licensed under the MIT License\n'
    r'// https://github\.com/Sven-vh/JsonReflect\n'
    r'// =+\n',
    re.MULTILINE
)

def read_license() -> str:
    """Read the LICENSE file content."""
    license_path = PROJECT_ROOT / "LICENSE"
    if license_path.exists():
        return license_path.read_text(encoding='utf-8')
    return ""

def get_file_list_comment() -> str:
    """Generate a comment listing all included files."""
    lines = ["/*", " * This single header file contains the following files:"]
    lines.append(" *")
    lines.append(" * External Dependencies:")
    for f in EXTERN_FILES:
        lines.append(f" *   - {f}")
    lines.append(" *")
    lines.append(" * JsonReflect Library:")
    for f in PROJECT_FILES:
        lines.append(f" *   - {f}")
    lines.append(" */")
    return "\n".join(lines)

def generate_header_comment() -> str:
    """Generate the top comment block for the single header."""
    license_text = read_license()
    
    header = f"""// ============================================================================
// JsonReflect - Single Header Version
// Reflection-based JSON Serialization Library
// By Sven van Huessen (https://www.svenvh.nl/)
// Made as self-study project at Breda University of Applied Sciences
// https://github.com/Sven-vh/JsonReflect
//
// Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}
// ============================================================================
//
// MIT License
//
// Copyright (c) 2026 Sven van Huessen
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// ============================================================================

"""
    return header

def remove_internal_includes(content: str) -> str:
    """Remove includes that reference internal files."""
    for pattern in INTERNAL_INCLUDE_PATTERNS:
        content = re.sub(pattern + r'\s*\n?', '', content)
    return content

def remove_pragma_once(content: str) -> str:
    """Remove #pragma once directives."""
    return re.sub(r'#pragma\s+once\s*\n?', '', content)

def remove_jsonreflect_header_comment(content: str) -> str:
    """Remove the JsonReflect header comment block."""
    return JSONREFLECT_HEADER_PATTERN.sub('', content)

def process_file(filepath: Path, is_extern: bool = False) -> str:
    """Process a single file and return its cleaned content."""
    content = filepath.read_text(encoding='utf-8')
    
    # Remove pragma once
    content = remove_pragma_once(content)
    
    # Remove internal includes
    content = remove_internal_includes(content)
    
    # Remove JsonReflect header comments from project files
    if not is_extern:
        content = remove_jsonreflect_header_comment(content)
    
    # Clean up excessive blank lines
    content = re.sub(r'\n{3,}', '\n\n', content)
    
    return content.strip()

def generate_section_comment(title: str) -> str:
    """Generate a section separator comment."""
    return f"""
// ============================================================================
// {title}
// ============================================================================

"""

def generate_file_comment(filepath: str) -> str:
    """Generate a comment indicating the start of a file's content."""
    return f"""
// ----------------------------------------------------------------------------
// Begin: {filepath}
// ----------------------------------------------------------------------------

"""

def generate_single_header(output_path: Path) -> None:
    """Generate the single header file."""
    parts = []
    
    # Add the main header comment and license
    parts.append(generate_header_comment())
    
    # Add the file list comment
    parts.append(get_file_list_comment())
    parts.append("\n\n")
    
    # Add include guard
    parts.append("#ifndef JSONREFLECT_SINGLE_HEADER_HPP\n")
    parts.append("#define JSONREFLECT_SINGLE_HEADER_HPP\n\n")
    
    # Add the JSON_USE_IMPLICIT_CONVERSIONS define before nlohmann/json
    parts.append("/* Disable implicit nlohmann conversion */\n")
    parts.append("/* This is to avoid confusion with templates that are accidentally converted to json objects */\n")
    parts.append("#ifndef JSON_USE_IMPLICIT_CONVERSIONS\n")
    parts.append("#define JSON_USE_IMPLICIT_CONVERSIONS 0\n")
    parts.append("#endif\n\n")
    
    # Process extern files
    parts.append(generate_section_comment("External Dependencies"))
    
    for filepath in EXTERN_FILES:
        full_path = PROJECT_ROOT / filepath
        if not full_path.exists():
            print(f"Warning: File not found: {full_path}", file=sys.stderr)
            continue
        
        parts.append(generate_file_comment(filepath))
        content = process_file(full_path, is_extern=True)
        parts.append(content)
        parts.append("\n\n")
    
    # Process project files
    parts.append(generate_section_comment("JsonReflect Library"))
    
    for filepath in PROJECT_FILES:
        full_path = PROJECT_ROOT / filepath
        if not full_path.exists():
            print(f"Warning: File not found: {full_path}", file=sys.stderr)
            continue
        
        parts.append(generate_file_comment(filepath))
        content = process_file(full_path, is_extern=False)
        parts.append(content)
        parts.append("\n\n")
    
    # Close include guard
    parts.append("#endif // JSONREFLECT_SINGLE_HEADER_HPP\n")
    
    # Write the output
    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_text("".join(parts), encoding='utf-8')
    
    print(f"Generated single header: {output_path}")
    print(f"Total size: {output_path.stat().st_size:,} bytes")

def main():
    # Default output path
    output_path = PROJECT_ROOT / "single_header" / "JsonReflect.hpp"
    
    # Allow custom output path via command line
    if len(sys.argv) > 1:
        output_path = Path(sys.argv[1])
        if not output_path.is_absolute():
            output_path = PROJECT_ROOT / output_path
    
    generate_single_header(output_path)
    return 0

if __name__ == "__main__":
    sys.exit(main())
