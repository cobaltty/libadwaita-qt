#!/bin/bash
# Build RPM package for libadwaita-qt6
# Run from the project root directory on your host (not in container)
#
# Prerequisites (Fedora):
#   sudo dnf install rpm-build rpmdevtools qt6-qtbase-devel qt6-qtwayland-devel ninja-build
#
# Usage:
#   ./build-rpm.sh

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
NAME="libadwaita-qt6"
VERSION="1.4.50"

# Setup rpmbuild tree
RPMBUILD_DIR="${HOME}/rpmbuild"
mkdir -p "${RPMBUILD_DIR}/BUILD" "${RPMBUILD_DIR}/RPMS" "${RPMBUILD_DIR}/SOURCES" "${RPMBUILD_DIR}/SPECS" "${RPMBUILD_DIR}/SRPMS"

# Create source tarball
TARBALL="${RPMBUILD_DIR}/SOURCES/${NAME}-${VERSION}.tar.gz"
echo "Creating source tarball..."
TMPDIR=$(mktemp -d)
cp -a "${SCRIPT_DIR}" "${TMPDIR}/${NAME}-${VERSION}"
tar czf "${TARBALL}" -C "${TMPDIR}" "${NAME}-${VERSION}"
rm -rf "${TMPDIR}"
echo "  -> ${TARBALL}"

# Copy spec file
cp "${SCRIPT_DIR}/${NAME}.spec" "${RPMBUILD_DIR}/SPECS/"

# Build
echo "Building RPM..."
rpmbuild -ba "${RPMBUILD_DIR}/SPECS/${NAME}.spec"

echo ""
echo "Done. RPMs in:"
ls -1 "${RPMBUILD_DIR}"/RPMS/*/*.rpm 2>/dev/null
ls -1 "${RPMBUILD_DIR}"/SRPMS/*.rpm 2>/dev/null
echo ""
echo "Install with:"
echo "  sudo dnf install ${RPMBUILD_DIR}/RPMS/x86_64/${NAME}-${VERSION}*.rpm"
