# OpenTTD's GOG Galaxy Integration Plugin

This plugin allows OpenTTD to announce what the player is doing on GOG Galaxy.

## Supported OpenTTD

- OpenTTD 14+

## Supported Platforms

- Windows (32bit, 64bit)
- MacOS (i386, x86_64, arm64)

## Preparation

- Lookup the latest SDK version [here](https://devportal.gog.com/galaxy/components/sdk).
- Run `./download-sdk.sh [win32|win64|macos] <SDK version>`.

In case you can't execute `download-sdk.sh`, you have to do some actions manually:

- Download the latest GOG Galaxy SDK from [here](https://devportal.gog.com/galaxy/components/sdk).
- Unpack the tar-file.
- Copy the `Include/galaxy` folder to `src/galaxy` (so there now is a `src/galaxy` folder with include-files).
- Copy the `Libraries` folder to `lib` (so there are now files in `lib`).

## Building

This is a default CMake Project.

```bash
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=$(pwd)/install -DGOG_GALAXY_CLIENT_ID=dev -DGOG_GALAXY_CLIENT_SECRET=dev ..
make
```

On MacOS, you need to add `-DCMAKE_OSX_ARCHITECTURES=[arm64|x86_64|i386]` to the `cmake` line, to select which of the targets you want to build for.

Without an actual GOG Galaxy Client ID/secret, it will not show the correct game when trying; please register your own client with GOG for testing.

## Installation

```bash
make install
```

This creates a `lib` folder in the `install` folder of your build-dir.

In your OpenTTD folder, for example in your Documents folder, there should be a `social_integration` folder (if not, start OpenTTD once to create that folder).
Copy the content of the `lib` folder into `social_integration/gog-galaxy`.

Now start OpenTTD to try out the plugin.
Start OpenTTD with `-dmisc=1` to get a bit more information about what plugins it sees and how they are loading.
