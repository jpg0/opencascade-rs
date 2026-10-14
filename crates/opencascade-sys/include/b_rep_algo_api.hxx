#include <BRepAlgoAPI_BuilderAlgo.hxx>
#include <BRepAlgoAPI_Common.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgoAPI_Section.hxx>
#include <bindings_common.hxx>

inline void BRepAlgoAPI_Fuse_SetFuzzyValue(BRepAlgoAPI_Fuse &self, double value) { self.SetFuzzyValue(value); }
inline void BRepAlgoAPI_Fuse_SetRunParallel(BRepAlgoAPI_Fuse &self, bool flag) { self.SetRunParallel(flag); }
inline void BRepAlgoAPI_Cut_SetFuzzyValue(BRepAlgoAPI_Cut &self, double value) { self.SetFuzzyValue(value); }
inline void BRepAlgoAPI_Cut_SetRunParallel(BRepAlgoAPI_Cut &self, bool flag) { self.SetRunParallel(flag); }
inline void BRepAlgoAPI_Common_SetFuzzyValue(BRepAlgoAPI_Common &self, double value) { self.SetFuzzyValue(value); }
inline void BRepAlgoAPI_Common_SetRunParallel(BRepAlgoAPI_Common &self, bool flag) { self.SetRunParallel(flag); }
