// 8 Найти и вывести все файлы имя которых начинается с указанной буквы

#include "ListLib.h"
#include <dirent.h>
#include <malloc.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifdef WIN32
#define SEP "\\"
#else
#define SEP "/"
#endif /* ifdef WIN32 */

int PrintFileLocations(char const *StartDir, char const DesiredChar, TextList **FileList);
int InnerPrintFileLocations(int count, char const *StartDir, char const DesiredChar, TextList **FileList);

uint16_t PrintAndCountList(TextList *list);
uint8_t ScanDirectory(DIR *dir, TextList **DirList, TextList **FileList, char const DesiredChar);
uint8_t FindFiles(char const *StartDir, char const DesiredChar, TextList **FileList);

int main(int argc, const char *argv[])
{
	TextList // Список файлов
		*FileList = NULL;

	// Если было переданно недостаточно аргументов, работу можно сразу прекратить
	if (argc < 3)
	{
		fprintf(stderr, "Укажите, файлы с какого символа и в какой папке вы хотите искать!\n");
		return ERR_BADCALL;
	}

	char DesChar = argv[1][0]; // Искомый символ

	/*Запуск функции поиска, возвращающий число найденых элементов*/
	uint8_t errCode = FindFiles(argv[2], DesChar, &FileList);
	switch (errCode)
	{
	case ERR_BADDIR:
		puts("Ошибка папки");
		RemoveList(FileList);
		return ERR_BADDIR;

	case ERR_MALLOC:
		puts("Ошибка памяти");
		RemoveList(FileList);
		return ERR_MALLOC;

	default:
		break;
	}
	// PrintFileLocations(argv[2], DesChar, &FileList);

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
	if (list) // Вывод осуществляется только если список существует
	{
		do
		{
			printf("%s\n", list->text); // Вывод текущей строки
			count++;
		} while ((list = list->next)); // Переключение на новую строку
		// Если следующей строкой оказался NULL, цикл завершается
	}
	return count;
}

uint8_t ScanDirectory(DIR *curDir, TextList **DirList, TextList **FileList, char const DesiredChar)
{
	//Сохранение названия текущей папки и удаление её из списка
	char *curDirName = (char *) malloc(strlen((*DirList)->text + 1) * sizeof(char));
	if (!curDirName)
		return ERR_MALLOC;

	strcpy(curDirName,(*DirList)->text); 
	RemoveTopElement(DirList);			 

	struct dirent *ep;
	// Пока папка содержит элементы
	while ((ep = readdir(curDir)))
	{
		// Если у элемента название .. или . , то он игнорируется
		if (strcmp(ep->d_name, ".") == 0 || strcmp(ep->d_name, "..") == 0)
			continue;

		// Выделение памяти под путь к очередному файлу, проверка
		char *newDir = (char *)malloc(sizeof(char) * (strlen(curDirName) + strlen(ep->d_name) + 2));
		if (!newDir)
		{
			free(curDirName);
			return ERR_MALLOC;
		}

		// Сохранение полного пути к файлу
		strcpy(newDir, curDirName);
		strcat(newDir, SEP);
		strcat(newDir, ep->d_name);

		// Проверка каждого элемента из папки и, если его первый символ
		// совпал с DesiredChar, при этом элемент не является другой папкой
		// осуществляется сохранение его пути
		if (DT_DIR != ep->d_type && DesiredChar == ep->d_name[0])
		{
			uint8_t err = PushElement(FileList, newDir);
			free(newDir);
			if (err) // Если не удалось выделить память под элемент, прекращение работы
			{
				free(curDirName);
				return ERR_MALLOC;
			}
			continue;
		}

		// Если новый элемент папки сам является таковой, его имя сохраняется в список
		if (DT_DIR == ep->d_type)
		{
			uint8_t err = PushElement(DirList, newDir);
			free(newDir);
			if (err) // Если не удалось выделить память под элемент, прекращение работы
			{
				free(curDirName);
				return ERR_MALLOC;
			}
			continue;
		}
		free(newDir);
	}
	free(curDirName);
	return ERR_NO;
}

/*Функция ищет в файлы, начинающиеся с DesiredChar, в папке с названием StartDir*/
uint8_t FindFiles(char const *StartDir, char const DesiredChar, TextList **FileList)
{
	TextList // Список папок
		*DirList = NULL;
	if (PushElement(&DirList, StartDir))
		return ERR_MALLOC;

	while (DirList->text) // Пока есть элементы в списке папок
	{
		// Открытие очередной папки, проверка
		DIR *curDir = opendir(DirList->text);
		if (!curDir)
		{
			RemoveList(DirList);
			return ERR_BADDIR;
		}
		uint8_t err = ScanDirectory(curDir, &DirList, FileList, DesiredChar);
		closedir(curDir);

		if (err)
		{ // Если не удалось выделить память, возрат ошибки
			RemoveList(DirList);
			return ERR_MALLOC;
		}
	}
	RemoveList(DirList);
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
