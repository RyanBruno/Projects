FROM alpine

RUN apk add --no-cache transmission-daemon

COPY settings.json /root/.config/transmission-daemon/

ENTRYPOINT transmission-daemon -f
