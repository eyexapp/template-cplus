# ── Build stage ──────────────────────────────────────────────────
FROM alpine:3.20 AS build

RUN apk update && \
    apk add \
        build-base \
        cmake \
        curl \
        git \
        gcc \
        g++ \
        libc-dev \
        linux-headers \
        ninja \
        pkgconfig \
        tar \
        unzip \
        zip

# Bootstrap vcpkg
WORKDIR /usr/local/vcpkg
ENV VCPKG_FORCE_SYSTEM_BINARIES=1
RUN curl -sSL "https://github.com/Microsoft/vcpkg/archive/2024.04.26.tar.gz" | \
    tar --strip-components=1 -zxf - \
    && ./bootstrap-vcpkg.sh -disableMetrics

# Copy source and build
COPY . /v/source
WORKDIR /v/source

RUN cmake -S /v/source -B /v/binary -GNinja \
    -DCMAKE_TOOLCHAIN_FILE=/usr/local/vcpkg/scripts/buildsystems/vcpkg.cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_TESTING=OFF

RUN cmake --build /v/binary
RUN strip /v/binary/src/app

# ── Runtime stage (scratch — minimal) ───────────────────────────
FROM scratch AS runtime
WORKDIR /r

COPY --from=build /v/binary/src/app /r/app
COPY --from=build /lib/ld-musl-x86_64.so.1 /lib/ld-musl-x86_64.so.1
COPY --from=build /usr/lib/libstdc++.so.6 /usr/lib/libstdc++.so.6
COPY --from=build /usr/lib/libgcc_s.so.1 /usr/lib/libgcc_s.so.1

ENTRYPOINT ["/r/app"]