<!-- Задание выполнил Никитин Сергей 205 группа

Реализовал:

1. Базовую часть
2. Синтез последовательности изображений, содержащих поворот 3д моделей вокруг их центра (изображение 5)
3. Загрузку и рендеринг .obj файлов (изображения 10, 11)
4. Фрагментные шейдеры через указатели на функции
5. Блочный алгоритм растеризации
6. Освещение (изображение 9) -->

## How to build project

* mkdir build && cd build
* cmake -DCMAKE_BUILD_TYPE=Release (or Debug) -DUSE_OPENGL=OFF ..
* make -j 8

#### To view not black pictures of scenes need to write:
* cd ./build && make -j 8 && cd .. && ./build/testapp

