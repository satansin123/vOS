FROM debian:bookworm-slim AS base

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    gdb \
    git \
    curl \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

FROM base AS builder

# Create dynamic build scripts
RUN echo '#!/bin/bash\n\
set -e\n\
PROJECT_NAME=$(basename $(pwd))\n\
echo "=== Building: $PROJECT_NAME ==="\n\
cmake -B build -S . -G Ninja -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE:-Release}\n\
cmake --build build\n\
echo "Build completed for: $PROJECT_NAME"' > /usr/local/bin/cmake-build && \
    chmod +x /usr/local/bin/cmake-build

COPY . .

FROM base AS runtime

WORKDIR /app
COPY --from=builder /app .

RUN PROJECT_NAME=$(basename $(pwd)) && \
    cmake -B build -S . -G Ninja -DCMAKE_BUILD_TYPE=Release && \
    cmake --build build && \
    cp build/bin/$PROJECT_NAME . && \
    rm -rf build src CMakeLists.txt

CMD bash -c 'PROJECT_NAME=$(basename $(pwd)) && ./$PROJECT_NAME docker'
