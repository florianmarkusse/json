#!/bin/bash
set -eo pipefail

YELLOW='\033[33m'
BLUE='\e[1;34m'
GREEN='\e[1;32m'
BOLD='\033[1m'
NO_COLOR='\033[0m'

echo -e "${BLUE}${BOLD}Upgrading installation.${NO_COLOR}"
sudo apt update && sudo apt upgrade -y && sudo apt dist-upgrade -y

echo -e "${BLUE}${BOLD}Installing basic build tools.${NO_COLOR}"

echo -e "${YELLOW}${BOLD}Installing gcc. ${NO_COLOR}"
sudo apt install gcc -y

echo -e "${YELLOW}${BOLD}Installing cmake. ${NO_COLOR}"
sudo apt install cmake -y

echo -e "${GREEN}${BOLD}Basic Installation complete.${NO_COLOR}"
echo -e "${YELLOW}${BOLD}You may need to restart your shell for the changes to take effect.${NO_COLOR}"
