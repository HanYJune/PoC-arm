use cc;

fn main() {
    let src = [
        "src/cbd.c",
        "src/fips202.c",
        "src/indcpa.c",
        "src/kem.c",
        "src/ntt.c",
        "src/poly.c",
        "src/polyvec.c",
        "src/reduce.c",
        "src/rng.c",
        "src/verify.c",
        "src/symmetric-shake.c"
    ];

    let mut build = cc::Build::new();
    build
        .files(src.iter())
        .include("src")
        .flag("-fomit-frame-pointer")
        .define("PRINT", "1");

    #[cfg(all(target_arch = "aarch64", target_os = "macos"))]
    {
        build.include("/opt/homebrew/opt/openssl@3/include");
    }

    build.compile("kyber-ffi");
}


