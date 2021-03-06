#!/bin/bash

glslang=$(PATH=./glslang/bin:$PATH which glslangValidator 2> /dev/null)
result=0

shaderdir=$1

if [ -z $glslang ]; then
    curl -L -o output.zip https://github.com/KhronosGroup/glslang/releases/download/master-tot/glslang-master-linux-Release.zip 2> /dev/null
    python -m zipfile -e output.zip glslang
    chmod +x glslang/bin/glslangValidator
    glslang="./glslang/bin/glslangValidator"
fi

function print_error
{
    echo "While validating $1:"
    echo "$2"
    result=1
}

function validate_vert
{
    output=$(cat "$shaderdir/desktop_header.glsl" "$shaderdir/$1" | $glslang --stdin -S vert -l -DVALIDATING)
    if [ $? -ne 0 ]; then print_error "$1" "$output"; fi
    output=$(cat "$shaderdir/es_header.glsl" "$shaderdir/$1" | $glslang --stdin -S vert -l -DVALIDATING)
    if [ $? -ne 0 ]; then print_error "$1" "$output"; fi
    output=$(cat "$shaderdir/desktop_header_core.glsl" "$shaderdir/${1/.vert/_core.vert}" | $glslang --stdin -S vert -l -DVALIDATING)
    if [ $? -ne 0 ]; then print_error "$1" "$output"; fi
}

function validate_frag
{
    output=$(cat "$shaderdir/desktop_header.glsl" "$shaderdir/sdf.frag" "$shaderdir/$1" | $glslang --stdin -S frag -l -DVALIDATING)
    if [ $? -ne 0 ]; then print_error "$1" "$output"; fi
    output=$(cat "$shaderdir/es_header.glsl" "$shaderdir/sdf.frag" "$shaderdir/$1" | $glslang --stdin -S frag -l -DVALIDATING)
    if [ $? -ne 0 ]; then print_error "$1" "$output"; fi
    output=$(cat "$shaderdir/desktop_header_core.glsl" "$shaderdir/sdf_core.frag" "$shaderdir/${1/.frag/_core.frag}" | $glslang --stdin -S frag -l -DVALIDATING)
    if [ $? -ne 0 ]; then print_error "$1" "$output"; fi
}

validate_vert "piechart.vert"
validate_frag "piechart.frag"
validate_vert "linechart.vert"
validate_frag "linechart.frag"

if [ $result -eq 0 ]; then
    echo "Successfully validated shaders, no errors found."
fi
exit $result
