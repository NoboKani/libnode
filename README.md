# libnode

[![release.yml workflow status](https://github.com/NoboKani/libnode/workflows/Release/badge.svg)](https://github.com/NoboKani/libnode/actions/workflows/release.yml)

This repo contains the scripts that build [Node.js](http://nodejs.org/) as a static library

## Usage

### Configuring

#### Specify the Node version:
```sh
export LIBNODE_NODE_VERSION=v20.9.0
```

```powershell
$Env:LIBNODE_NODE_VERSION="v20.9.0"
```

#### Remove `Intl` support to reduce the size (optional):
```sh
export LIBNODE_CONFIG_FLAGS=--without-intl
export LIBNODE_ZIP_SUFFIX=-nointl
```

```powershell
$Env:LIBNODE_CONFIG_FLAGS="--without-intl"
$Env:LIBNODE_ZIP_SUFFIX="-nointl"
```

#### Build the x86 version (optional, Windows only):
```sh
export LIBNODE_X86=1
```

```powershell
$Env:LIBNODE_X86="1"
```

### Downloading the source code of Node.js:
```sh
python3 -m scripts.download
```

### Patch the source code:
```sh
python3 -m scripts.patch
```

### Building Node.js:
```sh
python3 -m scripts.build
```

### Copying the headers:
```sh
python3 -m scripts.headers
```

### Postprocessing the static library files:
```sh
python3 -m scripts.postproc
```

### Testing the library:
```sh
python3 -m scripts.test
```

### Archiving:
```sh
python3 -m scripts.archive
```
