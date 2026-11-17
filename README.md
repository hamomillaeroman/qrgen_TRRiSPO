# qrgen — генератор QR-кодов

QR-код из строки в PNG; выбор
размера и уровня коррекции

## Зависимости

    xargs -a build-deps.txt sudo apt-get install -y

## Сборка

    cmake -S . -B build
    cmake --build build
    ./build/qrgen --version

## Что сделано

**ПР 1** — в разработке.
