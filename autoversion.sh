#
# This script auto-updates a VERSION string definition.
# It outputs informational messages to stderr, while the actual
# output (on stdout) can easily be redirected to a file.
#

LATEST_RELEASE="v1.4.2"

if VER=`git describe --tags --dirty --always`; then
	echo "Setting version information: ${VER}" >&2
else
	VER=${LATEST_RELEASE}
	# try to add short commit ID on AppVeyor
	if [ -n ${APPVEYOR_REPO_COMMIT} ]; then
		VER="${VER} `echo ${APPVEYOR_REPO_COMMIT} | cut -c 1-7`"
		echo "Deriving version from commit ID: ${VER}" >&2
	else
		echo "Unable to determine current version (using \"${VER}\" as fallback)" >&2
	fi
fi
echo >&2

echo "/* Auto-generated information. DO NOT EDIT */"
echo "#define VERSION \"${VER}\""
