#!/bin/sh

set -e

if [ -z "${1}" ] || [ -z "${2}" ]; then
    echo "Usage: ${0} <os> <sdk-version>"
    exit 1
fi

if [ -e "sdk" ]; then
    echo "ERROR - 'sdk' directory already exists; please remove that directory before running this script"
    exit 1
fi

SDK_OS="${1}"
SDK_VERSION="${2}"

mkdir sdk
(
    cd sdk
    case ${SDK_OS} in
        win32)
            curl -O https://cdn.gog.com/open/galaxy/sdk/${SDK_VERSION}/Downloads/DevelopmentKit_${SDK_VERSION}_Windows_MSVC19_32bit.tar.gz
            tar -xzf DevelopmentKit_${SDK_VERSION}_Windows_MSVC19_32bit.tar.gz
            mv DevelopmentKit_${SDK_VERSION}_Windows_MSVC19_32bit DevelopmentKit
            ;;
        win64)
            curl -O https://cdn.gog.com/open/galaxy/sdk/${SDK_VERSION}/Downloads/DevelopmentKit_${SDK_VERSION}_Windows_MSVC19_64bit.tar.gz
            tar -xzf DevelopmentKit_${SDK_VERSION}_Windows_MSVC19_64bit.tar.gz
            mv DevelopmentKit_${SDK_VERSION}_Windows_MSVC19_64bit DevelopmentKit
            ;;
        macos)
            curl -O https://cdn.gog.com/open/galaxy/sdk/${SDK_VERSION}/Downloads/DevelopmentKit_${SDK_VERSION}_OSX_universal.tar.gz
            tar -xzf DevelopmentKit_${SDK_VERSION}_OSX_universal.tar.gz
            mv DevelopmentKit_${SDK_VERSION}_OSX_universal DevelopmentKit
            ;;
        *)
            echo "ERROR - Unsupported OS '${SDK_OS}'"
            exit 1
            ;;
    esac
)

rm -rf src/galaxy
cp -R sdk/DevelopmentKit/Include/galaxy src/galaxy

rm -rf lib
cp -R sdk/DevelopmentKit/Libraries lib

rm -rf sdk

echo "OK - SDK downloaded."
