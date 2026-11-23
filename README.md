# qrgen — Генератор QR-кодов (Вариант 5)

Программа для генерации QR-кодов из строки в формате PNG с возможностью выбора размера и уровня коррекции (L, M, Q, H). Включает CLI-интерфейс и HTTP-сервер.

## 1. Зависимости

Список всех необходимых пакетов для сборки находится в файле `build-deps.txt`. Установка:

    xargs -a build-deps.txt sudo apt-get install -y

## 2. Сборка из исходников

    cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
    cmake --build build

## 3. Запуск (CLI)

**Генерация PNG-файла:**
    ./build/qrgen generate --text "Привет, мир!" --output qr.png --size 256 --level 2

**Запуск HTTP-сервера:**
    ./build/qrgen server
    # Откройте в браузере: http://localhost:8080/?text=Hello

## 4. Сборка и установка .deb пакета

    ./build_deb.sh
    sudo apt install ./dist/qrgen_1.0.0_amd64.deb

**Удаление:**
    sudo apt remove qrgen     # Конфигурация в /etc/qrgen сохраняется
    sudo apt purge qrgen      # Полное удаление вместе с конфигурацией

## 5. Docker (ПР 3)

Сборка и запуск контейнера:
    docker build -t hamomillaeroman/qrgen:0.3.0 .
    docker run -p 8080:8080 -d hamomillaeroman/qrgen:0.3.0

## 6. Kubernetes (ПР 4)

Развертывание в кластере (kind или minikube):
    kubectl apply -f k8s/
    kubectl port-forward service/qrgen-service 8080:80
    # Откройте в браузере: http://localhost:8080/?text=Kubernetes

---

## 📋 Что сделано по практическим работам:

### ПР 1. Репозиторий, программа и deb-пакет
* Реализована программа на C11. Логика вынесена в статическую библиотеку `qrcore` (файлы `core.h`, `core.c`). Логика не печатает в консоль и не открывает файлы.
* Ввод-вывод, разбор аргументов (`getopt_long`), подкоманды (`generate`, `server`) и чтение конфигурации `/etc/qrgen/qrgen.conf` вынесены в `main.c`.
* Настроена сборка CMake, версия автоматически подтягивается из файла `VERSION`.
* Написан скрипт `build_deb.sh` для сборки deb-пакета. Зависимости определяются автоматически через `dpkg-shlibdeps`. Конфигурация объявлена как `conffile`.

### ПР 2. Непрерывная интеграция (CI/CD)
* Написаны модульные тесты (8+ проверок) с использованием CTest, включая граничные случаи.
* Настроен конвейер GitHub Actions (`.github/workflows/ci.yml`), состоящий из этапов: `build` (сборка с `-Werror` и `cppcheck`), `test` (тесты с санитайзерами ASan/UBSan и проверка покрытия `gcovr` не ниже 70%) и `package` (сборка `.deb` и проверка `lintian`).
* Настроена задача `release` для автоматической публикации релиза на GitHub с прикрепленным `.deb` файлом при пуше тега `v*`.
### ПР 3. Контейнеризация и HTTP-сервер
* Разработан HTTP-сервер на базе библиотеки `libmicrohttpd`. Сервер принимает GET-запросы (например, `/?text=Hello`) и возвращает сгенерированный QR-код в формате PNG.
* Написан `Dockerfile` на базе Ubuntu 24.04. Образ собирает в себя только необходимые библиотеки времени выполнения.
* Docker-образ опубликован на Docker Hub (`hamomillaeroman/qrgen:0.3.0`).

### ПР 4. Оркестрация в Kubernetes
* Настроен локальный кластер Kubernetes с помощью `kind`.
* Написаны манифесты: `k8s/deployment.yaml` (запуск 2 реплик приложения) и `k8s/service.yaml` (ClusterIP для внутреннего балансирования трафика).
* Приложение успешно развернуто в кластере и проверено через проброс портов (`kubectl port-forward`).
