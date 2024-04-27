// 8 Найти и вывести все файлы имя которых начинается с указанной буквы

#include <dirent.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "ListLib.h"

#ifdef WIN32
#define SEP "\\"
#else
#define SEP "/"
#endif /* ifdef WIN32 */

int PrintFileLocations(char const *StartDir, char const DesiredChar, TextList **FileList);
int InnerPrintFileLocations(int count, char const *StartDir, char const DesiredChar, TextList **FileList);
uint16_t PrintAndCountList(TextList *list);


// int main(void)
// {
// 	TextList *kek = NULL;
// 	char buff[64];
// 	do
// 	{
// 		fgets(buff, 64, stdin);
// 		if (PushElement(&kek, buff))
// 		{
// 			RemoveList(kek);
// 			return ERR_MALLOC;
// 		}
//
// 	} while (buff[0] != '\n');
//
// 	PrintList(kek);
// 	RemoveList(kek);
// 	return ERR_NO;
// }

int main(int argc, const char *argv[])
{
	TextList //Список файлов
	*FileList = NULL;
	
	// Если было переданно недостаточно аргументов, работу можно сразу прекратить
	if (argc < 3)
	{
		fprintf(stderr, "Укажите, файлы с какого символа и в какой папке вы хотите искать!\n");
		return ERR_BADCALL;
	}

	char DesChar = argv[1][0]; // Искомый символ

	/*Запуск функции поиска, возвращающий число найденых элементов*/
	PrintFileLocations(argv[2], DesChar, &FileList);
	int count = PrintAndCountList(FileList);

	if (count)
	{
		printf("Найдено %d файлов, начинающихся с %c\n", count, DesChar);
	}
	else
		printf("Файлов, начинающихся с %c не найдено!\n", DesChar);

	RemoveList(FileList);
	return ERR_NO;
}

uint16_t PrintAndCountList(TextList *list)
{
	uint16_t count = 0;
	if (list)	//Вывод осуществляется только если список существует
	{
		do
		{
			printf("%s\n", list->text);	//Вывод текущей строки
			count++;
		}
		while ((list = list->next));	//Переключение на новую строку	
		//Если следующей строкой оказался NULL, цикл завершается
	}
	return count;
}

/*Функция ищет в файлы, начинающиеся с DesiredChar, в папке с названием StartDir*/
uint8_t FindFiles(char const *StartDir, char const DesiredChar, TextList **FileList)
{
	TextList //Список папок
	*DirList = NULL;
	if(PushElement(&DirList, StartDir))
		return ERR_MALLOC;

	while(DirList->text)	//Пока есть элементы в списке папок
	{
		// DIR *curDir = opendir(const char *name);
			
	}
	return ERR_NO;
}

/*Функция ищет в файлы, начинающиеся с DesiredChar, в папке с названием StartDir*/
int PrintFileLocations(char const *StartDir, char const DesiredChar, TextList **FileList)
{
	// Запуск внутренней функции со счётчиком
	return InnerPrintFileLocations(0, StartDir, DesiredChar, FileList);
}

int InnerPrintFileLocations(int count, char const *StartDir, char const DesiredChar, TextList **FileList)
{
	DIR *dp = opendir(StartDir);
	// Граничное условие: если StartDir это не папка,
	// то нет в ней файлов на DesiredChar
	if (!dp)
		return count;

	struct dirent *ep;
	// Цикл перебирает элементы данной папки
	while ((ep = readdir(dp)))
	{
		// Если у элемента название .. или . , то он игнорируется
		if (strcmp(ep->d_name, ".") == 0 || strcmp(ep->d_name, "..") == 0)
			continue;

		// Выделение памяти под путь к очередному файлу, сохранение его имени
		char *newDir = (char *)malloc(sizeof(char) * (strlen(StartDir) + strlen(ep->d_name) + 2));
		strcpy(newDir, StartDir);
		strcat(newDir, SEP);
		strcat(newDir, ep->d_name);

		// Проверка каждого элемента из папки и, если его первый символ
		// совпал с DesiredChar, при этом элемент не является другой папкой
		// - сохранение его пути и увеличение счётчика
		if (DT_DIR != ep->d_type && DesiredChar == ep->d_name[0])
		{
			PushElement(FileList, newDir);
			count++;
			continue;
		}

		// Открытие очередного файла
		count = InnerPrintFileLocations(count, newDir, DesiredChar, FileList);
		free(newDir); // Освобождение памяти с названием файла
	}
	closedir(dp); // закрытие папки
	return count; // Возврат числа элементов
}
