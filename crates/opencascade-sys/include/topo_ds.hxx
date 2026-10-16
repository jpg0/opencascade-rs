#include <TopTools_ListOfShape.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Builder.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Vertex.hxx>
#include <bindings_common.hxx>

struct TopoDSStatics {
  static const TopoDS_Vertex &Vertex(const TopoDS_Shape &shape) { return TopoDS::Vertex(shape); }
  static const TopoDS_Edge &Edge(const TopoDS_Shape &shape) { return TopoDS::Edge(shape); }
  static const TopoDS_Wire &Wire(const TopoDS_Shape &shape) { return TopoDS::Wire(shape); }
  static const TopoDS_Face &Face(const TopoDS_Shape &shape) { return TopoDS::Face(shape); }
  static const TopoDS_Shell &Shell(const TopoDS_Shape &shape) { return TopoDS::Shell(shape); }
  static const TopoDS_Solid &Solid(const TopoDS_Shape &shape) { return TopoDS::Solid(shape); }
  static const TopoDS_Compound &Compound(const TopoDS_Shape &shape) { return TopoDS::Compound(shape); }
};
