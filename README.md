# Исследование хеш-таблиц

## Введение

В данной работе рассматриваются алгоритмы хеширования и способы её оптимизации хеш-таблицы. Поэтому работа разделена на 2 соответствующие части. Итак, приступим.

## 1. Алгоритмы хеширования

### 0.0) Требования к алгоритму

Хеш-таблица - структура данных, работающая с данными вида "ключ - значение". В нашем случае и ключ, и значение - строковые данные. Для быстрого доступа по ключу используется хеширование - сопоставление ключу какого-то числа, по которому производится доступ в таблицу. Т.к. хеш может получится довольно большим, сама пара ключ-значение будет храниться в ячейке [hash % size].

Вполне возможно, что значение hash % size для нескольких пар к.-з. совпадёт. Поэтому во время поиска по таблице придётся перебирать все ключи, соответствующие данной ячейке (они хранятся методом цепочек), и сравнивать с данным. Эта операция занимает наибольшее время, затраты на неё нужно оптимизировать.

Из этих соображений очевидно, что главное требование к алгоритму подсчёта контрольной суммы (хеша) - равномерное распределение по всем ячейкам таблицы.

### 0.1) Методика измерения

Для этой работы мы написали хеш-таблицу, в которой реализована функция заполнения из файла. На вход будет подаваться небольшой англо-русский словарь (порядка 6500 слов). Размер таблицы, будет сжат с рекомендуемых 1,5 элемента на ячейку до 10-15 для того, чтобы большинство ячеек было заполнено.

Коллизии, как было сказано ранее, будут решаться методом цепочек. Поэтому в массиве будут храниться указатели на первый элемент списка, а в самих элементах (структурах для хранения пары к.-з.) - на следующий. С кодом можно ознакомиться в папке "Lab 1".

Далее с помощью библиотеки matplotlib строится график, где по оси абцисс откладывается номер ячейки, а ординат - количество элементов в ячейке. По нему можно судить о равномерности распределения хеш-функции. Для лучших алгоритмов будет найдена дисперсия.

<!--- ToDo: Подсветка от Git, вставка картинок через команды md-->

### 1) Const Hash

Функция всегда возвращает одно и то же, заранее определённое значение. Это самая простая функция, время которой, к тому же, не зависит от размера ключа. Но она неэффективна, у неё вряд ли найдётся какое-либо практическое применение.

<img src = "Lab 1/ScreenShots/1. Const.png" height = 80>
<center>
<img src = "Lab 1/Graphics/1. Const.png" height = 300>
</center>

### 2) Len Hash

Функция возвращает длину ключа. Имеет узкий диапазон значений (ключи имеют длину от 2 до 30 символов) и плохое распределение. Очевидно, она не подходит для больших таблиц.

<img src = "Lab 1/ScreenShots/2. Len.png" height = 100>
<center>
<img src = "Lab 1/Graphics/2. Len.png" height = 300>
</center>

### 3) Sum Hash

Функция возвращает сумму байт символов ключа, представленных в кодировке UTF-8 (т.к. ключ - английское слово, то эта сумма совпадает с суммой ASCII-кодов символов). Распределение плохое, т.к. числовые значения букв лежат от 97 до 122, и в значениях, кратных этим, есть очевидные пики. Возможно, при длине таблицы не более 26 этот алгоритм имел бы применение, но тогда сама таблица вряд ли является эффективной структурой для хранения данных.

<img src = "Lab 1/ScreenShots/3. Sum.png" height = 240>
<center>
<img src = "Lab 1/Graphics/3. Sum.png" height = 300>
</center>

### 4) SumLen Hash

Функция возвращает результат 3-го алгоритма, поделённого на результат 2-го. По своей сути является средним значением символа, поэтому имеет такое плохое распределение.

<img src = "Lab 1/ScreenShots/4. SumLen.png" height = 160>
<center>
<img src = "Lab 1/Graphics/4. SumLen.png" height = 300>
</center>

### 5) FirstChar

Возвращает значение первого символа. Своими проблемами очень схожа на предыдущую, хотя и имеет "более ровное" распределение.

<img src = "Lab 1/ScreenShots/5. FirstChar.png" height = 100>
<center>
<img src = "Lab 1/Graphics/5. FirstChar.png" height = 300>
</center>

### 6) ROL

Эта функция производит посимвольный проход: посчитанный для предыдущих букв ключа хеш циклически сдвигается влево, после чего к нему прибавляется значение текущего символа. Функция имеет хорошее распределение, её дисперсия (среднеквадратическое отклонение) D = 41,1.

<img src = "Lab 1/ScreenShots/6. ROL.png" height = 260>
<center>
<img src = "Lab 1/Graphics/6. ROL.png" height = 300>
</center>

### 7) ROR

Алгоритм аналогичен предыдущему, только сдвиг теперь производится вправо. D = 28,6.

<img src = "Lab 1/ScreenShots/7. ROR.png" height = 260>
<center>
<img src = "Lab 1/Graphics/7. ROR.png" height = 300>
</center>

### 8) CRC

Алгоритм работает как "деление в столбик", только деление заменено на побитовый XOR. В нашем случае делимым выступает ключ, а делителем - определённая константа. Более строго это объясняется как деление многочленов друг на друга, но тут мы не будет вдаваться в подробности, т.к. это долго и трудно для понимания. D = 17,1.

<img src = "Lab 1/ScreenShots/8. CRC32.png" height = 380>
<center>
<img src = "Lab 1/Graphics/8. CRC32.png" height = 300>
</center>

### 9) Выводы

Очевидно, что лучше всего показали себя последние 3 функции, теперь сравним их. Но тут придётся немножко заглянуть в будущее. Далее, в рамках второй части работы, они будут переписаны на языке assembly, а размер таблицы увеличен до рекомендуемого. И там результаты измерений получились следующие:

    CRC: D = 1,5  ROL: D = 2,5  ROR: D = 5,7

Таким образом, CRC превосходит циклические алгоритмы, но кто лучше: ROL или ROL - определить сложно. Наше мнение - ROL лучше, так как показал себя лучше в ситуации, более приближенной к реальности. Более того, т.к. при индексации берётся остаток от деления на размер таблицы, некоторое число старших разрядов "отбрасывается", поэтому эффективнее (в теории) должен быть алгоритм, который "чаще изменяет младшие разряды хеша, а не старшие" (извините за слишком вольное объяснение), т.е. ROL.
