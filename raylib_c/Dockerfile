FROM emscripten/emsdk as builder

RUN apt update && apt install -y ninja-build

WORKDIR /project

COPY assets assets
COPY src src
COPY scripts scripts
COPY CMakeLists.txt CMakeLists.txt
COPY public public

RUN chmod +x ./scripts/*

RUN ./scripts/build-web.sh .

RUN ls cmake-build-empscripten

FROM busybox:latest

WORKDIR /app

COPY --from=builder /project/cmake-build-empscripten/bin/sokoban.data   releases/sokoban.data
COPY --from=builder /project/cmake-build-empscripten/bin/sokoban.html   releases/index.html
COPY --from=builder /project/cmake-build-empscripten/bin/sokoban.js     releases/sokoban.js
COPY --from=builder /project/cmake-build-empscripten/bin/sokoban.wasm   releases/sokoban.wasm
COPY --from=builder /project/public/favicon.ico                         releases/favicon.ico
COPY --from=builder /project/public/icon620x620.png                     releases/icon620x620.png
COPY --from=builder /project/public/icon.png                            releases/icon.png
COPY --from=builder /project/public/manifest.json                       releases/manifest.json
COPY --from=builder /project/public/sw.js                               releases/sw.js

RUN touch releases/httpd.conf

CMD ["busybox", "httpd", "-f", "-v", "-p", "3000", "-h", "releases", "-c", "httpd.conf"]