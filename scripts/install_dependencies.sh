#!/bin/bash

# 1553 Bus Analyzer Dependency Installation Script
# This script installs 1553 Bus Analyzer dependencies

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Functions to print colored output
print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to install dependencies
install_dependencies() {
    # Check if running on Ubuntu/Debian
    if ! command -v apt &> /dev/null; then
        print_error "Error: This script is designed for Ubuntu/Debian systems with apt package manager."
        exit 1
    fi

    # Update package lists
    print_info "Updating package lists..."
    sudo apt update

    print_info "Installing core dependencies..."
    sudo apt install -y \
        build-essential \
        cmake \
        libboost-all-dev \
        nlohmann-json3-dev \
        libfmt-dev \
        libspdlog-dev

    print_info "Installing wxWidgets dependencies..."

    # Detect Ubuntu version and install appropriate wxWidgets packages
    # Ubuntu 24.04+ uses wxWidgets 3.2.x, while older versions use 3.0.x
    UBUNTU_VERSION=$(lsb_release -rs 2>/dev/null || echo "unknown")
    print_info "Detected Ubuntu/distribution version: $UBUNTU_VERSION"

    # Check if wxWidgets 3.2 packages are available (Ubuntu 24.04+)
    if apt-cache search libwxgtk3.2-dev | grep -q libwxgtk3.2-dev; then
        print_info "Installing wxWidgets 3.2 packages (newer systems)..."
        sudo apt install -y \
            libwxgtk3.2-dev \
            wx3.2-headers \
            wx-common
    elif apt-cache search libwxgtk3.0-gtk3-dev | grep -q libwxgtk3.0-gtk3-dev; then
        print_info "Installing wxWidgets 3.0 packages (older systems)..."
        sudo apt install -y \
            libwxgtk3.0-gtk3-dev \
            wx3.0-headers \
            wx-common
    else
        print_error "No compatible wxWidgets development packages found."
        print_info "Please install wxWidgets manually or check your package repositories."
        exit 1
    fi
}

# Function to check if running as root for system installation
check_permissions() {
    if [[ $EUID -ne 0 ]]; then
        print_error "Installation requires root privileges."
        print_info "Run with sudo: sudo ./install_dependencies.sh"
    fi
}

# Main installation process
main() {
    print_info "1553 Bus Analyzer Dependency Installation"
    check_permissions
    install_dependencies
    print_success "All 1553 Bus Analyzer dependencies installed successfully!"
}

# Run main function
main "$@"
