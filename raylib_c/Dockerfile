FROM emscripten/emsdk as builder

RUN apt update && apt install -y ninja-build

WORKDIR /project

COPY assets assets
COPY src src
COPY scripts scripts
COPY CMakeLists.txt CMakeLists.txt

RUN chmod +x ./scripts/*

RUN ./scripts/build-web.sh .

RUN ls cmake-build-empscripten

FROM busybox:latest

WORKDIR /root

COPY --from=builder /project/cmake-build-empscripten/bin/sokoban.data app/releases/sokoban.data
COPY --from=builder /project/cmake-build-empscripten/bin/sokoban.html app/releases/index.html
COPY --from=builder /project/cmake-build-empscripten/bin/sokoban.js app/releases/sokoban.js
COPY --from=builder /project/cmake-build-empscripten/bin/sokoban.wasm app/releases/sokoban.wasm

RUN touch app/releases/httpd.conf

CMD ["busybox", "httpd", "-f", "-v", "-p", "3000", "-h", "app/releases", "-c", "httpd.conf"]