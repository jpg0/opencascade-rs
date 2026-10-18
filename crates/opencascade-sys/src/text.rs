pub use inner::*;

#[cxx::bridge]
mod inner {
    unsafe extern "C++" {
        include!("opencascade-sys/include/text.hxx");

        type OcvTextBuilder;

        type TopoDS_Shape = crate::topo_ds::TopoDS_Shape;

        #[cxx_name = "construct_unique"]
        pub fn OcvTextBuilder_new() -> UniquePtr<OcvTextBuilder>;
        #[rust_name = "init_font"]
        pub fn InitFont(self: Pin<&mut OcvTextBuilder>, data: &[u8], text_height: f64) -> bool;
        #[rust_name = "is_ready"]
        pub fn IsReady(self: &OcvTextBuilder) -> bool;
        #[rust_name = "ascender"]
        pub fn Ascender(self: &OcvTextBuilder) -> f64;
        #[rust_name = "descender"]
        pub fn Descender(self: &OcvTextBuilder) -> f64;
        #[rust_name = "line_spacing"]
        pub fn LineSpacing(self: &OcvTextBuilder) -> f64;
        #[rust_name = "scale"]
        pub fn Scale(self: &OcvTextBuilder) -> f64;
        #[rust_name = "uses_unicode_subset_fallback"]
        pub fn UsesUnicodeSubsetFallback(self: &OcvTextBuilder) -> bool;
        #[rust_name = "advance_x"]
        pub fn AdvanceX(self: Pin<&mut OcvTextBuilder>, character: u32, next: u32) -> f64;
        #[rust_name = "perform"]
        pub fn Perform(self: Pin<&mut OcvTextBuilder>, text: &str) -> bool;
        #[rust_name = "perform_centered"]
        pub fn PerformCentered(self: Pin<&mut OcvTextBuilder>, text: &str) -> bool;
        #[rust_name = "perform_right"]
        pub fn PerformRight(self: Pin<&mut OcvTextBuilder>, text: &str) -> bool;
        #[rust_name = "perform_aligned"]
        pub fn PerformAligned(
            self: Pin<&mut OcvTextBuilder>,
            text: &str,
            horizontal: u8,
            vertical: u8,
        ) -> bool;
        #[rust_name = "shape"]
        pub fn Shape(self: &OcvTextBuilder) -> &TopoDS_Shape;
    }
}

unsafe impl Send for OcvTextBuilder {}
