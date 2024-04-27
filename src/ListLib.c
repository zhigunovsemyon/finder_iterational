#include "ListLib.h"

uint8_t PushElement(TextList **list, const char *text)
{
	//Выделение памяти под новый элемент, проверка, удалось ли
	TextList *new = (TextList *)malloc(sizeof(TextList) + sizeof(char) * (1 + strlen(text)));
	if (!new)
		return ERR_MALLOC;

	new->next = *list;				//Сохранение адреса старого элемента в буфер
	new->text = (char *)(new + 1);	//Присвоение адреса текстового поля
	strcpy(new->text, text);		//Копирование текста в поле
	*list = new;					//Запись нового адреса в список

	return ERR_NO;
}

// void PrintList(TextList *list)
// {
// 	if (list)	//Вывод осуществляется только если список существует
// 	{
// 		do
// 			printf("%s", list->text);	//Вывод текущей строки
// 		while ((list = list->next));	//Переключение на новую строку	
// 		//Если следующей строкой оказался NULL, цикл завершается
// 	}
// 	putchar('\n');
// }

void RemoveTopElement(TextList **list)
{
	TextList *tmp = (*list)->next;	//Сохранение следующего элемента
	free(*list);					//Удаление текущего элемента
	*list = tmp;					//Подстановка следующего элемента на место старого
}

void RemoveList(TextList *list)
{
	while (list)	//Пока указатель на список не равен NULL
	{
		RemoveTopElement(&list);	//Удаление верхнего элемента
		if (list)					//Если он не был последним, то переключение на следующий
			list = list->next;
	}
}
