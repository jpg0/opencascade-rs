use std::{
    env::var,
    path::{Path, PathBuf},
};

const LIB_DIR: &str = "lib";
const INCLUDE_DIR: &str = "include";

/// Name of the environment variable holding the FreeType source tree to build
/// OCCT's text machinery against. OCCT only ever links a prebuilt FreeType, so
/// one has to be built here first.
pub const FREETYPE_DIR_ENV: &str = "OCCT_FREETYPE_DIR";

/// Get the path to the OCCT library installation directory to be
/// used in build scripts.
///
/// Only valid during build (`cargo clean` removes these files).
pub fn occt_path() -> PathBuf {
    // moves the output into target/TARGET/OCCT
    // this way its less likely to be rebuilt without a cargo clean
    Path::new(&var("OUT_DIR").expect("missing OUT_DIR")).join("../../../../OCCT")
}

/// Get the path to the static FreeType installation built alongside OCCT.
///
/// The CDylib link flags for it are emitted by the `opencascade-sys` build
/// script; this is only the prefix those flags point at.
pub fn freetype_path() -> PathBuf {
    occt_path().with_file_name("freetype")
}

/// Build a static FreeType from source.
///
/// Only the font formats a plain TrueType/OpenType face needs are compiled in:
/// zlib, bzip2, png, harfbuzz and brotli are all for other consumers, and
/// turning them off keeps the dependency list at zero.
fn build_freetype() -> PathBuf {
    let source = var(FREETYPE_DIR_ENV).unwrap_or_else(|_| {
        panic!(
            "{FREETYPE_DIR_ENV} is not set, and OCCT's text needs a FreeType source tree to \
             build against. Set it to a FreeType checkout (see .cargo/config.toml in the \
             consuming repository)."
        )
    });
    let source = PathBuf::from(source);
    assert!(
        source.join("include").join("freetype").is_dir(),
        "{FREETYPE_DIR_ENV}={} does not look like a FreeType source tree",
        source.display()
    );
    let prefix = freetype_path();
    cmake::Config::new(&source)
        .define("BUILD_SHARED_LIBS", "FALSE")
        .define("FT_DISABLE_ZLIB", "TRUE")
        .define("FT_DISABLE_BZIP2", "TRUE")
        .define("FT_DISABLE_PNG", "TRUE")
        .define("FT_DISABLE_HARFBUZZ", "TRUE")
        .define("FT_DISABLE_BROTLI", "TRUE")
        .profile("Release")
        .out_dir(&prefix)
        .build();
    assert!(
        prefix.join("lib").join("libfreetype.a").is_file(),
        "FreeType built from {} did not produce libfreetype.a in {}",
        source.display(),
        prefix.join("lib").display()
    );
    prefix
}

/// OCCT quietly drops FreeType when it cannot find it, which compiles the glyph
/// methods into stubs that return null shapes; a text Model would then render as
/// nothing with a green build. Refuse instead.
fn assert_occt_uses_freetype() {
    let cache = occt_path().join("build").join("CMakeCache.txt");
    let text = std::fs::read_to_string(&cache).unwrap_or_else(|error| {
        panic!(
            "cannot read {} to check that OCCT was configured with FreeType: {error}",
            cache.display()
        )
    });
    assert!(
        text.lines().any(|line| line.starts_with("USE_FREETYPE:BOOL=TRUE")),
        "OCCT was configured without FreeType, so it would build text geometry as null shapes. \
         FreeType came from {} and the configure log will say why it was not used.",
        freetype_path().display()
    );
}

/// Build the OCCT library.
pub fn build_occt() {
    let freetype = build_freetype();
    let freetype_include = freetype.join("include").join("freetype2");
    cmake::Config::new(Path::new(env!("OCCT_SRC_DIR")))
        .define("BUILD_LIBRARY_TYPE", "Static")
        .define("BUILD_MODULE_ApplicationFramework", "FALSE")
        .define("BUILD_MODULE_Draw", "FALSE")
        .define("BUILD_MODULE_Visualization", "FALSE")
        .define("BUILD_MODULE_DETools", "FALSE")
        .define("BUILD_DOC_Overview", "FALSE")
        .define("USE_D3D", "FALSE")
        .define("USE_DRACO", "FALSE")
        .define("USE_EIGEN", "FALSE")
        .define("USE_FFMPEG", "FALSE")
        .define("USE_FREEIMAGE", "FALSE")
        .define("USE_FREETYPE", "TRUE")
        .define("3RDPARTY_FREETYPE_DIR", &freetype)
        .define("3RDPARTY_FREETYPE_INCLUDE_DIR_ft2build", &freetype_include)
        .define("3RDPARTY_FREETYPE_INCLUDE_DIR_freetype2", &freetype_include)
        .define("3RDPARTY_FREETYPE_LIBRARY", freetype.join("lib").join("libfreetype.a"))
        .define("3RDPARTY_FREETYPE_LIBRARY_DIR", freetype.join("lib"))
        .define("USE_GLES2", "FALSE")
        .define("USE_OPENGL", "FALSE")
        .define("USE_OPENVR", "FALSE")
        .define("USE_RAPIDJSON", "FALSE")
        .define("USE_TBB", "FALSE")
        .define("USE_TCL", "FALSE")
        .define("USE_TK", "FALSE")
        .define("USE_VTK", "FALSE")
        .define("USE_XLIB", "FALSE")
        .define("INSTALL_DIR_LIB", LIB_DIR)
        .define("INSTALL_DIR_INCLUDE", INCLUDE_DIR)
        .profile("Release")
        .out_dir(occt_path())
        .build();
    assert_occt_uses_freetype();
}
