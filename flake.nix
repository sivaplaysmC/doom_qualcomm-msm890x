# WARN: Slop gen'd
{
  description = "Android NDK cross-compilation toolchain (armv7a, API 23)";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-25.11";

  outputs = { nixpkgs, ... }:
    let
      system = "x86_64-linux";

      pkgs = import nixpkgs {
        system = "x86_64-linux";
        config.allowUnfree = true;
        # config.allowUnfreePredicate = pkg: builtins.elem (nixpkgs.lib.getName pkg) [ "edl" ];
        config.android_sdk.accept_license = true;  # required for NDK
      };

      ndkVersion = "27.0.12077973";
      api        = "23";

      android = pkgs.androidenv.composeAndroidPackages {
        includeNDK = true;
        ndkVersions = [ ndkVersion ];
      };

      ndkRoot   = "${android.androidsdk}/libexec/android-sdk/ndk/${ndkVersion}";
      toolchain = "${ndkRoot}/toolchains/llvm/prebuilt/linux-x86_64";
      bin       = "${toolchain}/bin";
      triple    = "armv7a-linux-androideabi${api}";

    in
    {
      packages.${system}.default = pkgs.buildEnv {
        name  = "ndk-toolchain";
        paths = [
          (pkgs.writeShellScriptBin "cc"     "exec ${bin}/${triple}-clang   \"$@\"")
          (pkgs.writeShellScriptBin "c++"    "exec ${bin}/${triple}-clang++ \"$@\"")
          (pkgs.writeShellScriptBin "ar"     "exec ${bin}/llvm-ar           \"$@\"")
          (pkgs.writeShellScriptBin "ranlib" "exec ${bin}/llvm-ranlib       \"$@\"")
          (pkgs.writeShellScriptBin "strip"  "exec ${bin}/llvm-strip        \"$@\"")
          (pkgs.writeShellScriptBin "nm"     "exec ${bin}/llvm-nm           \"$@\"")
          (pkgs.writeShellScriptBin "make"   "exec ${pkgs.gnumake}/bin/make \"$@\"")

          (pkgs.writeShellScriptBin "ndk-env" ''
            export CC="${bin}/${triple}-clang"
            export CXX="${bin}/${triple}-clang++"
            export AR="${bin}/llvm-ar"
            export RANLIB="${bin}/llvm-ranlib"
            export STRIP="${bin}/llvm-strip"
            export NM="${bin}/llvm-nm"
            export CFLAGS="-mcpu=cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=softfp -mthumb -Os -fPIC -ffunction-sections -fdata-sections"
            export CXXFLAGS="$CFLAGS"
            export LDFLAGS="-Wl,--gc-sections -Wl,--fix-cortex-a8"
            echo "NDK ${ndkVersion} (${triple}) loaded" >&2
          '')
        ];
      };
    };
}

# vim: sw=2

