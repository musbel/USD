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
#include "pxr/pxr.h"
#include "pxr/imaging/glf/glew.h"

#include "pxr/imaging/hdSt/meshShaderKey.h"
#include "pxr/base/tf/staticTokens.h"

PXR_NAMESPACE_OPEN_SCOPE


TF_DEFINE_PRIVATE_TOKENS(
    _tokens,
    ((baseGLSLFX,              "mesh.glslfx"))
    ((smooth,                  "MeshNormal.Smooth"))
    ((flat,                    "MeshNormal.Flat"))
    ((limit,                   "MeshNormal.Limit"))
    ((doubleSidedFS,           "MeshNormal.Fragment.DoubleSided"))
    ((singleSidedFS,           "MeshNormal.Fragment.SingleSided"))
    ((edgeNoneGS,              "MeshWire.Geometry.NoEdge"))
    ((edgeNoneFS,              "MeshWire.Fragment.NoEdge"))
    ((edgeOnlyGS,              "MeshWire.Geometry.Edge"))
    ((edgeOnlyBlendFS,         "MeshWire.Fragment.EdgeOnlyBlendColor"))
    ((edgeOnlyNoBlendFS,       "MeshWire.Fragment.EdgeOnlyNoBlend"))
    ((edgeOnSurfGS,            "MeshWire.Geometry.Edge"))
    ((edgeOnSurfFS,            "MeshWire.Fragment.EdgeOnSurface"))
    ((patchEdgeOnlyFS,         "MeshPatchWire.Fragment.EdgeOnly"))
    ((patchEdgeOnSurfFS,       "MeshPatchWire.Fragment.EdgeOnSurface"))
    ((mainVS,                  "Mesh.Vertex"))
    ((mainBSplineTCS,          "Mesh.TessControl.BSpline"))
    ((mainBezierTES,           "Mesh.TessEval.Bezier"))
    ((mainTriangleGS,          "Mesh.Geometry.Triangle"))
    ((mainQuadGS,              "Mesh.Geometry.Quad"))
    ((litFS,                   "Mesh.Fragment.Lit"))
    ((unlitFS,                 "Mesh.Fragment.Unlit"))
    ((mainFS,                  "Mesh.Fragment"))
    ((instancing,              "Instancing.Transform"))
);

HdSt_MeshShaderKey::HdSt_MeshShaderKey(
    Hd_GeometricShader::PrimitiveType primitiveType,
    bool lit,
    bool smoothNormals,
    bool doubleSided,
    bool faceVarying,
    bool blendWireframeColor,
    HdCullStyle cullStyle,
    HdMeshGeomStyle geomStyle)
    : primType(primitiveType)
    , cullStyle(cullStyle)
    , polygonMode(HdPolygonModeFill)
    , glslfx(_tokens->baseGLSLFX)
{
    if (geomStyle == HdMeshGeomStyleEdgeOnly ||
        geomStyle == HdMeshGeomStyleHullEdgeOnly) {
        polygonMode = HdPolygonModeLine;
    }

    // vertex shader
    VS[0] = _tokens->instancing;
    VS[1] = (smoothNormals ? _tokens->smooth
                           : _tokens->flat);
    VS[2] = _tokens->mainVS;
    VS[3] = TfToken();

    // tessellation control shader
    const bool isPrimTypePatches = 
        Hd_GeometricShader::IsPrimTypePatches(primType);

    TCS[0] = isPrimTypePatches ? _tokens->instancing : TfToken();
    TCS[1] = isPrimTypePatches ? _tokens->mainBSplineTCS : TfToken();
    TCS[2] = TfToken();

    // tessellation evaluation shader
    TES[0] = isPrimTypePatches ? _tokens->instancing : TfToken();
    TES[1] = isPrimTypePatches ? _tokens->mainBezierTES : TfToken();
    TES[2] = TfToken();

    // geometry shader (note that PRIM_MESH_PATCHES uses triangles)
    GS[0] = _tokens->instancing;
    GS[1] = isPrimTypePatches ? _tokens->limit : 
                (smoothNormals ? _tokens->smooth : _tokens->flat);
    GS[2] = ((geomStyle == HdMeshGeomStyleEdgeOnly ||
              geomStyle == HdMeshGeomStyleHullEdgeOnly)   ? _tokens->edgeOnlyGS
           : (geomStyle == HdMeshGeomStyleEdgeOnSurf ||
              geomStyle == HdMeshGeomStyleHullEdgeOnSurf) ? _tokens->edgeOnSurfGS
                                                          : _tokens->edgeNoneGS);

    GS[3] = Hd_GeometricShader::IsPrimTypeQuads(primType)? 
                _tokens->mainQuadGS : _tokens->mainTriangleGS;
    GS[4] = TfToken();

    // mesh special optimization:
    //    there are some cases that we can skip the geometry shader.
    if (smoothNormals
        && (geomStyle == HdMeshGeomStyleSurf || geomStyle == HdMeshGeomStyleHull)
        && Hd_GeometricShader::IsPrimTypeTriangles(primType)
        && (!faceVarying)) {
        GS[0] = TfToken();
    }
    if (Hd_GeometricShader::IsPrimTypePoints(primType)) {
        GS[0] = TfToken();
    }

    // fragment shader
    FS[0] = _tokens->instancing;
    FS[1] = (smoothNormals ? _tokens->smooth
                           : _tokens->flat);
    FS[2] = (doubleSided   ? _tokens->doubleSidedFS
                           : _tokens->singleSidedFS);
    if (isPrimTypePatches) {
        FS[3] = ((geomStyle == HdMeshGeomStyleEdgeOnly ||
                  geomStyle == HdMeshGeomStyleHullEdgeOnly)   ? _tokens->patchEdgeOnlyFS
              : ((geomStyle == HdMeshGeomStyleEdgeOnSurf ||
                  geomStyle == HdMeshGeomStyleHullEdgeOnSurf) ? _tokens->patchEdgeOnSurfFS
                                                              : _tokens->edgeNoneFS));
    } else {
        if (geomStyle == HdMeshGeomStyleEdgeOnly ||
            geomStyle == HdMeshGeomStyleHullEdgeOnly) {
            FS[3] = blendWireframeColor ? _tokens->edgeOnlyBlendFS
                                        : _tokens->edgeOnlyNoBlendFS;
        } else if (geomStyle == HdMeshGeomStyleEdgeOnSurf ||
                   geomStyle == HdMeshGeomStyleHullEdgeOnSurf) {
            FS[3] = _tokens->edgeOnSurfFS;
        } else {
            FS[3] = _tokens->edgeNoneFS;
        }
    }
    FS[4] = lit ? _tokens->litFS : _tokens->unlitFS;
    FS[5] = _tokens->mainFS;
    FS[6] = TfToken();
}

HdSt_MeshShaderKey::~HdSt_MeshShaderKey()
{
}

PXR_NAMESPACE_CLOSE_SCOPE

