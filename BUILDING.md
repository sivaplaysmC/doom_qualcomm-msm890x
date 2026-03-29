# Building doom

## With nix

Run `nix build`. That will create a `result` directory. Then, `source ./result/bin/ndk-env`. That will set the right `CC` variable.

Once environment set, run `make doom`

## Without nix

Download ndk with android-sdk. Then, run:

```sh
export TOOLS=$SDKROOT/libexec/android-sdk/ndk/27.0.12077973/toolchains/llvm/prebuilt/linux-x86_64/bin/armv7a-linux-androideabi23-
export CC="$TOOLS-clang"
export CXX="$TOOLS-clang++"
export AR="$TOOLS-ar"
export RANLIB="$TOOLS-ranlib"
export STRIP="$TOOLS-strip"
export NM="$TOOLS-nm"
export CFLAGS="-mcpu=cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=softfp -mthumb -Os -fPIC -ffunction-sections -fdata-sections"
export LDFLAGS="-Wl,--gc-sections -Wl,--fix-cortex-a8"
make doom
```

# Running it

Get a rootshell on your device, and stop the b2g service.

```sh
stop b2g
```

Once that is done, find a way to drop the compiled executable (`doom`) and `doom1.wad` into the device.
I prefer `/data/local/tmp`.

set `HOME=/data/local/tmp` and run the executable.
