#!/bin/bash
set -eo pipefail

YELLOW='\033[33m'
RED='\033[31m'
BOLD='\033[1m'
NO_COLOR='\033[0m'

BUILD_MODES=("Debug" "Release" "Profiling" "Fuzzing")
BUILD_MODE="Release"

SELECTED_TARGETS=()

BUILD_TESTS=false
BUILD_FUZZ=false
BUILD_EXAMPLE=false
INCLUDE_WHAT_YOU_USE=true

function display_usage() {
	echo -e "${RED}${BOLD}Usage: $0 [${YELLOW}OPTIONS${RED}]${NO_COLOR}"
	echo -e "${BOLD}Options:${NO_COLOR}"
	echo -e "  -m, --build-mode <TYPE>    Set the build mode (${YELLOW}${BUILD_MODES[*]}${NO_COLOR}). Default is ${YELLOW}Release${NO_COLOR}."
	echo -e "  -t, --build-tests          Build tests."
	echo -e "  -f, --build-fuzz           Build fuzz."
	echo -e "  -x, --build-example        Build example."
	echo -e "  -u, --no-iwyu              Opt-out of include-what-you-use."
	echo -e "  -s, --select-targets       Select specific target(s, space-separated) to be built."
	echo -e "  -h, --help                 Display this help message."
	exit 1
}

function is_valid_build_mode() {
	local mode="$1"
	for valid_mode in "${BUILD_MODES[@]}"; do
		if [[ "$mode" == "$valid_mode" ]]; then
			return 0
		fi
	done
	return 1
}

function display_single_flag_configuration() {
	local flag="$1"
	local option="$2"

	if [ "$flag" = true ]; then
		echo -e "${BOLD}${YELLOW}${option}${NO_COLOR}: ${YELLOW}Yes${NO_COLOR}"
	else
		echo -e "${BOLD}${YELLOW}${option}${NO_COLOR}: ${YELLOW}No${NO_COLOR}"
	fi
}

# Function to display configuration
function display_configuration() {
	echo -e "${BOLD}${YELLOW}Configuration...${NO_COLOR}"

	echo -e "${BOLD}${YELLOW}BUILD_MODE${NO_COLOR}: ${YELLOW}${BUILD_MODE}${NO_COLOR}"

	if [ "${#SELECTED_TARGETS[@]}" -gt 0 ]; then
		echo -n -e "${BOLD}${YELLOW}SELECTED_TARGETS${NO_COLOR}: ${YELLOW}"

		for ((i = 0; i < "${#SELECTED_TARGETS[@]}"; i++)); do
			local target="${SELECTED_TARGETS[$i]}"

			echo -n -e "${target}"
			if [ "$i" -lt $((${#SELECTED_TARGETS[@]} - 1)) ]; then
				echo -n -e " "
			fi
		done
		echo -e "${NO_COLOR}"
	else
		echo -e "${BOLD}${YELLOW}SELECTED_TARGETS${NO_COLOR}: ${YELLOW}ALL${NO_COLOR}"
	fi

	display_single_flag_configuration $BUILD_TESTS "Tests"
	display_single_flag_configuration $BUILD_FUZZ "Fuzz"
	display_single_flag_configuration $BUILD_EXAMPLE "Example"
	display_single_flag_configuration $INCLUDE_WHAT_YOU_USE "Include-What-You-Use"

	echo ""
}

while [[ "$#" -gt 0 ]]; do
	case $1 in
	-m | --build-mode)
		if ! is_valid_build_mode "$2"; then
			echo -e "${RED}${BOLD}Invalid ${YELLOW}BUILD_MODE${RED}. Valid options: ${NO_COLOR}${YELLOW}${BUILD_MODES[*]}${NO_COLOR}."
			exit 1
		fi
		BUILD_MODE="$2"
		shift 2
		;;
	-t | --build-tests)
		BUILD_TESTS=true
		shift
		;;
	-f | --build-fuzz)
		BUILD_FUZZ=true
		shift
		;;
	-x | --build-example)
		BUILD_EXAMPLE=true
		shift
		;;
	-u | --no-iwyu)
		INCLUDE_WHAT_YOU_USE=false
		shift
		;;
	-s | --select-targets)
		shift
		while [[ "$#" -gt 0 && "$1" =~ ^[^-] ]]; do
			SELECTED_TARGETS+=("$1")
			shift
		done
		;;
	-h | --help)
		display_usage
		;;
	*)
		display_usage
		;;
	esac
done

display_configuration

CONFIGURE_CMAKE_OPTIONS=(
	-S .
	-B build/
	-D CMAKE_BUILD_TYPE="$BUILD_MODE"
	-D BUILD_TESTS="$BUILD_TESTS"
	-D BUILD_FUZZ="$BUILD_FUZZ"
	-D BUILD_EXAMPLE="$BUILD_EXAMPLE"
)

case $BUILD_MODE in
"Fuzzing")
	CONFIGURE_CMAKE_OPTIONS+=(
		-D CMAKE_C_COMPILER="/usr/local/bin/afl-clang-lto"
	)
	;;
*)
	CONFIGURE_CMAKE_OPTIONS+=(
		-D CMAKE_C_COMPILER="/usr/bin/gcc"
	)
	;;
esac

if [ "$INCLUDE_WHAT_YOU_USE" = true ]; then
	CONFIGURE_CMAKE_OPTIONS+=(
		-D CMAKE_C_INCLUDE_WHAT_YOU_USE="include-what-you-use;-w;-Xiwyu;--verbose=1"
	)
fi

echo -e "${BOLD}cmake ${CONFIGURE_CMAKE_OPTIONS[*]}${NO_COLOR}"
cmake "${CONFIGURE_CMAKE_OPTIONS[@]}"

BUILD_CMAKE_OPTIONS=(--build build/)

if [ "${#SELECTED_TARGETS[@]}" -gt 0 ]; then
	BUILD_CMAKE_OPTIONS+=("--target")
	for target in "${SELECTED_TARGETS[@]}"; do
		BUILD_CMAKE_OPTIONS+=("${target}")
	done
fi

echo -e "${BOLD}cmake ${BUILD_CMAKE_OPTIONS[*]}${NO_COLOR}"
cmake "${BUILD_CMAKE_OPTIONS[@]}"
