// 8 Найти и вывести все файлы имя которых начинается с указанной буквы

#include "ListLib.h"	//Включает malloc.h, stdint.h, string.h, список ошибок
#include <dirent.h>
#include <stdio.h>

#ifdef WIN32
#define SEP "\\"	//Разделитель между папок для Windows
#else
#define SEP "/"		//Разделитель между папок для Unix
#endif /* ifdef WIN32 */

uint16_t PrintAndCountList(TextList *list);
uint8_t ScanDirectory(DIR *dir, TextList **DirList, TextList **FileList, char const DesiredChar);
uint8_t FindFiles(char const *StartDir, char const DesiredChar, TextList **FileList);

int main(int argc, const char *argv[])
{
	uint32_t //Счетчик элементов
		count;
	TextList // Список файлов
		*FileList = NULL;

	// Если было переданно недостаточно аргументов, работу можно сразу прекратить
	if (argc < 3)
	{
		puts("Укажите, файлы с какого символа и в какой папке вы хотите искать!");
		return ERR_BADCALL;
	}

	/*Запуск функции поиска, возврат кода ошибки*/
	uint8_t errCode = FindFiles(argv[2], argv[1][0], &FileList);
	switch (errCode)
	{
	case ERR_BADDIR:
		puts("Ошибка папки");
		break;
	case ERR_MALLOC:
		puts("Ошибка памяти");
		break;
	case ERR_NO:
		count = PrintAndCountList(FileList);
		if (count)
			printf("Найдено %d файлов, начинающихся с %c\n", count, argv[1][0]);
		else
			printf("Файлов, начинающихся с %c не найдено!\n", argv[1][0]);
		break;
	}
	RemoveList(FileList);	//Очистка списка файлов
	return errCode;
}

//Функция считает число элементов в списке list, выводит их
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

/*Функция сканирует папку curDir на предмет файлов, начинающихся с DesiredChar, 
 * сохраняет их по указателю pFileList на список файлов, сохраняет новые папки по указателю pDirList на список*/
uint8_t ScanDirectory(DIR *curDir, TextList **pDirList, TextList **pFileList, char const DesiredChar)
{
	//Сохранение названия текущей папки и удаление её из списка
	char *curDirName = (char *) malloc(sizeof(char) * (1 + strlen((*pDirList)->text)));
	if (!curDirName)
		return ERR_MALLOC;
	strcpy(curDirName,(*pDirList)->text); 
	RemoveTopElement(pDirList);			 

	struct dirent *ep;
	// Пока папка содержит элементы
	while ((ep = readdir(curDir)))
	{
		// Если у элемента название .. или . , то он игнорируется
		if (strcmp(ep->d_name, ".") == 0 || strcmp(ep->d_name, "..") == 0)
			continue;

		// Выделение памяти под путь к очередному файлу (путь к текущей папке + имя файла + разделитель и 0 в конце)
		char *newDir = (char *)malloc(sizeof(char) * (strlen(curDirName) + strlen(ep->d_name) + 2));
		if (!newDir)
		{	//Проверка
			free(curDirName);
			return ERR_MALLOC;
		}

		// Сохранение полного пути к файлу
		strcpy(newDir, curDirName);
		strcat(newDir, SEP);
		strcat(newDir, ep->d_name);

		/*	Проверка каждого элемента из папки и, если его первый символ
			совпал с DesiredChar, при этом элемент не является другой папкой
			осуществляется сохранение его пути */
		if (DT_DIR != ep->d_type && DesiredChar == ep->d_name[0])
		{
			uint8_t err = PushElement(pFileList, newDir);
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
			uint8_t err = PushElement(pDirList, newDir);
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

/*Функция ищет файлы, начинающиеся с DesiredChar, 
 * в папке с названием StartDir, сохраняет их по указателю FileList*/
uint8_t FindFiles(char const *StartDir, char const DesiredChar, TextList **pFileList)
{
	TextList // Список папок
		*DirList = NULL;
	if (PushElement(&DirList, StartDir))
		return ERR_MALLOC;

	while (DirList) // Пока есть элементы в списке папок
	{
		// Открытие очередной папки, проверка
		DIR *curDir = opendir(DirList->text);
		if (!curDir)
		{
			RemoveList(DirList);
			return ERR_BADDIR;
		}
		uint8_t err = ScanDirectory(curDir, &DirList, pFileList, DesiredChar);
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
