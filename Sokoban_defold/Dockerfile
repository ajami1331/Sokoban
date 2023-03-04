FROM gmantaos/defold as builder

WORKDIR /project

COPY assets assets
COPY input input
COPY main main
COPY game.project game.project

RUN ls -la

RUN bob resolve --platform js-web

RUN bob build --platform js-web --archive --strip-executable

RUN bob bundle --platform js-web --archive --strip-executable --bundle-output ./bundle

FROM busybox:latest

WORKDIR /root

COPY --from=builder /project/bundle/Sokoban_defold dist

RUN touch dist/httpd.conf

CMD ["busybox", "httpd", "-f", "-v", "-p", "3000", "-h", "dist", "-c", "httpd.conf"]