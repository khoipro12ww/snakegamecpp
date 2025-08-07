#!/bin/bash

# ECS Game Build Script
# Hỗ trợ build project với SFML 3.0

set -e  # Exit on any error

echo "=== ECS Game Build Script ==="

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Check if cmake is installed
if ! command -v cmake &> /dev/null; then
    echo -e "${RED}Error: CMake is not installed${NC}"
    echo "Please install CMake first:"
    echo "  Ubuntu/Debian: sudo apt install cmake"
    echo "  macOS: brew install cmake"
    exit 1
fi

# Check if compiler is available
if ! command -v g++ &> /dev/null && ! command -v clang++ &> /dev/null; then
    echo -e "${RED}Error: No C++ compiler found${NC}"
    echo "Please install a C++ compiler:"
    echo "  Ubuntu/Debian: sudo apt install build-essential"
    echo "  macOS: xcode-select --install"
    exit 1
fi

# Function to print colored output
print_status() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Parse command line arguments
BUILD_TYPE="Release"
CLEAN_BUILD=false
INSTALL_DEPS=false
RUN_AFTER_BUILD=false

while [[ $# -gt 0 ]]; do
    case $1 in
        -d|--debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        -c|--clean)
            CLEAN_BUILD=true
            shift
            ;;
        -i|--install-deps)
            INSTALL_DEPS=true
            shift
            ;;
        -r|--run)
            RUN_AFTER_BUILD=true
            shift
            ;;
        -h|--help)
            echo "Usage: $0 [OPTIONS]"
            echo "Options:"
            echo "  -d, --debug       Build in Debug mode (default: Release)"
            echo "  -c, --clean       Clean build directory first"
            echo "  -i, --install-deps Install SFML dependencies"
            echo "  -r, --run         Run the game after successful build"
            echo "  -h, --help        Show this help message"
            exit 0
            ;;
        *)
            print_error "Unknown option: $1"
            echo "Use -h or --help for usage information"
            exit 1
            ;;
    esac
done

print_status "Build configuration: $BUILD_TYPE"

# Install dependencies if requested
if [ "$INSTALL_DEPS" = true ]; then
    print_status "Installing dependencies..."
    
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        # Linux
        print_status "Detected Linux system"
        sudo apt update
        sudo apt install -y cmake build-essential git
        
        # Check if SFML 3.0 is available
        if ! pkg-config --exists sfml-graphics; then
            print_warning "SFML not found in system packages"
            print_status "Building SFML 3.0 from source..."
            
            # Create temp directory for SFML build
            TEMP_DIR=$(mktemp -d)
            cd "$TEMP_DIR"
            
            git clone https://github.com/SFML/SFML.git
            cd SFML
            git checkout 3.0.0
            
            mkdir build
            cd build
            cmake .. -DCMAKE_BUILD_TYPE=Release
            make -j$(nproc)
            sudo make install
            sudo ldconfig
            
            cd - > /dev/null
            rm -rf "$TEMP_DIR"
            
            print_status "SFML 3.0 installed successfully"
        else
            print_status "SFML found in system"
        fi
        
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        # macOS
        print_status "Detected macOS system"
        if command -v brew &> /dev/null; then
            brew install cmake sfml
        else
            print_error "Homebrew not found. Please install Homebrew first."
            exit 1
        fi
    else
        print_warning "Unsupported OS for automatic dependency installation"
        print_status "Please install SFML 3.0 manually"
    fi
fi

# Get script directory
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
cd "$SCRIPT_DIR"

# Clean build directory if requested
if [ "$CLEAN_BUILD" = true ]; then
    print_status "Cleaning build directory..."
    rm -rf build
fi

# Create build directory
print_status "Creating build directory..."
mkdir -p build
cd build

# Configure with CMake
print_status "Configuring project with CMake..."
cmake .. \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# Build the project
print_status "Building project..."
CPU_COUNT=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
make -j"$CPU_COUNT"

if [ $? -eq 0 ]; then
    print_status "Build completed successfully!"
    
    # Show build info
    echo
    echo "Build Information:"
    echo "  Build Type: $BUILD_TYPE"
    echo "  Executable: $(pwd)/ECS_Game"
    echo "  Build Time: $(date)"
    
    # Run the game if requested
    if [ "$RUN_AFTER_BUILD" = true ]; then
        echo
        print_status "Running the game..."
        ./ECS_Game
    else
        echo
        print_status "To run the game: cd build && ./ECS_Game"
    fi
else
    print_error "Build failed!"
    exit 1
fi