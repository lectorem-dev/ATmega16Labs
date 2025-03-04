#define F_CPU 16000000
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/io.h>
#include <stdio.h>

#define PIN_INT1 PD3
#define LED_NEG PD0

/* Микросхемы для управления индикаторами */
#define SENRES PORTB&=~(1<<4)
#define SENSET PORTB|=(1<<4)

unsigned char buttons;

void SPI_init()
{
	DDRB|=(7<<0)|(1<<4);  /* Управление линиями выбора микросхем */
	DDRB|=(1<<5)|(1<<7);  /* Управление линиями SPI */
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
}

unsigned char SPI_WR(unsigned char data)
{
	SPDR = data;
	while(!(SPSR & (1<<SPIF))) {} // Ожидаем окончания передачи
	return SPDR;// Читаем данные после завершения передачи
}

unsigned char ASK_btns(void)
{
	SENRES;
	SENSET;
	return SPI_WR(0);
}

void init_INT1(void) {
	DDRD &= ~(1<<PIN_INT1);  // Настроим вывод на вход
	PORTD |= (1<<PIN_INT1);   // Включаем подтягивающий резистор
	GICR |= (1<<INT1);        // Разрешаем внешнее прерывание на INT1
	MCUCR |= (1<<ISC01) | (0<<ISC00);  // Настроим условие прерывания
	sei();  // Включаем глобальные прерывания
}

ISR(INT1_vect) {
	buttons = ASK_btns();
	
	// Разделяем число на два 4-битных числа
	unsigned char num1 = (buttons >> 4) & 0x0F;  // Первое число (старшие 4 бита)
	unsigned char num2 = buttons & 0x0F;         // Второе число (младшие 4 бита)
	int result = 0;
	
	// Выполнение математической операции в зависимости от нажатой кнопки
	if (!(PINA & (1 << PA0))) {
		result = num1 + num2;  // Сложение
		} else if (!(PINA & (1 << PA1))) {
		result = num1 - num2;  // Вычитание
		} else if (!(PINA & (1 << PA2))) {
		result = num1 * num2;  // Умножение
		} else if (!(PINA & (1 << PA3))) {
		if (num2 != 0) {
			result = num1 / num2;  // Деление (если делитель не 0)
			} else {
			result = 0;  // Предотвращаем деление на 0
		}
	}

	// Выводим результат на порт C (в двоичном виде)
	PORTC = ~result;  // Инвертируем сигнал перед выводом на порт C
}


int main(void) {
	SPI_init();
	
	DDRC = 0xFF;  // Настроим порт C как выход
	init_INT1();  // Инициализация INT1
	
	DDRA &= ~((1 << PA0) | (1 << PA1) | (1 << PA2) | (1 << PA3)); // Кнопки как входы
	PORTA |= ((1 << PA0) | (1 << PA1) | (1 << PA2) | (1 << PA3)); // Подтяжка для кнопок

	// Инициализация значений перед работой
	buttons = 0;
	PORTD |= (1 << LED_NEG);  // Гасим лампочку при старте (активный уровень — низкий)

	while (1) {  }
	return 0;
}
