FROM debian:bookworm-slim AS base

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    gdb \
    git \
    curl \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Set dynamic working directory
ARG PROJECT_ROOT=/app
WORKDIR ${PROJECT_ROOT}

FROM base AS builder

# Build arguments for dynamic configuration
ARG BUILD_MODE=release
ARG CMAKE_BUILD_TYPE=Release

# Environment variables for runtime
ENV PROJECT_ROOT=/app
ENV BUILD_ROOT=/app/build
ENV CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}

# Create build scripts with dynamic paths
RUN echo '#!/bin/bash\n\
set -e\n\
echo "=== Dynamic CMake Build ==="\n\
echo "Working from: $(pwd)"\n\
echo "Project Root: ${PROJECT_ROOT}"\n\
echo "Build Root: ${BUILD_ROOT}"\n\
echo "Build Type: ${CMAKE_BUILD_TYPE}"\n\
cmake -B ${BUILD_ROOT} -S ${PROJECT_ROOT} -G Ninja -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}\n\
cmake --build ${BUILD_ROOT}\n\
echo "Build completed from $(pwd)!"' > /usr/local/bin/cmake-build && \
    chmod +x /usr/local/bin/cmake-build

RUN echo '#!/bin/bash\n\
set -e\n\
echo "=== Dynamic CMake Start ==="\n\
/usr/local/bin/cmake-build\n\
echo "Running from: $(pwd)"\n\
${BUILD_ROOT}/bin/CppProject docker' > /usr/local/bin/cmake-start && \
    chmod +x /usr/local/bin/cmake-start

RUN echo '#!/bin/bash\n\
echo "=== Dynamic CMake Clean ==="\n\
echo "Cleaning from: $(pwd)"\n\
rm -rf ${BUILD_ROOT}\n\
echo "Clean completed from $(pwd)!"' > /usr/local/bin/cmake-clean && \
    chmod +x /usr/local/bin/cmake-clean

# Copy source files
COPY . ${PROJECT_ROOT}/

# Runtime stage
FROM base AS runtime

ARG PROJECT_ROOT=/app
ENV PROJECT_ROOT=${PROJECT_ROOT}

WORKDIR ${PROJECT_ROOT}

# Copy everything from builder
COPY --from=builder ${PROJECT_ROOT} ${PROJECT_ROOT}/

# Build the application with dynamic paths
RUN cmake -B build -S . -G Ninja -DCMAKE_BUILD_TYPE=Release && \
    cmake --build build && \
    cp build/bin/CppProject . && \
    rm -rf build src CMakeLists.txt

CMD ["./CppProject", "docker"]
