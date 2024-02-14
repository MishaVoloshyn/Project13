#include "framework.h"
#include "Project13.h"
#include "vector"

#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

// Коллекция для хранения статичных элементов
std::vector<CStatic> statics;

// Функция для проверки корректности размеров
bool IsValidSize(int width, int height) {
    return width >= 10 && height >= 10;
}

// Функция для получения информации о статике
std::wstring GetStaticInfo(const CStatic& staticCtrl) {
    std::wstringstream ss;
    ss << L"Статик #" << staticCtrl.GetDlgCtrlID() << L"\n";
    ss << L"Ширина: " << staticCtrl.GetWidth() << L"\n";
    ss << L"Высота: " << staticCtrl.GetHeight() << L"\n";
    ss << L"Координаты: (" << staticCtrl.GetLeft() << ", " << staticCtrl.GetTop() << L")";
    return ss.str();
}

// Функция для обработки щелчка левой кнопкой мыши
void OnLButtonDown(HWND hWnd, int x, int y) {
    // Завершение создания статика, если он был начат
    if (statics.back().GetSafeHwnd()) {
        statics.back().DestroyWindow();
        statics.pop_back();
    }

    // Начало создания нового статика
    CStatic staticCtrl;
    staticCtrl.Create(L"STATIC", WS_CHILD | WS_VISIBLE, x, y, 10, 10, hWnd, (UINT)statics.size() + 1);
    statics.push_back(staticCtrl);
}

// Функция для обработки перемещения мыши
void OnMouseMove(HWND hWnd, int x, int y) {
    // Обновление размеров статика при его создании
    if (statics.back().GetSafeHwnd()) {
        int width = x - statics.back().GetLeft();
        int height = y - statics.back().GetTop();
        if (IsValidSize(width, height)) {
            statics.back().SetWindowPos(NULL, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER);
        }
    }
}

// Функция для обработки отпускания левой кнопки мыши
void OnLButtonUp(HWND hWnd, int x, int y) {
    // Завершение создания статика
    if (statics.back().GetSafeHwnd()) {
        int width = x - statics.back().GetLeft();
        int height = y - statics.back().GetTop();
        if (!IsValidSize(width, height)) {
            MessageBox(hWnd, L"Минимальный размер статика - 10x10", L"Ошибка", MB_OK);
            statics.back().DestroyWindow();
            statics.pop_back();
        }
    }
}

// Функция для обработки щелчка правой кнопкой мыши
void OnRButtonDown(HWND hWnd, int x, int y) {
    // Поиск статика под курсором
    CStatic* staticCtrl = nullptr;
    for (auto& staticItem : statics) {
        if (staticItem.GetSafeHwnd() && staticItem.PtInRect(x, y)) {
            staticCtrl = &staticItem;
            break;
        }
    }

    // Отображение информации о найденном статике
    if (staticCtrl) {
        std::wstring info = GetStaticInfo(*staticCtrl);
        MessageBox(hWnd, info.c_str(), L"Информация о статике", MB_OK);
    }
}

// Функция для обработки двойного щелчка левой кнопкой мыши
void OnLButtonDblClk(HWND hWnd, int x, int y) {
    // Поиск статика под курсором
    CStatic* staticCtrl = nullptr;
    for (auto& staticItem : statics) {
        if (staticItem.GetSafeHwnd() && staticItem.PtInRect(x, y)) {
                staticCtrl = &staticItem;
            break;
        }
    }

    // Удаление найденного статика
    if (staticCtrl) {
        std::wstring info = GetStaticInfo(*staticCtrl);
        staticCtrl->DestroyWindow();
        statics.erase(std::find(statics.begin(), statics.end(), *staticCtrl));

        MessageBox(hWnd, info.c_str(), L"Информация о статике", MB_OK);
    }
}

// Функция для регистрации класса окна
ATOM MyRegisterClass(HINSTANCE hInstance) {
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEXW);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PROJECT13));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_PROJECT13);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

// Функция для инициализации приложения
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
    hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd) {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

// Функция для обработки сообщений окна
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Разобрать выбор в меню:
        switch (wmId) {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: Добавьте сюда любой код прорисовки, использующий HDC...
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_LBUTTONDOWN:
        OnLButtonDown(hWnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        break;
    case WM_MOUSEMOVE:
        OnMouseMove(hWnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        break;
    case WM_LBUTTONUP:
        OnLButtonUp(hWnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        break;
    case WM_RBUTTONDOWN:
        OnRButtonDown(hWnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        break;
        case WM_LBUTTON
