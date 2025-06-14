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

COPY . .

FROM base AS runtime

WORKDIR /app
COPY --from=builder /app .

RUN cmake -B build -S . -G Ninja -DCMAKE_BUILD_TYPE=Release && \
    cmake --build build && \
    cp build/bin/vOS . && \
    rm -rf build src CMakeLists.txt

CMD ["./vOS", "docker"]
