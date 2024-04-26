// 8 Найти и вывести все файлы имя которых начинается с указанной буквы

#include <dirent.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>

#ifdef WIN32
#define SEP "\\"
#else
#define SEP "/"
#endif /* ifdef WIN32 */

enum Errors
{
	NOERR,
	BADCALL
};

int PrintFileLocations(char const *StartDir, char const DesiredChar);
int InnerPrintFileLocations(int count, char const *StartDir, char const DesiredChar);

int main(int argc, const char *argv[])
{
	// Если было переданно недостаточно аргументов, работу можно сразу прекратить
	if (argc < 3)
	{
		fprintf(stderr, "Укажите, файлы с какого символа и в какой папке вы хотите искать!\n");
		return BADCALL;
	}

	char DesChar = argv[1][0]; // Искомый символ

	/*Запуск функции поиска, возвращающий число найденых элементов*/
	int count = PrintFileLocations(argv[2], DesChar);

	if (count)
		printf("Найдено %d файлов, начинающихся с %c\n", count, DesChar);
	else
		printf("Файлов, начинающихся с %c не найдено!\n", DesChar);

	return NOERR;
}

/*Функция ищет в файлы, начинающиеся с DesiredChar, в папке с названием StartDir*/
int PrintFileLocations(char const *StartDir, char const DesiredChar)
{
	// Запуск внутренней функции со счётчиком
	return InnerPrintFileLocations(0, StartDir, DesiredChar);
}

int InnerPrintFileLocations(int count, char const *StartDir, char const DesiredChar)
{
	DIR *dp = opendir(StartDir);
	// Граничное условие: если StartDir это не папка,
	// то нет в ней файлов на DesiredChar
	if (!dp)
		return count;

	struct dirent *ep;
	// Цикл перебирает элементы данной папки
	while (ep = readdir(dp))
	{
		// Если у элемента название .. или . , то он игнорируется
		if (strcmp(ep->d_name, ".") == 0 || strcmp(ep->d_name, "..") == 0)
			continue;

		// Проверка каждого элемента из папки и, если его первый символ
		// совпал с DesiredChar, при этом элемент не является другой папкой
		// - вывод его пути и увеличение счётчика
		if (DT_DIR != ep->d_type && DesiredChar == ep->d_name[0])
		{
			printf("%s%s%s\n", StartDir, SEP, ep->d_name);
			count++;
			continue;
		}

		// Выделение памяти под путь к очередному файлу, сохранение его имени
		char *newDir = (char *)malloc(sizeof(char) * (strlen(StartDir) + strlen(ep->d_name) + 2));
		strcpy(newDir, StartDir);
		strcat(newDir, SEP);
		strcat(newDir, ep->d_name);

		// Открытие очередного файла
		count = InnerPrintFileLocations(count, newDir, DesiredChar);
		free(newDir); // Освобождение памяти с названием файла
	}
	closedir(dp); // закрытие папки
	return count; // Возврат числа элементов
}
