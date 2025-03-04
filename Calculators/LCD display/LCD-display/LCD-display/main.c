#define F_CPU 16000000
#include <avr/interrupt.h>
#include <stdio.h>

#define PIN_INT1 PD3
#define LED_NEG PD0

unsigned char buttons;

/* Микросхемы для управления индикаторами */
#define SENRES PORTB&=~(1<<4)
#define SENSET PORTB|=(1<<4)

#include <util/delay.h>
#include <avr/io.h>
#define RS 7
#define RW 6
#define EN 5

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

void lcd_com(unsigned char p){
	PORTD &= ~(1<<RS);
	PORTD |= (1<<EN); // EN=1 начало записи команды
	PORTC = p; // Вывод команды на шину данных экрана
	_delay_us(500);
	PORTD &=~(1<<EN); // EN=1 конец записи команды
	_delay_us(500);
}

void lcd_dat(unsigned char p){
	PORTD|=(1<<RS)|(1<<EN); // Начало записи данных
	PORTC=p; // Установка значений
	_delay_us(500);
	PORTD&=~(1<<EN); // Конец записи данных
	_delay_us(500);
}

void lcd_init(void){
	DDRD |= (1<<RS)|(1<<RW)|(1<<EN); // Управление на вывод
	
	// Обнуляем биты управления и шин данных
	PORTD=0x00;
	DDRC=0xFF;
	PORTC=0x00;
	
	_delay_us(500);
	lcd_com(0x08); // Полное включение дисплея
	_delay_us(500);
	lcd_com(0x3C); // 8 бит данных 2 строки
	_delay_us(500);
	lcd_com(0x01); // очистка строки
	_delay_us(500);
	lcd_com(0x06); // сдвиг гурсора вправо
	_delay_us(900);
	lcd_com(0x0F); // курсор показан и мигает
}

void lcd_string(char *str){
	char data=0;
	while(*str){
		data=*str++;
		lcd_dat(data);
	}
}

void init_INT1(void) {
	DDRD &= ~(1<<PIN_INT1);  // Настроим вывод на вход
	PORTD |= (1<<PIN_INT1);   // Включаем подтягивающий резистор
	GICR |= (1<<INT1);        // Разрешаем внешнее прерывание на INT1
	MCUCR |= (1<<ISC01) | (0<<ISC00);  // Настроим условие прерывания
	sei();  // Включаем глобальные прерывания
}

void display_number(int num1, int num2, char operator, int result) {
	char str[20];  // Массив для хранения строки
	// Форматируем строку вида "num1 operator num2 = result"
	sprintf(str, "%d %c %d = %d", num1, operator, num2, result);
	
	lcd_com(0x01); // Очистка экрана
	lcd_com(0x80); // Курсор на начало первой строки
	lcd_string(str);  // Передаем строку в lcd_string
	lcd_com(0xE9); // Убираем курсор
}

ISR(INT1_vect) {
	buttons = ASK_btns();
	
	// Разделяем число на два 4-битных числа
	unsigned char num1 = (buttons >> 4) & 0x0F;  // Первое число (старшие 4 бита)
	unsigned char num2 = buttons & 0x0F;         // Второе число (младшие 4 бита)
	int result = 0;
	char operator;  // Оператор

	// Выполнение математической операции в зависимости от нажатой кнопки
	if (!(PINA & (1 << PA0))) {
		result = num1 + num2;  // Сложение
		operator = '+';  // Устанавливаем оператор
		} else if (!(PINA & (1 << PA1))) {
		result = num1 - num2;  // Вычитание
		operator = '-';
		} else if (!(PINA & (1 << PA2))) {
		result = num1 * num2;  // Умножение
		operator = '*';
		} else if (!(PINA & (1 << PA3))) {
		if (num2 != 0) {
			result = num1 / num2;  // Деление (если делитель не 0)
			} else {
			result = 0;  // Предотвращаем деление на 0
		}
		operator = '/';
	}

	// Выводим результат на ЖК-дисплей
	display_number(num1, num2, operator, result);
}


int main(void) {
	SPI_init();
	lcd_init();
	
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
