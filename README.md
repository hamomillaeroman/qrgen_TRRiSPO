# qrgen — генератор QR-кодов

Вариант 5. Преобразует строку в PNG с QR-кодом; выбор размера и уровня коррекции L/M/Q/H.

## Зависимости

    xargs -a build-deps.txt sudo apt-get install -y

## Сборка

    cmake -S . -B build
    cmake --build build
    ./build/qrgen --version

## Установка из пакета

    ./build_deb.sh
    sudo apt install ./dist/qrgen_0.1.0_amd64.deb

## Что сделано

**ПР 1** — репозиторий, программа на C, deb-пакет с conffile.

**ПР 3** — HTTP-сервер на libmicrohttpd, Docker-образ, публикация на Docker Hub.
**ПР 4** — Развертывание в кластере Kubernetes (kind), манифесты Deployment и Service.
