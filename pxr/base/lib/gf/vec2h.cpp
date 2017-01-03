//
// Copyright 2016 Pixar
//
// Licensed under the Apache License, Version 2.0 (the "Apache License")
// with the following modification; you may not use this file except in
// compliance with the Apache License and the following modification to it:
// Section 6. Trademarks. is deleted and replaced with:
//
// 6. Trademarks. This License does not grant permission to use the trade
//    names, trademarks, service marks, or product names of the Licensor
//    and its affiliates, except as required to comply with Section 4(c) of
//    the License and to reproduce the content of the NOTICE file.
//
// You may obtain a copy of the Apache License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the Apache License with the above modification is
// distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the Apache License for the specific
// language governing permissions and limitations under the Apache License.
//
////////////////////////////////////////////////////////////////////////
// This file is generated by a script.  Do not edit directly.  Edit the
// vec.template.cpp file to make changes.

#include "pxr/base/gf/vec2h.h"

#include "pxr/base/gf/math.h"
#include "pxr/base/gf/ostreamHelpers.h"
#include "pxr/base/tf/type.h"

// Include headers for other vec types to support wrapping conversions and
// operators.
#include "pxr/base/gf/vec2d.h"
#include "pxr/base/gf/vec2f.h"
#include "pxr/base/gf/vec2i.h"

#include <vector>
#include <iostream>

TF_REGISTRY_FUNCTION(TfType) {
    TfType::Define<GfVec2h>();
}

std::ostream& 
operator<<(std::ostream &out, GfVec2h const &v)
{
    return out << '(' 
        << Gf_OstreamHelperP(v[0]) << ", " 
        << Gf_OstreamHelperP(v[1]) << ')';
}

GfVec2h::GfVec2h(class GfVec2d const &other)
{
    _data[0] = other[0];
    _data[1] = other[1];
}
GfVec2h::GfVec2h(class GfVec2f const &other)
{
    _data[0] = other[0];
    _data[1] = other[1];
}
GfVec2h::GfVec2h(class GfVec2i const &other)
{
    _data[0] = other[0];
    _data[1] = other[1];
}

bool
GfVec2h::operator==(GfVec2d const &other) const
{
    return _data[0] == other[0] &&
           _data[1] == other[1];
}
bool
GfVec2h::operator==(GfVec2f const &other) const
{
    return _data[0] == other[0] &&
           _data[1] == other[1];
}
bool
GfVec2h::operator==(GfVec2i const &other) const
{
    return _data[0] == other[0] &&
           _data[1] == other[1];
}

