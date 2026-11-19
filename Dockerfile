FROM ubuntu:24.04
ARG VERSION=0.3.0

LABEL version="${VERSION}" \
      description="qrgen — генератор QR-кодов с HTTP сервером"

# Устанавливаем только библиотеки времени выполнения
RUN apt-get update && apt-get install -y --no-install-recommends \
    libqrencode4 \
    libpng16-16 \
    libmicrohttpd12 \
    && rm -rf /var/lib/apt/lists/*

# Копируем скомпилированный бинарник
COPY build/release/qrgen /usr/local/bin/qrgen
COPY packaging/qrgen.conf /etc/qrgen/qrgen.conf

# Открываем порт
EXPOSE 8080

ENTRYPOINT ["/usr/local/bin/qrgen"]
CMD ["server"]
