# ATmega16Labs

Репозиторий содержит лабораторные работы по программированию микроконтроллера ATmega16, а также три калькулятора, разработанные в рамках подготовки к экзамену.

## Структура репозитория

[LabsDocs](https://github.com/Lectorem794643/ATmega16Labs/tree/main/LaboratoryWork/LabsDocs) — оформленные отчёты по шести лабораторным работам.

[Calculators](https://github.com/Lectorem794643/ATmega16Labs/tree/main/Calculators) — три калькулятора, реализующие ввод двоичных чисел через порт и отображение результата разными способами:

  1. [**LCD Calculator**](https://github.com/Lectorem794643/ATmega16Labs/tree/main/Calculators/LCD%20display) — вывод результата на LCD-дисплей.
  2. [**LED Calculator**](https://github.com/Lectorem794643/ATmega16Labs/tree/main/Calculators/LED%20output) — вывод результата с помощью 8 светодиодов (LED), отображающих число в двоичном формате.
  3. [**7-Segment Calculator**](https://github.com/Lectorem794643/ATmega16Labs/tree/main/Calculators/Seven-segment%20indicator) — вывод результата на группу семисегментных индикаторов.

### Общий принцип работы
Все три калькулятора принимают два 4-битных числа с порта микроконтроллера и выполняют одну из операций: сложение, вычитание, умножение или деление. Выбор операции осуществляется с помощью отдельных входов. 

### LCD Calculator

![LCD](https://github.com/Lectorem794643/ATmega16Labs/blob/main/Calculators/LCD%20display/Proteus.png)

### LED Calculator

![LED](https://github.com/Lectorem794643/ATmega16Labs/blob/main/Calculators/LED%20output/Proteus.png)

### 7-Segment Calculator

![7-Segment](https://github.com/Lectorem794643/ATmega16Labs/blob/main/Calculators/Seven-segment%20indicator/Proteus.png)
