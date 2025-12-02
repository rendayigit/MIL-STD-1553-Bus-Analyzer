#!/bin/bash

# 1553 Bus Analyzer Installation Script
# This script installs 1553 Bus Analyzer to standard Linux directories

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BM_BINARY_SOURCE="$PROJECT_ROOT/bin/bus-monitor"
BC_BINARY_SOURCE="$PROJECT_ROOT/bin/bus-controller"
# RT_BINARY_SOURCE="$PROJECT_ROOT/bin/remote-terminal"

echo "Project root: $PROJECT_ROOT"

# Installation path
BIN_DIR="/usr/local/bin"

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

# Function to check if running with sudo for system installation
check_permissions() {
    if [[ -z "$SUDO_USER" ]]; then
        print_error "SUDO_USER not set. Please run this script with sudo."
        return 1
    fi
}

# Function to check if binary exists
check_binaries() {
    if [[ ! -f "$BM_BINARY_SOURCE" ]]; then
        print_error "Binary not found: $BM_BINARY_SOURCE"
    else
        print_success "$BM_BINARY_SOURCE found"
    fi
    
    if [[ ! -f "$BC_BINARY_SOURCE" ]]; then
        print_error "Binary not found: $BC_BINARY_SOURCE"
    else
        print_success "$BC_BINARY_SOURCE found"
    fi

    # if [[ ! -f "$RT_BINARY_SOURCE" ]]; then
    #     print_error "Binary not found: $RT_BINARY_SOURCE"
    # else
    #     print_success "$RT_BINARY_SOURCE found"
    # fi
}

# Function to install binary
install_binaries() {
    print_info "Installing binaries to $BIN_DIR..."
    
    if ! mkdir -p "$BIN_DIR"; then
        print_error "Failed to create bin directory: $BIN_DIR"
        exit 1
    fi
    
    if ! cp "$BM_BINARY_SOURCE" "$BIN_DIR/"; then
        print_error "Failed to copy $BM_BINARY_SOURCE to $BIN_DIR"
    fi
    
    if ! cp "$BC_BINARY_SOURCE" "$BIN_DIR/"; then
        print_error "Failed to copy $BC_BINARY_SOURCE to $BIN_DIR"
    fi

    # if ! cp "$RT_BINARY_SOURCE" "$BIN_DIR/"; then
    #     print_error "Failed to copy $RT_BINARY_SOURCE to $BIN_DIR"
    # fi

    # Set appropriate permissions
    chmod 755 "$BIN_DIR/bus-monitor"
    chmod 755 "$BIN_DIR/bus-controller"
    # chmod 755 "$BIN_DIR/remote-terminal"
}

# Create desktop shortcut and copy icon for the invoking (non-root) user
create_desktop_shortcuts() {
    APP_DIR="/home/$SUDO_USER/.local/share/applications"
    ICON_DIR="/home/$SUDO_USER/.local/share/icons"
    USER_DESKTOP_DIR="/home/$SUDO_USER/Desktop"
    
    BM_DESKTOP_FILE_CONTENT="[Desktop Entry]\nName=Run Bus Monitor\nComment=Run 1553 Bus Monitor\nExec=bus-monitor\nIcon=$ICON_DIR/bus-monitor.png\nTerminal=false\nType=Application\nCategories=Utility;\n"
    BM_DESKTOP_FILE_PATH="$APP_DIR/bm.desktop"
    BM_ICON_PATH="$PROJECT_ROOT/assets/bm.png"

    BC_DESKTOP_FILE_CONTENT="[Desktop Entry]\nName=Run Bus Controller\nComment=Run 1553 Bus Controller\nExec=bus-controller\nIcon=$ICON_DIR/bus-controller.png\nTerminal=false\nType=Application\nCategories=Utility;\n"
    BC_DESKTOP_FILE_PATH="$APP_DIR/bc.desktop"
    BC_ICON_PATH="$PROJECT_ROOT/assets/bc.png"

    RT_DESKTOP_FILE_CONTENT="[Desktop Entry]\nName=Run Remote Terminal\nComment=Run 1553 Remote Terminal\nExec=remote-terminal\nIcon=$ICON_DIR/remote-terminal.png\nTerminal=false\nType=Application\nCategories=Utility;\n"
    RT_DESKTOP_FILE_PATH="$APP_DIR/rt.desktop"
    RT_ICON_PATH="$PROJECT_ROOT/assets/rt.png"

    mkdir -p "$APP_DIR" || { print_error "Failed to create $APP_DIR"; return 1; }
    chown $SUDO_USER:$SUDO_USER "$APP_DIR" || { print_error "Failed to set ownership for $APP_DIR"; return 1; }
    mkdir -p "$ICON_DIR" || { print_error "Failed to create $ICON_DIR"; return 1; }
    chown $SUDO_USER:$SUDO_USER "$ICON_DIR" || { print_error "Failed to set ownership for $ICON_DIR"; return 1; }

    # Write desktop files
    printf "%b" "$BM_DESKTOP_FILE_CONTENT" > "$BM_DESKTOP_FILE_PATH" || { print_error "Failed to write $BM_DESKTOP_FILE_PATH"; return 1; }
    chown $SUDO_USER:$SUDO_USER "$BM_DESKTOP_FILE_PATH" || { print_error "Failed to set ownership for $BM_DESKTOP_FILE_PATH"; return 1; }

    printf "%b" "$BC_DESKTOP_FILE_CONTENT" > "$BC_DESKTOP_FILE_PATH" || { print_error "Failed to write $BC_DESKTOP_FILE_PATH"; return 1; }
    chown $SUDO_USER:$SUDO_USER "$BC_DESKTOP_FILE_PATH" || { print_error "Failed to set ownership for $BC_DESKTOP_FILE_PATH"; return 1; }

    # printf "%b" "$RT_DESKTOP_FILE_CONTENT" > "$RT_DESKTOP_FILE_PATH" || { print_error "Failed to write $RT_DESKTOP_FILE_PATH"; return 1; }
    # chown $SUDO_USER:$SUDO_USER "$RT_DESKTOP_FILE_PATH" || { print_error "Failed to set ownership for $RT_DESKTOP_FILE_PATH"; return 1; }

    # Make shortcut files executable
    chmod +x "$BM_DESKTOP_FILE_PATH" || { print_error "Failed to set executable permissions for $BM_DESKTOP_FILE_PATH"; return 1; }
    chmod +x "$BC_DESKTOP_FILE_PATH" || { print_error "Failed to set executable permissions for $BC_DESKTOP_FILE_PATH"; return 1; }
    # chmod +x "$RT_DESKTOP_FILE_PATH" || { print_error "Failed to set executable permissions for $RT_DESKTOP_FILE_PATH"; return 1; }

    # Create shortcut on user's desktop
    ln -s "$BM_DESKTOP_FILE_PATH" "$USER_DESKTOP_DIR/bm.desktop" || { print_error "Failed to create desktop shortcut for $BM_DESKTOP_FILE_PATH"; return 1; }
    ln -s "$BC_DESKTOP_FILE_PATH" "$USER_DESKTOP_DIR/bc.desktop" || { print_error "Failed to create desktop shortcut for $BC_DESKTOP_FILE_PATH"; return 1; }
    # ln -s "$RT_DESKTOP_FILE_PATH" "$USER_DESKTOP_DIR/rt.desktop" || { print_error "Failed to create desktop shortcut for $RT_DESKTOP_FILE_PATH"; return 1; }

    print_info "Created desktop shortcuts in: $APP_DIR"

    # Copy icons to icon directory
    cp "$BM_ICON_PATH" "$ICON_DIR/bus-monitor.png" || { print_error "Failed to copy $BM_ICON_PATH to $ICON_DIR"; return 1; }
    chown $SUDO_USER:$SUDO_USER "$ICON_DIR/bus-monitor.png" || { print_error "Failed to set ownership for $ICON_DIR/bus-monitor.png"; return 1; }
    cp "$BC_ICON_PATH" "$ICON_DIR/bus-controller.png" || { print_error "Failed to copy $BC_ICON_PATH to $ICON_DIR"; return 1; }
    chown $SUDO_USER:$SUDO_USER "$ICON_DIR/bus-controller.png" || { print_error "Failed to set ownership for $ICON_DIR/bus-controller.png"; return 1; }
    # cp "$RT_ICON_PATH" "$ICON_DIR/remote-terminal.png" || { print_error "Failed to copy $RT_ICON_PATH to $ICON_DIR"; return 1; }
    # chown $SUDO_USER:$SUDO_USER "$ICON_DIR/remote-terminal.png" || { print_error "Failed to set ownership for $ICON_DIR/remote-terminal.png"; return 1; }
}

# Main installation process
main() {
    print_info "1553 Bus Analyzer Installation"
    check_permissions
    check_binaries
    install_binaries
    create_desktop_shortcuts
}

# Run main function
main "$@"
