#include <stdint.h>
#include <malloc.h>
#include <string.h>

enum Erorrs
{
	ERR_NO,		//Отсутствие ошибок
	ERR_MALLOC,	//Ошибка выделения памяти
	ERR_BADCALL	//Неправильный вызов программы
};

//LIFO список строк
typedef struct TextList TextList;
struct TextList
{
	char *text;		//Текстовое поле
	TextList *next;	//Указатель на следующий элемент
};

/*Добавление новой строки text по адресу указателя на список list
Возврат ERR_NO при отсутствии ошибок, либо ERR_MALLOC при нехватке памяти*/
uint8_t PushElement(TextList **list, const char *text);

//Вывод всего списка list без разделителей между элементами
// void PrintList(TextList *list);

//Удаление верхнего элемента по адресу указателя на список list
void RemoveTopElement(TextList **list);

//Удаление всего списка list
void RemoveList(TextList *list);

