#! /bin/bash

_HOME2_=$(dirname $0)
export _HOME2_
_HOME_=$(cd $_HOME2_;pwd)
export _HOME_

echo $_HOME_
cd $_HOME_

# docker info

mkdir -p $_HOME_/artefacts
mkdir -p $_HOME_/script
mkdir -p $_HOME_/workspace

echo '#! /bin/bash

## ----------------------
numcpus_=$(nproc)
quiet_=1
## ----------------------

echo "hello"

export qqq=""

if [ "$quiet_""x" == "1x" ]; then
	export qqq=" -qq "
fi


redirect_cmd() {
    if [ "$quiet_""x" == "1x" ]; then
        "$@" > /dev/null 2>&1
    else
        "$@"
    fi
}


echo "installing system packages ..."

redirect_cmd apt-get update $qqq

redirect_cmd apt-get install $qqq -y --force-yes --no-install-recommends lsb-release
system__=$(lsb_release -i|cut -d ':' -f2|sed -e "s#\s##g")
version__=$(lsb_release -r|cut -d ':' -f2|sed -e "s#\s##g")
echo "compiling on: $system__ $version__"

echo "installing more system packages ..."

# ---- TZ ----
export DEBIAN_FRONTEND=noninteractive
ln -fs /usr/share/zoneinfo/America/New_York /etc/localtime
redirect_cmd apt-get install $qqq -y --force-yes tzdata
redirect_cmd dpkg-reconfigure --frontend noninteractive tzdata
# ---- TZ ----

pkgs="
    rsync
    nano
    clang
    cmake
    libconfig-dev
    libgtest-dev
    libopus-dev
    libsodium-dev
    libvpx-dev
    ninja-build
    pkg-config
"

for i in $pkgs ; do
    redirect_cmd apt-get install $qqq -y --force-yes --no-install-recommends $i
done

pkgs_z="
    binutils
    llvm-dev
    libavutil-dev
    libavcodec-dev
    libavformat-dev
    libavfilter-dev
    libx264-dev
"

for i in $pkgs_z ; do
    redirect_cmd apt-get install $qqq -y --force-yes --no-install-recommends $i
done


echo ""
echo ""
echo "--------------------------------"
echo "clang version:"
c++ --version
echo "--------------------------------"
echo ""
echo ""

cd /c-toxcore

rm -Rf /workspace/_build/
rm -Rf /workspace/auto_tests/
rm -Rf /workspace/cmake/
rm -f  /workspace/CMakeLists.txt

echo "make a local copy ..."
redirect_cmd rsync -avz --exclude=".localrun" ./ /workspace/

cd /workspace/

CC=clang .circleci/cmake-tsan

mkdir -p /artefacts/tsan/
chmod a+rwx -R /workspace/
chmod a+rwx -R /artefacts/

cp /workspace/_build/Testing/Temporary/* /artefacts/tsan/
cp /workspace/_build/unit_* /workspace/_build/auto_* /artefacts/tsan/

' > $_HOME_/script/do_it___external.sh

chmod a+rx $_HOME_/script/do_it___external.sh


system_to_build_for="ubuntu:18.04"

cd $_HOME_/
docker run -ti --rm \
  -v $_HOME_/artefacts:/artefacts \
  -v $_HOME_/script:/script \
  -v $_HOME_/../:/c-toxcore \
  -v $_HOME_/workspace:/workspace \
  -e DISPLAY=$DISPLAY \
  "$system_to_build_for" \
  /bin/bash /script/do_it___external.sh

