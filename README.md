## Декомпилятор для GameMaker:Studio ##
Данный проект предназначен _только_ для извлечения и анализа кода. Распаковщики остальных ресурсов уже существуют, google it.
Это прототип и пока не полностью в рабочем состоянии.

Вот краткий список вещей, которые _не_ работают:

* Старые версии байт-кода
* Аксессоры: `[| ]`, `[? ]`, `[@ ]`, `[# ]` (формально, конечно, работают, т.к. заменяются на эквивалентные функции)
* В некоторых случаях `break` и `continue` в циклах

### Порядок запуска ###

* Открыть и собрать проект в Code::Blocks.
* Взять исполняемый файл приложения, созданного в GM:S. Извлечь из него data.win (с помощью любого архиватора)
* Запустить декомпилятор, указав путь к data.win и выходной директории через параметры командной строки

### Параметры командной строки ###

* -f <file> - путь к data.win.
* -o <dir> - папка с результатом.
* -t <script1> [<script2> ...] - обработать только перечисленные скрипты.
* -e <script1> [<script2> ...] - не обрабатывать перечисленные скрипты.
* -v - подробные логи.