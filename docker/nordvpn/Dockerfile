FROM alpine

ENTRYPOINT ["openvpn"]

RUN apk add --no-cache openvpn

COPY login.conf /vpn

ENTRYPOINT openvpn --config /vpn/*.ovpn
