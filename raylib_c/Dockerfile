FROM emscripten/emsdk as builder

RUN apt update && apt install -y ninja-build

WORKDIR /project

COPY assets assets
COPY src src
COPY scripts scripts
COPY CMakeLists.txt CMakeLists.txt

RUN ./scripts/build-web.sh .

RUN ls cmake-build-empscripten

FROM busybox:latest

WORKDIR /root

COPY --from=builder /project/cmake-build-empscripten/sokoban.data dist/sokoban.data
COPY --from=builder /project/cmake-build-empscripten/sokoban.html dist/index.html
COPY --from=builder /project/cmake-build-empscripten/sokoban.js dist/sokoban.js
COPY --from=builder /project/cmake-build-empscripten/sokoban.wasm dist/sokoban.wasm

RUN ls dist

RUN touch dist/httpd.conf

CMD ["busybox", "httpd", "-f", "-v", "-p", "3000", "-h", "dist", "-c", "httpd.conf"]