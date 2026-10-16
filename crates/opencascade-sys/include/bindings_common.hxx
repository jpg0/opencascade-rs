#pragma once
#include "rust/cxx.h"
#include <NCollection_List.hxx>
#include <memory>
#include <Standard_Version.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfVec.hxx>
#include <TColgp_Array1OfDir.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <TColgp_Array1OfXYZ.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>

#if OCC_VERSION_MAJOR >= 8
#include <Geom_BSplineCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Law_Function.hxx>
#include <Poly_Triangulation.hxx>
#include <Standard_Type.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TopTools_HSequenceOfShape.hxx>
typedef opencascade::handle<Geom_BezierCurve> Handle_Geom_BezierCurve;
typedef opencascade::handle<Geom_BezierSurface> Handle_Geom_BezierSurface;
typedef opencascade::handle<Geom_BSplineCurve> Handle_Geom_BSplineCurve;
typedef opencascade::handle<Geom_Curve> Handle_Geom_Curve;
typedef opencascade::handle<Geom_CylindricalSurface> Handle_Geom_CylindricalSurface;
typedef opencascade::handle<Geom_Plane> Handle_Geom_Plane;
typedef opencascade::handle<Geom_Surface> Handle_Geom_Surface;
typedef opencascade::handle<Geom_TrimmedCurve> Handle_Geom_TrimmedCurve;
typedef opencascade::handle<Geom2d_Curve> Handle_Geom2d_Curve;
typedef opencascade::handle<Geom2d_Ellipse> Handle_Geom2d_Ellipse;
typedef opencascade::handle<Geom2d_TrimmedCurve> Handle_Geom2d_TrimmedCurve;
typedef opencascade::handle<Law_Function> Handle_Law_Function;
typedef opencascade::handle<Poly_Triangulation> Handle_Poly_Triangulation;
typedef opencascade::handle<Standard_Type> Handle_Standard_Type;
typedef opencascade::handle<TColgp_HArray1OfPnt> Handle_TColgp_HArray1OfPnt;
typedef opencascade::handle<TopTools_HSequenceOfShape> Handle_TopTools_HSequenceOfShape;
#endif

// Generic template constructor
template <typename T, typename... Args> std::unique_ptr<T> construct_unique(Args... args) {
  return std::unique_ptr<T>(new T(args...));
}

// Type casting
template <typename T, typename U> inline U upcast(T src) { return src; }
template <typename T, typename U> inline const U &upcast_ref(const T &src) { return src; }

// Generic List
template <typename T> std::unique_ptr<std::vector<T>> list_to_vector(const NCollection_List<T> &list) {
  return std::unique_ptr<std::vector<T>>(new std::vector<T>(list.begin(), list.end()));
}

template <typename T> const T &handle_try_deref(const opencascade::handle<T> &handle) {
  if (handle.IsNull()) {
    throw std::runtime_error("null handle dereference");
  }
  return *handle;
}
