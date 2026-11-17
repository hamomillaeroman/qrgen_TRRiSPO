# qrgen — генератор QR-кодов

Вариант 5. Преобразует строку в PNG с QR-кодом; выбор размера и уровня
коррекции L/M/Q/H.
## Зависимости

    xargs -a build-deps.txt sudo apt-get install -y

## Сборка

    cmake -S . -B build
    cmake --build build
    ./build/qrgen --version

## Что сделано

**ПР 1** — в разработке.
