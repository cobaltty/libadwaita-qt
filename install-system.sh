#!/bin/bash
# Build and install libadwaita as a system-wide Qt6 style plugin.
# Run from the libadwaita root directory.
#
# Usage:
#   ./install-system.sh          # build + install (requires sudo)
#   ./install-system.sh -- uninstall

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build"
PREFIX="${PREFIX:-/usr}"

if [ "${1:-}" = "--" ]; then
    echo "Uninstalling libadwaita ..."
    sudo rm -f "${PREFIX}/lib64/qt6/plugins/styles/libadwaita.so" 2>/dev/null || \
    sudo rm -f "${PREFIX}/lib/qt6/plugins/styles/libadwaita.so" 2>/dev/null || \
    sudo rm -f "${PREFIX}/lib/x86_64-linux-gnu/qt6/plugins/styles/libadwaita.so" 2>/dev/null || \
        echo "Style plugin not found in standard paths"
    sudo rm -f "${PREFIX}/lib64/libadwaita6.so"* 2>/dev/null || true
    sudo rm -f "${PREFIX}/lib/libadwaita6.so"* 2>/dev/null || true
    sudo rm -f "${PREFIX}/lib/x86_64-linux-gnu/libadwaita6.so"* 2>/dev/null || true
    sudo rm -f "${PREFIX}/lib64/libadwaita6priv.so"* 2>/dev/null || true
    sudo rm -f "${PREFIX}/lib/libadwaita6priv.so"* 2>/dev/null || true
    sudo rm -f "${PREFIX}/lib/x86_64-linux-gnu/libadwaita6priv.so"* 2>/dev/null || true
    sudo ldconfig
    echo "Uninstalled."
    exit 0
fi

# Detect Qt6 plugin path
QT6_PLUGINS_DIR=""
for candidate in \
    "$(qmake6 -query QT_INSTALL_PLUGINS 2>/dev/null)" \
    "${PREFIX}/lib64/qt6/plugins" \
    "${PREFIX}/lib/qt6/plugins" \
    "${PREFIX}/lib/x86_64-linux-gnu/qt6/plugins"; do
    if [ -n "${candidate}" ] && [ -d "$(dirname "${candidate}")" ]; then
        QT6_PLUGINS_DIR="${candidate}"
        break
    fi
done

if [ -z "${QT6_PLUGINS_DIR}" ]; then
    echo "Error: Could not determine Qt6 plugin directory."
    echo "Set QT6_PLUGINS_DIR manually and re-run."
    exit 1
fi

echo "Qt6 plugin dir: ${QT6_PLUGINS_DIR}"

# Detect library path
LIB_DIR=""
for candidate in \
    "${PREFIX}/lib64" \
    "${PREFIX}/lib" \
    "${PREFIX}/lib/x86_64-linux-gnu"; do
    if [ -d "${candidate}" ]; then
        LIB_DIR="${candidate}"
        break
    fi
done

if [ -z "${LIB_DIR}" ]; then
    echo "Error: Could not determine library directory."
    exit 1
fi

echo "Library dir: ${LIB_DIR}"

# Build
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"
cmake "${SCRIPT_DIR}" -DUSE_QT6=ON -DCMAKE_BUILD_TYPE=Release -DBUILD_EXAMPLE=OFF
cmake --build . --parallel "$(nproc)"

# Find the built files
STYLE_SO=""
for candidate in \
    "${BUILD_DIR}/plugins/styles/libadwaita.so" \
    "${BUILD_DIR}/src/style/libadwaita.so"; do
    if [ -f "${candidate}" ]; then
        STYLE_SO="${candidate}"
        break
    fi
done

if [ -z "${STYLE_SO}" ]; then
    echo "Error: libadwaita.so not found after build."
    exit 1
fi

LIB6="${BUILD_DIR}/src/lib/libadwaita6.so.1.4.50"
LIB6PRIV="${BUILD_DIR}/src/lib/libadwaita6priv.so.1.4.50"

echo "Installing:"
echo "  ${STYLE_SO} -> ${QT6_PLUGINS_DIR}/styles/"
echo "  ${LIB6} -> ${LIB_DIR}/"
echo "  ${LIB6PRIV} -> ${LIB_DIR}/"

sudo install -Dm755 "${STYLE_SO}" "${QT6_PLUGINS_DIR}/styles/libadwaita.so"
sudo install -Dm755 "${LIB6}" "${LIB_DIR}/libadwaita6.so.1.4.50"
sudo install -Dm755 "${LIB6PRIV}" "${LIB_DIR}/libadwaita6priv.so.1.4.50"
sudo ln -sf libadwaita6.so.1.4.50 "${LIB_DIR}/libadwaita6.so.1"
sudo ln -sf libadwaita6.so.1 "${LIB_DIR}/libadwaita6.so"
sudo ln -sf libadwaita6priv.so.1.4.50 "${LIB_DIR}/libadwaita6priv.so.1"
sudo ln -sf libadwaita6priv.so.1 "${LIB_DIR}/libadwaita6priv.so"

sudo ldconfig

echo ""
echo "Installed. Test with:"
echo "  QT_STYLE_OVERRIDE=libadwaita designer-qt6"
echo ""
echo "To uninstall: $0 --"
