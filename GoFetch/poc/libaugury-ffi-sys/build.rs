use cc;

fn main() {
    let src = [
        "src/c_augury.c"
    ];

    cc::Build::new()
        .files(src.iter())
        .include("src")
        .static_flag(true)
        .compile("augury-ffi");
}
