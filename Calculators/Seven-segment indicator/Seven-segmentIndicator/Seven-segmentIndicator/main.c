#define F_CPU 16000000
#include <avr/interrupt.h>

#define PIN_INT1 PD3
#define LED_NEG PD0

unsigned char buttons;

/* Микросхемы для управления индикаторами */
#define ST1RES PORTB&=~(1<<0)
#define ST1SET PORTB|=(1<<0)
#define ST2RES PORTB&=~(1<<1)
#define ST2SET PORTB|=(1<<1)
#define ST3RES PORTB&=~(1<<2)
#define ST3SET PORTB|=(1<<2)
#define SENRES PORTB&=~(1<<4)
#define SENSET PORTB|=(1<<4)

/* Коды символов для 7-сегментных индикаторов */
unsigned char symbol_codes[]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};

void SPI_init()
{
	DDRB|=(7<<0)|(1<<4);  /* Управление линиями выбора микросхем */
	DDRB|=(1<<5)|(1<<7);  /* Управление линиями SPI */
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
}

unsigned char SPI_WR(unsigned char data)
{
	unsigned char tmp;
	SPDR = data;
	while(!(SPSR & (1<<SPIF)))
	tmp=SPDR;
	return tmp;
}

void send_digit(unsigned char value,unsigned char pos)
{
	SPI_WR(symbol_codes[value]); // на все микросхемы отправлен один и тот же код, но записан он будет только в ту микросхему, на которую будет отправлен бит записи
	switch(pos)
	{
		case 1:	ST1SET; ST1RES; break;
		case 2:	ST2SET;	ST2RES;	break;
		case 3:	ST3SET;	ST3RES;	break;
	}
}

unsigned char ASK_btns(void)
{
	unsigned char btns;
	SENRES;
	SENSET;
	btns=SPI_WR(0);
	return btns;
}

void display_number(int number) {
	unsigned char hundreds = (number / 100) % 10; // Сотни
	unsigned char tens = (number / 10) % 10;      // Десятки
	unsigned char units = number % 10;       // Единицы

	send_digit(hundreds, 1); // Вывод на первый индикатор
	send_digit(tens, 2);     // Вывод на второй индикатор
	send_digit(units, 3);    // Вывод на третий индикатор
}

void init_INT1(void) {
	DDRD &= ~(1<<PIN_INT1);// Настроим вывод на вход
	PORTD |= (1<<PIN_INT1);// Включаем подтягивающий резистор
	GICR |= (1<<INT1);// Разрешаем внешнее прерывание на INT1
	MCUCR |= (1<<ISC01)|(0<<ISC00);// Настроим условие прерывания
	sei(); // Включаем глобальные прерывания
}

void configure_neg_indicator() {
	DDRD |= (1 << LED_NEG); // PD0 как выход для индикатора отрицательного числа
	PORTD |= (1 << LED_NEG); // Гасим лампочку по умолчанию (активный уровень — низкий)
}

// Обработчик прерывания для INT1
ISR(INT1_vect) {
	buttons = ASK_btns();
	
	// Разделяем число на два 4-битных числа
	unsigned char num1 = (buttons >> 4) & 0x0F; // Первое число (старшие 4 бита)
	unsigned char num2 = buttons & 0x0F;       // Второе число (младшие 4 бита)
	int result = 0;

	// Выполнение математической операции в зависимости от нажатой кнопки
	if (!(PINA & (1 << PA0))) {
		result = num1 + num2; // Сложение
		} else if (!(PINA & (1 << PA1))) {
		result = num1 - num2; // Вычитание
		} else if (!(PINA & (1 << PA2))) {
		result = num1 * num2; // Умножение
		} else if (!(PINA & (1 << PA3))) {
		if (num2 != 0) {
			result = num1 / num2; // Деление (если делитель не 0)
			} else {
			result = 0; // Предотвращаем деление на 0
		}
	}

	// Если результат отрицательный, включаем лампочку (низкий уровень), иначе гасим
	if (result < 0) {
		PORTD &= ~(1 << LED_NEG); // Включаем лампочку для отрицательных чисел
		} else {
		PORTD |= (1 << LED_NEG); // Гасим лампочку для положительных чисел
	}
	
	// Выводим результат на семисегментные индикаторы
	if (result < 0) {
		result = -result; // Преобразуем отрицательное число в положительное
	}
	display_number(result);
}

// Основная программа
int main(void) {
	SPI_init();
	init_INT1();
	configure_neg_indicator();
	
	DDRA &= ~((1 << PA0) | (1 << PA1) | (1 << PA2) | (1 << PA3)); // Кнопки как входы
	PORTA |= ((1 << PA0) | (1 << PA1) | (1 << PA2) | (1 << PA3)); // Подтяжка для кнопок

	// Инициализация значений перед работой
	buttons = 0;
	display_number(0); // Отображение нуля при старте
	PORTD |= (1 << LED_NEG); // Гасим лампочку при старте (активный уровень — низкий)

	while (1) {	}
	return 0;
}