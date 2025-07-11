# Merge MKV GUI

Небольшой GUI поверх MKVToolNix который позволяет легко добавлять в MKV файлы (и не только!) аудио дорожки и субтитры пакетным (batch) образом.
Приложение преднозначено для Windows. Для macOS и Linux требует небольших изменений.

![example](docs/example.png)

## Требования

- Чтобы собрать:
  - CMake
  - C++ compiler, should support C++20 standard
  - [dep](https://github.com/cprkv/dep) - используется в качестве менеджера зависимостей
  - `cd deps/wxWidgets; git submodule update --init --recursive`
- Чтобы запустить:
  - MKVToolNix

## Как работает

Приложению нужны 3 директории (drag-n-drop поддерживается):

- Где лежат файлы `.mkv`
- Где лежат файлы `.ass` (опционально)
- Где лежат файлы `.mka` (опционально)

После их указания, нужно нажать кнопку `process` и тогда файлы объединятся в `директория где лежат mkv`/`merged`.

Файлы объединяются друг с другом по отличиям в группе. Вот пример общих черт в каждой группе, они учитываться не будут:

![common-parts](docs/common-parts.png)

Вот пример отличий, и по ним файлы будут объединены:

![join-parts](docs/join-parts.png)
