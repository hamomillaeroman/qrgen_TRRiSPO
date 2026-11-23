FROM ubuntu:24.04
ARG VERSION=1.0.1

LABEL version="${VERSION}" description="qrgen with HTTP server"

RUN apt-get update && apt-get install -y --no-install-recommends \
    libqrencode4 libpng16-16 libmicrohttpd12 \
    && rm -rf /var/lib/apt/lists/*

# Создаем пользователя со свободным UID 10001
RUN useradd -u 10001 -m appuser

COPY build/release/qrgen /usr/local/bin/qrgen
COPY packaging/qrgen.conf /etc/qrgen/qrgen.conf

# Указываем цифровой UID
USER 10001

EXPOSE 8080
ENTRYPOINT ["/usr/local/bin/qrgen"]
CMD ["server"]
