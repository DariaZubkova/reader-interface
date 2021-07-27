
#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <windows.h>
#include <math.h>
//#include <comdlg32.dll>


#include "mymenu.h"

#define MAX_SIZE 32767

/*структура для работы с текстом
 *str - весь текст
 *strStart - смещение(начала строк)
 *maxStrLen - длина самой длинной строки
 *strCount - количество строк
 */
typedef struct text_t
{
	char* str;
	int* strStart;
	int maxStrLen;
	int strCount;
}text_t;

/*структура метрики
 *х - координата по х
 *у - координата по х
 */
typedef struct metric_t
{
  int x;
  int y;
}metric_t;

/*структура длины символа
 *xChar - усредненное значение ширины символа
 *xCaps - средняя ширина символов верхнего регистра
 *yChar - полное значение высоты символа
 */
typedef struct symSize_t
{
    int xChar;
    int xCaps;
    int yChar;
} symSize_t;

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("CodeBlocksWindowsApp");

TCHAR szMenuName[ ] = _T("Menu");

int WINAPI WinMain (HINSTANCE hThisInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpszArgument,
                     int nCmdShow)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;               /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);

    wincl.lpszMenuName = szMenuName;                 /*Menu */

    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH); /*белый цвет экрана*/

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           _T("Code::Blocks Template Windows App"),       /* Title Text */
           WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_HSCROLL, /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           544,                 /* The programs width */
           375,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);
    UpdateWindow(hwnd);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}


/*Функция ищет длину самой длинной строки
 *IN::numLine - количество строк
 *IN::str - весь тексте
 *RETURN::int maxStrLen- размер самой длинной строки
 */
int FindMaxStrLen(int numLine, char* str)
{
    int i = 0, maxStrLen = 0, strLen = 0;

    while(str[i] != '\0')
    {
        strLen = 0;
        while(str[i] != '\0' && str[i] != '\n')
        {
            strLen++;
            i++;
        }
        if(strLen > maxStrLen)
            maxStrLen = strLen;
        i++;
    }

    return maxStrLen;
}

/*Функция освобождает память
 *IN::text - указатель на структуру хранения текста
 */
void MyFree(text_t* text)
{
    if (text->strStart != NULL)
        free(text->strStart);
    if (text->str != NULL)
        free(text->str);

    text->str = NULL;
    text->strStart = NULL;
}

/*Функция для создания диалогового окна
 *IN::ofn - указатель на структуру диалогового окна
 *IN::hwnd - дескриптор окна
 *IN::pstrFileName - путь к файлу
 *IN::pstrTitleName - имя файла
 *RETURN::BOOL - создалось диалоговое окно или нет
 */
BOOL FileOpenDlg(OPENFILENAME* ofn, HWND hwnd, PSTR pstrFileName, PSTR pstrTitleName)
{
    ofn->hwndOwner         = hwnd;
    ofn->lpstrFile         = pstrFileName;
    ofn->lpstrFileTitle    = pstrTitleName;
    ofn->Flags             = OFN_HIDEREADONLY;

    return GetOpenFileName(ofn);
}

/*Функция для инициализации структуры диалогового окна
 *IN::hwnd - дескриптор окна
 *IN::ofn - указатель на структуру диалогового окна
 */
void FileInit(HWND hwnd, OPENFILENAME* ofn)
{
    static char szFilter[] = "Text Files(*.TXT)\0*.txt\0\0";
    ofn->lStructSize       = sizeof(OPENFILENAME);
    ofn->hwndOwner         = hwnd;
    ofn->lpstrFilter       = szFilter;
    ofn->lpstrDefExt       = "txt";
    ofn->nMaxFile          = _MAX_PATH;
    ofn->nMaxFileTitle     = _MAX_FNAME + _MAX_EXT;
    ofn->nMaxCustFilter    = 0;
    ofn->nFilterIndex      = 0;
    ofn->Flags             = 0;
    ofn->nFileOffset       = 0;
    ofn->nFileExtension    = 0;
    ofn->lCustData         = 0L;
    ofn->lpfnHook          = NULL;
    ofn->lpTemplateName    = NULL;
    ofn->lpstrInitialDir   = NULL;
    ofn->lpstrTitle        = NULL;
    ofn->lpstrFileTitle    = NULL;
    ofn->lpstrFile         = NULL;
    ofn->lpstrCustomFilter = NULL;
    ofn->hInstance         = NULL;
}

/*Функция для считывания из файла
 *IN::text - указатель на структуру текста
 *IN::file - указатель на имя файла
 *IN::sym - структура размеров символа
 *IN::iMaxWidth - указатель на максимальное количество символов в строке
 *RETURN:: 0 - прошло всё без ошибок, -1 - с ошибками(не выделилась память и т.д.)
 */
int ReadText(text_t* text, char* file, symSize_t sym, int* iMaxWidth)
{
    FILE* fin;
    fin = fopen(file, "r");
    if (fin != NULL){
            int sizeText = 0;
            int strCount = 0;
            int i;
            fseek(fin, 0, SEEK_END);
            sizeText = ftell(fin);
            fseek(fin, 0, SEEK_SET);
			if (sizeText > 0) {
				text->str = (char*)calloc(sizeof(char), sizeText + 1);
				text->strStart = (int*)calloc(sizeof(int), sizeText);
			}
			else {
				fclose(fin);
				return 0;
			}
            if (text->str != NULL && text->strStart != NULL) {
                fread(text->str, sizeof(char), sizeText, fin);
                text->str[sizeText] = '\0';
                text->strStart[strCount] = 0;

                for (i = 0; i < sizeText; i++)
                {
                    if (text->str[i] == '\n')
                    {
                        strCount++;
                        text->strStart[strCount] = i + 1;
                        sizeText--;
                    }
                }
                text->strStart[strCount + 1] = i;
                text->strCount = strCount + 1;
                text->maxStrLen = FindMaxStrLen(text->strCount, text->str);

            }
            else {
                    MyFree(text);
                    fclose(fin);
                    printf("Error: don't have memory\n");
                    return -1;
            }
            fclose(fin);
            if(text->str != NULL)
            {
                (*iMaxWidth) = text->maxStrLen * sym.xCaps;
            }
    }
    else {
        if (file != NULL) {
            printf("Error: don't open file\n");
            return -1;
        }
        else
            return 0;
    }
    return 0;
}

/*Функция для создания вертикального скролла в режиме с вёрсткой
 *IN::scroll - указатель на структуру скролла
 *IN::text - структура хранения текста
 *IN::sym - структура размеров символа
 *IN::client - метрика рабочей области
 *IN::rect - структура прямоугольника
 *IN::width - указатель на длину рабочей области
 *IN::yOneList - указатель на высоту рабочей обалсти
 *IN::yPos - позиция координаты у
 *IN::change - указатель на коэффициент
 *IN::newStrCount - указатель на количество новых строк
 */
void VertScrollWith(SCROLLINFO* scroll, text_t text, symSize_t sym, metric_t client, RECT rect, int* width, int* yOneList, double yPos, double* change, int* newStrCount)
{
    (*width) = (rect.right - rect.left) / sym.xCaps;
    (*yOneList) = (rect.bottom - rect.top) / sym.yChar;
    scroll->cbSize = sizeof((*scroll));
    scroll->fMask = SIF_ALL;
    scroll->nMin = 0;
    scroll->nPage = (rect.bottom - rect.top) / sym.yChar;

    int i = text.strCount;
    int paintBegin = 0;
    int paintEnd =i; //сколько строк поместиться в рабочую область
    int yStr = 0;
    int extraCount = 0;
    int textLen = 0;

	if (text.str != NULL) {
		for (i = paintBegin; i < paintEnd; i++)
		{
			textLen = text.strStart[yStr + 1] - text.strStart[yStr]; //длина(размер) данной строки

			if (i < text.strCount + extraCount)
			{
				int k = 0;
				if (textLen > (*width)) {
					while (textLen > (*width)) {
						k++;
						i++;
						textLen = text.strStart[yStr + 1] - text.strStart[yStr] - k * (*width);
						paintEnd++;
						extraCount++;
					}
				}
				yStr++;
			}
		}
		(*newStrCount) = paintEnd;
	}

	if ((*yOneList) >= (*newStrCount)) {
		scroll->cbSize = sizeof(scroll);
		scroll->fMask = SIF_ALL;
		scroll->nMin = 0;
		scroll->nMax = 0;
		scroll->nPage = 0;
		scroll->nPos = 0;
	}
	else {

		if ((*newStrCount) <= MAX_SIZE)
		{
			//scroll->nMax = abs(text.strCount - client.y / sym.yChar);
			scroll->nMax = text.strCount - 1;
			scroll->nPos = (int)(yPos);
			(*change) = 1;
		}
		else
		{
			scroll->nMax = MAX_SIZE;
			(*change) = (double)(fabs((double)(text.strCount - 1)) / (double)MAX_SIZE);
			scroll->nPos = (int)(yPos / (*change));
			scroll->nPage = (int)((rect.bottom - rect.top) / sym.yChar) / (*change);
		}
	}
}

/*Функция для создания вертикального скролла в режиме без вёрстки
 *IN::scroll - указатель на структуру скролла
 *IN::text - структура хранения текста
 *IN::sym - структура размеров символа
 *IN::client - метрика рабочей области
 *IN::rect - структура прямоугольника
 *IN::yOneList - указатель на высоту рабочего окна
 *IN::xOneList - указатель на ширину рабочего окна
 *IN::yPos - позиция координаты у
 *IN::change - указатель на коэффициент
 */
void VertScrollWithout(SCROLLINFO* scroll, text_t text, symSize_t sym, metric_t client, RECT rect, int* yOneList, int* xOneList, double yPos, double* change)
{
    (*yOneList) = (rect.bottom - rect.top) / sym.yChar;
    (*xOneList) = (rect.right - rect.left) / sym.xChar;
    if ((*yOneList) >= text.strCount) {
        scroll->cbSize = sizeof(scroll);
        scroll->fMask = SIF_ALL;
        scroll->nMin = 0;
        scroll->nMax = 0;
        scroll->nPage = 0;
        scroll->nPos = 0;
    }
    else {
        scroll->cbSize = sizeof((*scroll));
        scroll->fMask = SIF_ALL;
        scroll->nMin = 0;
        scroll->nPage = (rect.bottom - rect.top) / sym.yChar;

        if (text.strCount <= MAX_SIZE)
        {
            scroll->nMax = text.strCount - client.y / sym.yChar;
            scroll->nPos = (int)(yPos);
            (*change) = 1;
        }
        else
        {
            scroll->nMax = MAX_SIZE;
            (*change) = (double)((double)(text.strCount - 1) / (double)MAX_SIZE);
            scroll->nPos = (int)(yPos/(*change));
            scroll->nPage = (int)((rect.bottom - rect.top) / sym.yChar) / (*change);
            //yOneList = scroll.nPage;
        }
    }
}

/*Функция для создания горизонтального скролла в режиме с вёрсткой
 *IN::scroll - указатель на структуру скролла
 */
void HertScrollWith(SCROLLINFO* scroll)
{
    scroll->cbSize = sizeof(scroll);
    scroll->fMask = SIF_ALL;
    scroll->nMin = 0;
    scroll->nMax = 0;
    scroll->nPage = 0;
    scroll->nPos = 0;
}

/*Функция для создания вертикального скролла в режиме без вёрстки
 *IN::scroll - указатель на структуру скролла
 *IN::iMaxWidth - максимальное количество символов
 *IN::client - метрика рабочей области
 *IN::sym - структура размеров символа
 *IN::xPos - позиция координаты х
 *IN::xOneList - указатель на ширину рабочего окна
 *IN::maxStrLen - максимальная длина строки
 */
void HertScrollWithout(SCROLLINFO* scroll, int iMaxWidth, metric_t client, symSize_t sym, int xPos, int xOneList, int maxStrLen)
{
    if (xOneList >= maxStrLen) {
        scroll->cbSize = sizeof(scroll);
        scroll->fMask = SIF_ALL;
        scroll->nMin = 0;
        scroll->nMax = 0;
        scroll->nPage = 0;
        scroll->nPos = 0;
    }
    else {
        scroll->nMax = 2 + (iMaxWidth - client.x) / sym.xChar;
        if (scroll->nMax < 0) {
            scroll->nMax = 0;
        }
        scroll->nPage = client.x / sym.xChar;
        scroll->nPos = xPos;
    }
}

/*Функция для обновления начало и конца строчек вывода
 *IN::paintBegin - указатель на начальную строку
 *IN::paintEnd - указатель на конечную строку
 *IN::strCount - количество строк
 *IN::client - метрика рабочей области
 *IN::sym - структура размеров символа
 */
void PaintBeginEndInit(int* paintBegin, int* paintEnd, int strCount, metric_t client, symSize_t sym)
{
    (*paintBegin) = 0;
    (*paintEnd) = min(strCount, client.y / sym.yChar );
}

/*Функция для обновления координат для вывода текста
 *IN::paint - указатель на структуру координат при рисовке
 *IN::sym - структура размеров символа
 *IN::str - строка
 */
void UpdatePaint(metric_t* paint, symSize_t sym, int str)
{
    (*paint).x = sym.xChar;
    (*paint).y = sym.yChar * str;
}

/*Функция для отображения текста в режиме с вёрсткой
 *IN::hdc - описатель контекста устройства
 *IN::text - структура хранения текста
 *IN::client - метрика рабочей области
 *IN::sym - структура размеров символа
 *IN::newStrCount - количество строк
 *IN::yPos - позиция координаты у
 *IN::width - длина рабочей области
 *IN::yPrev - предыдущая позиция координаты у
 */
void PaintTextWith(HDC hdc, text_t text, metric_t client, symSize_t sym, int newStrCount, double yPos, int width, double yPrev)
{
    char* printNow = NULL;   //строчка, которая будет выводиться
	int textLen = 0; //длина строки при отрисовке
    metric_t paint;  //размер области, где выводить
    int i, paintBegin, paintEnd;
    i = newStrCount;
    PaintBeginEndInit(&paintBegin, &paintEnd, i, client, sym);
    int yStr = 0;
    for(i = paintBegin; i < paintEnd; i++)
    {
        UpdatePaint(&paint, sym, i);

        printNow = (char*)(text.str + sizeof(char)*(text.strStart[(int)yPos + yStr]));
        textLen = text.strStart[(int)yPos + yStr + 1] - text.strStart[(int)yPos + yStr];
        if(i < newStrCount)
        {
            if (textLen <= width) {
                    TextOut(hdc, paint.x, paint.y, printNow, textLen);
                }
                else {
                        int k = 0;
                    while(textLen > width) {
                        k++;
                        TextOut(hdc, paint.x, paint.y, printNow, width);
                        i++;
                        UpdatePaint(&paint, sym, i);
                        printNow = (char*)(text.str + sizeof(char)*(text.strStart[(int)yPos + yStr]) + sizeof(char) * k * width);
                        textLen = text.strStart[(int)yPos + yStr + 1] - text.strStart[(int)yPos + yStr] - k * width;
                    }
                    if (textLen <= width) {
                        TextOut(hdc, paint.x, paint.y, printNow, textLen);
                    }
                }
            yStr++;
        }
    }
}

/*Функция для отображения текста в режиме без вёрстки
 *IN::hdc - описатель контекста устройства
 *IN::text - структура хранения текста
 *IN::client - метрика рабочей области
 *IN::sym - структура размеров символа
 *IN::yPos - позиция координаты у
 *IN::xPos - позиция координаты х
 */
void PaintTextWithout(HDC hdc, text_t text, metric_t client, symSize_t sym, double yPos, int xPos)
{
    char* printNow = NULL;   //строчка, которая будет выводиться
	int textLen = 0; //длина строки при отрисовке
    metric_t paint;  //размер области, где выводить
    int i, paintBegin, paintEnd;
    i = text.strCount;
    PaintBeginEndInit(&paintBegin, &paintEnd, i, client, sym);
    for(i = paintBegin; i < paintEnd; i++)
    {
        UpdatePaint(&paint, sym, i);

        printNow = (char*)(text.str + sizeof(char)*(text.strStart[i + (int)(yPos)]) + sizeof(char) * xPos);
        textLen = text.strStart[i + 1 + (int)(yPos)] - text.strStart[i + (int)(yPos)] - xPos;

        if(i < text.strCount)
        {
            TextOut(hdc, paint.x, paint.y, printNow, textLen);
        }
    }
}
/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT ps;
    TEXTMETRIC tm;
    RECT rect;
    SCROLLINFO scroll;

    static OPENFILENAME ofn;
    static char szFileName[_MAX_PATH], szTitleName[_MAX_FNAME + _MAX_EXT];
    static HMENU hMenu;

    static text_t text;
    static symSize_t sym; //размеры символа
    static metric_t client; //размеры рабочего окна
    static int xPos = 0;    //сдвиг по х
    static double yPos = 0, prevPos = 0;    //сдвиг по у, предыдущий двиг по у

	static int iMaxWidth;   //максимальное количество символов
	static int yOneList = 0, xOneList = 0; //размер одного листа для х и у
	static int iChoice = ID_MODE_NO_FILTER; //выбор режима
	static double change = 1;   //коэффициент

	static int width = 0; //ширина окна для вёрстки
    static int newStrCount = 0; //количество строк для вёрстки

    switch (message)                  /* handle the messages */
    {
        case WM_CREATE: {
        printf("WM_CREATE\n");

        hdc = GetDC(hwnd);
        SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));
        GetTextMetrics(hdc, &tm);
        sym.xChar = tm.tmAveCharWidth;
        sym.xCaps =(tm.tmPitchAndFamily & 1 ? 3 : 2) * sym.xChar / 2;
        sym.yChar = tm.tmHeight + tm.tmExternalLeading;
        ReleaseDC(hwnd, hdc);

        CREATESTRUCT* p = (CREATESTRUCT*) lParam;
        FileInit(hwnd, &ofn);
        char* file = (char*)p->lpCreateParams;
        int check = 0;
        check = ReadText(&text, file, sym, &iMaxWidth);
         if (check != -1) {
            hMenu = GetMenu(hwnd);
            CheckMenuItem(hMenu, iChoice, MF_CHECKED);
        }
        return 0;
    }
        case WM_PAINT: {
            printf("WM_PAINT\n");
            hdc = BeginPaint(hwnd, &ps);

            if(text.str != NULL)
            {
                if (iChoice == ID_MODE_FILTER)
                    PaintTextWith(hdc, text, client, sym, newStrCount, yPos, width, prevPos);
                else
                    PaintTextWithout(hdc, text, client, sym, yPos, xPos);
            }
	    	EndPaint(hwnd, &ps);
	    	return 0;
        }
        case WM_SIZE : {
            printf("WM_SIZE\n");
            client.x = LOWORD(lParam);
            client.y = HIWORD(lParam);

            GetClientRect(hwnd, &rect);
            GetScrollInfo(hwnd, SB_VERT, &scroll);

            /*вертикальный скролл*/
		    if (iChoice == ID_MODE_FILTER)
               VertScrollWith(&scroll, text, sym, client, rect, &width, &yOneList, yPos, &change, &newStrCount);
            else
                VertScrollWithout(&scroll, text, sym, client, rect, &yOneList, &xOneList, yPos, &change);
            SetScrollRange(hwnd, SB_VERT, 0, scroll.nMax, FALSE);
            SetScrollPos(hwnd, SB_VERT, scroll.nPos, TRUE);

            /*горизонтальный скролл*/
            if (iChoice == ID_MODE_FILTER)
                HertScrollWith(&scroll);
            else
                HertScrollWithout(&scroll, iMaxWidth, client, sym, xPos, xOneList, text.maxStrLen);
            SetScrollRange(hwnd, SB_HORZ, 0, scroll.nMax, FALSE);
            SetScrollPos(hwnd, SB_HORZ, scroll.nPos, TRUE);

		    InvalidateRect(hwnd, NULL, TRUE);
		    UpdateWindow(hwnd);

            return 0;
        }


        case WM_HSCROLL:
	    {
	        printf("WM_HSCROLL\n");

		    scroll.cbSize = sizeof(scroll);
		    scroll.fMask = SIF_ALL;

		    GetScrollInfo(hwnd, SB_HORZ, &scroll);
		    xPos = scroll.nPos;

		    switch (LOWORD(wParam))
		    {
		        case SB_LINELEFT:
		        	scroll.nPos -= 1;
		        	break;
		        case SB_LINERIGHT:
		        	scroll.nPos += 1;
		        	break;
	    	    case SB_PAGELEFT:
		     	    scroll.nPos -= xOneList;
		     	    break;
	    	    case SB_PAGERIGHT:
		    	     scroll.nPos += xOneList;
		      	     break;
	    	    case SB_THUMBTRACK:
	    	    	scroll.nPos = scroll.nTrackPos;
	     	    	break;
	    	    }

	    	    if(scroll.nPos <= 0)
                    scroll.nPos = 0;
                else if(scroll.nPos >= scroll.nMax*change)
                scroll.nPos = scroll.nMax * change;

		        scroll.fMask = SIF_POS;
		        SetScrollInfo(hwnd, SB_HORZ, &scroll, TRUE);

	    	    if (scroll.nPos != xPos)
	    	    {
		        	xPos = scroll.nPos;
		        	InvalidateRect(hwnd, NULL, TRUE);
		        	UpdateWindow(hwnd);
	    	    }
		    return 0;
	    }
	case WM_VSCROLL:
	    {
	        printf("WM_VSCROLL\n");

		    scroll.cbSize = sizeof(scroll);
		    scroll.fMask = SIF_ALL;

		    GetScrollInfo(hwnd, SB_VERT, &scroll);
            prevPos = yPos;

		    switch (LOWORD(wParam))
            {
		        case SB_TOP:
			        yPos = 0;
			        scroll.nPos = scroll.nMin;
			        break;
		        case SB_BOTTOM:
		    	    scroll.nPos = scroll.nMax;
		    	    yPos = scroll.nMax * change;
                    break;
		        case SB_LINEUP:
			        yPos -= 1;
			        scroll.nPos = (int)(yPos / change);
			        break;
	    	    case SB_LINEDOWN:
		    	    yPos += 1;
		    	    scroll.nPos = (int)(yPos / change);
		    	    break;
		        case SB_PAGEUP:
			        yPos -= yOneList;
			        scroll.nPos = (int)(yPos / change);
			        break;
		        case SB_PAGEDOWN:
		        	yPos += yOneList;
		        	scroll.nPos = (int)(yPos / change);
	    	    	break;
	    	    case SB_THUMBTRACK:
		    	    scroll.nPos = (int)(scroll.nTrackPos);
		    	    yPos = scroll.nPos * change;
		    	    break;
            }

            if(yPos <= 0)
                yPos = 0;
            else if(yPos >= scroll.nMax*change)
                yPos = scroll.nMax * change;

            scroll.fMask = SIF_POS;
		    SetScrollInfo(hwnd, SB_VERT, &scroll, TRUE);

		    if (yPos != prevPos)
            {
			    InvalidateRect(hwnd, NULL, TRUE);
			    UpdateWindow(hwnd);
            }
		    return 0;
	    }
	    case WM_KEYDOWN:
	    {
	        printf("WM_KEYDOWN\n");
	        switch (wParam)
	        {
		        case VK_UP:
			        SendMessage(hwnd, WM_VSCROLL, SB_LINEUP, 0);
                    break;
		        case VK_DOWN:
		        	SendMessage(hwnd, WM_VSCROLL, SB_LINEDOWN, 0);
			        break;
		        case VK_LEFT:
		        	SendMessage(hwnd, WM_HSCROLL, SB_LINEUP, 0);
		        	break;
		        case VK_RIGHT:
			        SendMessage(hwnd, WM_HSCROLL, SB_LINEDOWN, 0);
			        break;
                case VK_PRIOR:
			        SendMessage(hwnd, WM_VSCROLL, SB_PAGEUP, 0);
			        break;
		        case VK_NEXT:
		        	SendMessage(hwnd, WM_VSCROLL, SB_PAGEDOWN, 0);
			        break;
                case VK_HOME:
                    SendMessage(hwnd, WM_VSCROLL, SB_TOP, 0);
                    break;
                case VK_END:
                    SendMessage(hwnd, WM_VSCROLL, SB_BOTTOM, 0);
                    break;
	    	}
		    return 0;
	    }

	    case WM_COMMAND:
        {
            printf("WM_COMMAND\n");
		    hMenu = GetMenu(hwnd);

		    switch (LOWORD(wParam))
            {
		    case ID_OPEN:
		        printf("WM_OPEN\n");
                if(FileOpenDlg(&ofn, hwnd, szFileName, szTitleName))
                {
			        MyFree(&text);
			        CheckMenuItem(hMenu, iChoice, MF_CHECKED);
			        printf("!!\n");

			        int check = 0;
                    check = ReadText(&text, szFileName, sym, &iMaxWidth);
                    if (check != 0)
                        break;

			        xPos = 0;
			        yPos = 0;
			        iMaxWidth = text.maxStrLen * sym.xCaps;
			        GetClientRect(hwnd, &rect);
                    GetScrollInfo(hwnd, SB_VERT, &scroll);

                    /*вертикальный скролл*/
                    if (iChoice == ID_MODE_FILTER)
                        VertScrollWith(&scroll, text, sym, client, rect, &width, &yOneList, yPos, &change, &newStrCount);
                    else
                        VertScrollWithout(&scroll, text, sym, client, rect, &yOneList, &xOneList, yPos, &change);
                    SetScrollRange(hwnd, SB_VERT, 0, scroll.nMax, FALSE);
                    SetScrollPos(hwnd, SB_VERT, scroll.nPos, TRUE);

                    /*горизонтальный скролл*/
                    if (iChoice == ID_MODE_FILTER)
                        HertScrollWith(&scroll);
                    else
                        HertScrollWithout(&scroll, iMaxWidth, client, sym, xPos, xOneList, text.maxStrLen);
                    SetScrollRange(hwnd, SB_HORZ, 0, scroll.nMax, FALSE);
                    SetScrollPos(hwnd, SB_HORZ, scroll.nPos, TRUE);
                }

			    InvalidateRect(hwnd, NULL, TRUE);
			    UpdateWindow(hwnd);
			    break;

		    case ID_CLOSE:
		        printf("ID_CLOSE\n");
			    DestroyWindow(hwnd);
			    break;

		    case ID_MODE_NO_FILTER:
		        printf("ID_MODE_NO_FILTER\n");
		        /*галочка меню*/
		        CheckMenuItem(hMenu, iChoice, MF_UNCHECKED);
			    iChoice = LOWORD(wParam);
			    CheckMenuItem(hMenu, iChoice, MF_CHECKED);

			    GetClientRect(hwnd, &rect);
                GetScrollInfo(hwnd, SB_VERT, &scroll);

                /*вертикальный скролл*/
                VertScrollWithout(&scroll, text, sym, client, rect, &yOneList, &xOneList, yPos, &change);
                SetScrollRange(hwnd, SB_VERT, 0, scroll.nMax, FALSE);
                SetScrollPos(hwnd, SB_VERT, scroll.nPos, TRUE);

                /*горизонтальный скролл*/
                HertScrollWithout(&scroll, iMaxWidth, client, sym, xPos, xOneList, text.maxStrLen);
                SetScrollRange(hwnd, SB_HORZ, 0, scroll.nMax, FALSE);
                SetScrollPos(hwnd, SB_HORZ, scroll.nPos, TRUE);

                InvalidateRect(hwnd, NULL, TRUE);
                UpdateWindow(hwnd);

		    	return 0;

            case ID_MODE_FILTER:

		        printf("ID_MODE_FILTER\n");
		        /*галочка меню*/
			    CheckMenuItem(hMenu, iChoice, MF_UNCHECKED);
			    iChoice = LOWORD(wParam);
			    CheckMenuItem(hMenu, iChoice, MF_CHECKED);

                GetClientRect(hwnd, &rect);
                GetScrollInfo(hwnd, SB_VERT, &scroll);

                /*вертикальный скролл*/
                VertScrollWith(&scroll, text, sym, client, rect, &width, &yOneList, yPos, &change, &newStrCount);
                SetScrollRange(hwnd, SB_VERT, 0, scroll.nMax, FALSE);
                SetScrollPos(hwnd, SB_VERT, scroll.nPos, TRUE);

                /*горизонтальный скролл*/
                HertScrollWith(&scroll);
                xPos = 0;
                SetScrollRange(hwnd, SB_HORZ, 0, scroll.nMax, FALSE);
                SetScrollPos(hwnd, SB_HORZ, scroll.nPos, TRUE);

		    	InvalidateRect(hwnd, NULL, TRUE);
		    	UpdateWindow(hwnd);
		    	return 0;
	    	}
	      	return 0;
        }

        case WM_DESTROY: {
            printf("WM_DESTROY\n");
            MyFree(&text);
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            break;
        }
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}
