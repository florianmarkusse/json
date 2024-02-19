#!/bin/bash
set -eo pipefail

YELLOW='\033[33m'
RED='\033[31m'
BLUE='\e[1;34m'
GREEN='\e[1;32m'
BOLD='\033[1m'
NO_COLOR='\033[0m'

function git_install_or_pull() {
	local repo=$1
	local folder=$2

	git clone "${repo}" 2>/dev/null || (cd "${folder}" && git pull && cd ../)
}

./install-dependencies.sh

mkdir -p dev-dependencies
cd dev-dependencies

echo -e "${BLUE}${BOLD}Installing code quality tools.${NO_COLOR}"
wget https://apt.llvm.org/llvm.sh
CURRENT_LLVM_STABLE=$(grep "CURRENT_LLVM_STABLE" llvm.sh | head -n 1 | mawk -F = '{ print $2 }')
if [[ -z ${CURRENT_LLVM_STABLE} ]]; then
	echo -e "${YELLOW}${BOLD}CURRENT_LLVM_STABLE${RED} is not set.${NO_COLOR}"
	exit 1
fi
if [[ ! (${CURRENT_LLVM_STABLE} =~ ^[0-9]+$) ]]; then
	echo -e "${YELLOW}${BOLD}CURRENT_LLVM_STABLE${RED} is not an integer.${NO_COLOR}"
	exit 1
fi
if [[ ${CURRENT_LLVM_STABLE} -lt 17 ]]; then
	echo -e "${YELLOW}${BOLD}CURRENT_LLVM_STABLE${RED} is below 17.${NO_COLOR}"
	exit 1
fi

echo -e "${YELLOW}${BOLD}Installing llvm-${CURRENT_LLVM_STABLE} suite.${NO_COLOR}"
chmod +x llvm.sh
sudo ./llvm.sh "${CURRENT_LLVM_STABLE}" all
rm llvm.sh
export LLVM_CONFIG="llvm-config-${CURRENT_LLVM_STABLE}"

echo -e "${YELLOW}${BOLD}Installing clang-tidy.${NO_COLOR}"
sudo apt install clang-tidy -y

echo -e "${YELLOW}${BOLD}Installing clang-format.${NO_COLOR}"
sudo apt install clang-format -y

echo -e "${YELLOW}${BOLD}Installing include-what-you-use.${NO_COLOR}"
sudo apt install iwyu -y

echo -e "${BLUE}${BOLD}Installing fuzzing tools.${NO_COLOR}"
# Required for AFL++ (how to always install newest version?)
echo -e "${YELLOW}${BOLD}Installing libstc++-12, if AFL++ fails to install afl-clang-lto this might need to be updated.${NO_COLOR}"
sudo apt install libstdc++-12-dev -y

echo -e "${YELLOW}${BOLD}Installing AFL++${NO_COLOR}"
git_install_or_pull git@github.com:AFLplusplus/AFLplusplus.git AFLplusplus
cd AFLplusplus
make
sudo make install
cd ../

echo -e "${YELLOW}${BOLD}Installing casr-afl${NO_COLOR}"
sudo apt install build-essential clang -y
curl -o rust-install.sh --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs
chmod +x rust-install.sh
./rust-install.sh -y
source "$HOME/.cargo/env"
rm rust-install.sh
sudo apt install gdb lsb-release -y
git_install_or_pull git@github.com:ispras/casr.git casr
cd casr
cargo build --release
cd ../

cd ../
echo -e "${GREEN}${BOLD}Dev Installation complete.${NO_COLOR}"
echo -e "${YELLOW}${BOLD}You may need to restart your shell for the changes to take effect.${NO_COLOR}"
