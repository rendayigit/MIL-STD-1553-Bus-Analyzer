#!/bin/bash

# 1553 Bus Analyzer Uninstallation Script
# This script removes 1553 Bus Analyzer from standard Linux directories

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Installation path
BM_BINARY="/usr/local/bin/bus-monitor"
BC_BINARY="/usr/local/bin/bus-controller"
RT_BINARY="/usr/local/bin/remote-terminal"

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

# Function to check if running with sudo for system uninstallation
check_permissions() {
    if [[ -z "$SUDO_USER" ]]; then
        print_error "SUDO_USER not set. Please run this script with sudo."
        return 1
    fi
}

# Function to remove binary
remove_binaries() {
    if [[ -f "$BM_BINARY" ]]; then
        print_info "Removing binary: $BM_BINARY"
        if rm "$BM_BINARY"; then
            print_success "Binary removed successfully"
        else
            print_error "Failed to remove binary"
        fi
    else
        print_warning "Binary not found: $BM_BINARY"
    fi

    if [[ -f "$BC_BINARY" ]]; then
        print_info "Removing binary: $BC_BINARY"
        if rm "$BC_BINARY"; then
            print_success "Binary removed successfully"
        else
            print_error "Failed to remove binary"
        fi
    else
        print_warning "Binary not found: $BC_BINARY"
    fi

    # if [[ -f "$RT_BINARY" ]]; then
    #     print_info "Removing binary: $RT_BINARY"
    #     if rm "$RT_BINARY"; then
    #         print_success "Binary removed successfully"
    #     else
    #         print_error "Failed to remove binary"
    #     fi
    # else
    #     print_warning "Binary not found: $RT_BINARY"
    # fi
}

# Function to remove desktop shortcut
remove_desktop_shortcuts() {
    BM_DESKTOP_SHORTCUT="/home/$SUDO_USER/Desktop/bm.desktop"
    BC_DESKTOP_SHORTCUT="/home/$SUDO_USER/Desktop/bc.desktop"
    RT_DESKTOP_SHORTCUT="/home/$SUDO_USER/Desktop/rt.desktop"

    BM_DESKTOP_FILE="/home/$SUDO_USER/.local/share/applications/bm.desktop"
    BC_DESKTOP_FILE="/home/$SUDO_USER/.local/share/applications/bc.desktop"
    RT_DESKTOP_FILE="/home/$SUDO_USER/.local/share/applications/rt.desktop"

    BM_ICON_FILE="/home/$SUDO_USER/.local/share/icons/bm.png"
    BC_ICON_FILE="/home/$SUDO_USER/.local/share/icons/bc.png"
    RT_ICON_FILE="/home/$SUDO_USER/.local/share/icons/rt.png"

    # Remove desktop shortcuts from Desktop if they exist
    if [[ -f "$BM_DESKTOP_SHORTCUT" ]]; then
        print_info "Removing desktop shortcut from Desktop: $BM_DESKTOP_SHORTCUT"
        if rm "$BM_DESKTOP_SHORTCUT"; then
            print_success "Desktop shortcut on Desktop removed successfully"
        else
            print_error "Failed to remove desktop shortcut from Desktop"
        fi
    else
        print_warning "Desktop shortcut on Desktop not found: $BM_DESKTOP_SHORTCUT"
    fi

    if [[ -f "$BC_DESKTOP_SHORTCUT" ]]; then
        print_info "Removing desktop shortcut from Desktop: $BC_DESKTOP_SHORTCUT"
        if rm "$BC_DESKTOP_SHORTCUT"; then
            print_success "Desktop shortcut on Desktop removed successfully"
        else
            print_error "Failed to remove desktop shortcut from Desktop"
        fi
    else
        print_warning "Desktop shortcut on Desktop not found: $BC_DESKTOP_SHORTCUT"
    fi

    # if [[ -f "$RT_DESKTOP_SHORTCUT" ]]; then
    #     print_info "Removing desktop shortcut from Desktop: $RT_DESKTOP_SHORTCUT"
    #     if rm "$RT_DESKTOP_SHORTCUT"; then
    #         print_success "Desktop shortcut on Desktop removed successfully"
    #     else
    #         print_error "Failed to remove desktop shortcut from Desktop"
    #     fi
    # else
    #     print_warning "Desktop shortcut on Desktop not found: $RT_DESKTOP_SHORTCUT"
    # fi

    # Remove desktop files if they exist
    if [[ -f "$BM_DESKTOP_FILE" ]]; then
        print_info "Removing desktop shortcut: $BM_DESKTOP_FILE"
        if rm "$BM_DESKTOP_FILE"; then
            print_success "Desktop shortcut removed successfully"
        else
            print_error "Failed to remove desktop shortcut"
        fi
    else
        print_warning "Desktop shortcut not found: $BM_DESKTOP_FILE"
    fi

    if [[ -f "$BC_DESKTOP_FILE" ]]; then
        print_info "Removing desktop shortcut: $BC_DESKTOP_FILE"
        if rm "$BC_DESKTOP_FILE"; then
            print_success "Desktop shortcut removed successfully"
        else
            print_error "Failed to remove desktop shortcut"
        fi
    else
        print_warning "Desktop shortcut not found: $BC_DESKTOP_FILE"
    fi

    # if [[ -f "$RT_DESKTOP_FILE" ]]; then
    #     print_info "Removing desktop shortcut: $RT_DESKTOP_FILE"
    #     if rm "$RT_DESKTOP_FILE"; then
    #         print_success "Desktop shortcut removed successfully"
    #     else
    #         print_error "Failed to remove desktop shortcut"
    #     fi
    # else
    #     print_warning "Desktop shortcut not found: $RT_DESKTOP_FILE"
    # fi

    # Remove icon files if they exist
    if [[ -f "$BM_ICON_FILE" ]]; then
        print_info "Removing icon: $BM_ICON_FILE"
        if rm "$BM_ICON_FILE"; then
            print_success "Icon removed successfully"
        else
            print_error "Failed to remove icon"
        fi
    else
        print_warning "Icon not found: $BM_ICON_FILE"
    fi

    if [[ -f "$BC_ICON_FILE" ]]; then
        print_info "Removing icon: $BC_ICON_FILE"
        if rm "$BC_ICON_FILE"; then
            print_success "Icon removed successfully"
        else
            print_error "Failed to remove icon"
        fi
    else
        print_warning "Icon not found: $BC_ICON_FILE"
    fi

    # if [[ -f "$RT_ICON_FILE" ]]; then
    #     print_info "Removing icon: $RT_ICON_FILE"
    #     if rm "$RT_ICON_FILE"; then
    #         print_success "Icon removed successfully"
    #     else
    #         print_error "Failed to remove icon"
    #     fi
    # else
    #     print_warning "Icon not found: $RT_ICON_FILE"
    # fi
}

# Main uninstallation process
main() {
    print_info "1553 Bus Analyzer Uninstallation"
    check_permissions
    remove_binaries
    remove_desktop_shortcuts
}

# Run main function
main "$@"
