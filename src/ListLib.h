#include <stdint.h>
#include <malloc.h>
#include <string.h>

enum Erorrs
{
	ERR_NO,		//Отсутствие ошибок
	ERR_MALLOC,	//Ошибка выделения памяти
	ERR_BADCALL,//Неправильный вызов программы
	ERR_BADDIR
};

//LIFO список строк
typedef struct TextList TextList;
struct TextList
{
	char *text;		//Текстовое поле
	TextList *next;	//Указатель на следующий элемент
};

/*Добавление новой строки text по адресу указателя на список list
 * !!Крайний элемент обязан указывать на NULL!!
Возврат ERR_NO при отсутствии ошибок, либо ERR_MALLOC при нехватке памяти*/
uint8_t PushElement(TextList **list, const char *text);

/*Удаление верхнего элемента по адресу указателя на список list
 * !!Крайний элемент обязан указывать на NULL!! */
void RemoveTopElement(TextList **list);

/*Удаление всего списка list
 * !!Крайний элемент обязан указывать на NULL!! */
void RemoveList(TextList **list);

