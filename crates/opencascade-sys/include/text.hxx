#include <Font_FTFont.hxx>
#include <Graphic3d_HorizontalTextAlignment.hxx>
#include <Graphic3d_VerticalTextAlignment.hxx>
#include <NCollection_Buffer.hxx>
#include <NCollection_String.hxx>
#include <Standard_Handle.hxx>
#include <StdPrs_BRepFont.hxx>
#include <StdPrs_BRepTextBuilder.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <bindings_common.hxx>
#include <cstdint>
#include <cstring>
#include <gp_Ax3.hxx>
#include <string>

//! Text to B-rep with the font handed over as bytes.
//!
//! StdPrs_BRepFont can only be initialised from a file path or through
//! Font_FontMgr's system font database, and neither suits a font compiled into
//! the binary. Its protected members are reachable from a subclass, so the face
//! is installed into Font_FTFont from a buffer instead. FreeType keeps the
//! pointer it is given (FT_New_Memory_Face does not copy) and reads glyph
//! outlines out of it lazily, so the copy has to live as long as the font does.
class OcvTextFont : public StdPrs_BRepFont {
public:
  OcvTextFont() = default;

  bool InitFromMemory(const rust::Slice<const uint8_t> theData, const double theTextHeight) {
    myScaleUnits = theTextHeight / double(THE_FONT_SIZE) * 72.0 / double(THE_RESOLUTION_DPI);
    myBuffer = new NCollection_Buffer(NCollection_BaseAllocator::CommonBaseAllocator(), theData.size());
    if (myBuffer.IsNull() || myBuffer->IsEmpty()) {
      return false;
    }
    std::memcpy(myBuffer->ChangeData(), theData.data(), theData.size());
    myFTFont = new Font_FTFont();
    //! A missing glyph must be the font's own .notdef, never a system font:
    //! OCCT's Unicode-subset fallback resolves through Font_FontMgr's machine
    //! font database, so the geometry would depend on what the host has
    //! installed, and that singleton is not safe to share between threads.
    myFTFont->SetUseUnicodeSubsetFallback(false);
    return myFTFont->Init(myBuffer, "openvcad.ttf", Font_FTFontParams(THE_FONT_SIZE, THE_RESOLUTION_DPI), 0);
  }

private:
  //! The size and resolution StdPrs_BRepFont renders at, private to OCCT; they
  //! are repeated here only so myScaleUnits matches its own getScale().
  static const unsigned int THE_FONT_SIZE = 72;
  static const unsigned int THE_RESOLUTION_DPI = 4800;

  occ::handle<NCollection_Buffer> myBuffer;
};

//! Owns one memory-backed font and builds strings with it.
class OcvTextBuilder {
public:
  OcvTextBuilder() = default;

  bool InitFont(const rust::Slice<const uint8_t> theData, const double theTextHeight) {
    occ::handle<OcvTextFont> aFont = new OcvTextFont();
    if (!aFont->InitFromMemory(theData, theTextHeight)) {
      return false;
    }
    myFont = aFont;
    return true;
  }

  bool IsReady() const { return !myFont.IsNull() && !myFont->FTFont().IsNull() && myFont->FTFont()->IsValid(); }

  //! Metrics and geometry are zero or absent until a font has been installed;
  //! the caller knows whether that worked from InitFont, and nothing here can
  //! reach a FreeType face that failed to load.
  double Ascender() const { return IsReady() ? myFont->Ascender() : 0.0; }

  double Descender() const { return IsReady() ? myFont->Descender() : 0.0; }

  double LineSpacing() const { return IsReady() ? myFont->LineSpacing() : 0.0; }

  double Scale() const { return IsReady() ? myFont->Scale() : 0.0; }

  //! TRUE if this font would consider a fallback face for a missing glyph; it is
  //! switched off for the installed font, and this is how a test says so.
  bool UsesUnicodeSubsetFallback() const {
    return !myFont.IsNull() && !myFont->FTFont().IsNull() && myFont->FTFont()->ToUseUnicodeSubsetFallback();
  }

  double AdvanceX(const uint32_t theChar, const uint32_t theNext) {
    return IsReady() ? myFont->AdvanceX(char32_t(theChar), char32_t(theNext)) : 0.0;
  }

  bool Perform(const rust::Str theText) {
    return build(theText, Graphic3d_HTA_LEFT, Graphic3d_VTA_BOTTOM);
  }

  //! Centred and right-aligned variants; OCCT lays each line out from advances,
  //! so these are per line rather than about the block's ink box.
  bool PerformCentered(const rust::Str theText) {
    return build(theText, Graphic3d_HTA_CENTER, Graphic3d_VTA_BOTTOM);
  }

  bool PerformRight(const rust::Str theText) {
    return build(theText, Graphic3d_HTA_RIGHT, Graphic3d_VTA_BOTTOM);
  }

  //! Both alignments at once, handed over as plain integers because cxx cannot
  //! bind an OCCT enum. Each code is its own enum's declaration order, so
  //! horizontal 0..2 is HTA_LEFT/CENTER/RIGHT and vertical 0..3 is VTA_BOTTOM/
  //! CENTER/TOP/TOPFIRSTLINE. A code outside those ranges is refused rather than
  //! defaulted, because either default would silently place the text on an
  //! anchor nobody asked for.
  bool PerformAligned(const rust::Str theText, const uint8_t theHAlign, const uint8_t theVAlign) {
    Graphic3d_HorizontalTextAlignment anAlignX;
    switch (theHAlign) {
      case 0:
        anAlignX = Graphic3d_HTA_LEFT;
        break;
      case 1:
        anAlignX = Graphic3d_HTA_CENTER;
        break;
      case 2:
        anAlignX = Graphic3d_HTA_RIGHT;
        break;
      default:
        return false;
    }

    Graphic3d_VerticalTextAlignment anAlignY;
    switch (theVAlign) {
      case 0:
        anAlignY = Graphic3d_VTA_BOTTOM;
        break;
      case 1:
        anAlignY = Graphic3d_VTA_CENTER;
        break;
      case 2:
        anAlignY = Graphic3d_VTA_TOP;
        break;
      case 3:
        anAlignY = Graphic3d_VTA_TOPFIRSTLINE;
        break;
      default:
        return false;
    }

    return build(theText, anAlignX, anAlignY);
  }

  const TopoDS_Shape &Shape() const { return myShape; }

private:
  bool build(const rust::Str theText,
             const Graphic3d_HorizontalTextAlignment theAlignX,
             const Graphic3d_VerticalTextAlignment theAlignY) {
    if (!IsReady()) {
      return false;
    }
    const std::string aText(theText.data(), theText.size());
    if (aText.find('\0') != std::string::npos) {
      return false;
    }
    StdPrs_BRepTextBuilder aBuilder;
    myShape =
        aBuilder.Perform(*myFont, NCollection_String(aText.c_str()), gp_Ax3(), theAlignX, theAlignY);
    const TopoDS_Iterator anIterator(myShape);
    return anIterator.More();
  }

  occ::handle<OcvTextFont> myFont;
  TopoDS_Shape myShape;
};
