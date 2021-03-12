#include <windows.h>
#include <cstdio>
using std::printf;

static HHOOK s_hCBTHook = NULL;

static LRESULT CALLBACK
CBTProc(INT nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode < 0 || s_hCBTHook == NULL)
        return ::CallNextHookEx(s_hCBTHook, nCode, wParam, lParam);

    HWND hwnd = reinterpret_cast<HWND>(wParam);
    switch (nCode)
    {
    case HCBT_ACTIVATE:
        printf("HCBT_ACTIVATE(hwnd:%p)\n", hwnd);
        break;
    case HCBT_CREATEWND:
        {
            DWORD tid, pid;
            WCHAR cls[MAX_PATH], txt[MAX_PATH];

            tid = GetWindowThreadProcessId(hwnd, &pid);
            GetClassNameW(hwnd, cls, MAX_PATH);
            GetWindowTextW(hwnd, txt, MAX_PATH);
            BOOL is_child = !!(GetWindowLong(hwnd, GWL_STYLE) & WS_CHILD);

            printf("HCBT_CREATEWND(hwnd:%p, pid:%lu (0x%lX), tid:%lu (0x%lX), cls:'%ls', txt:'%ls', is_child:%d)\n",
                   hwnd, pid, pid, tid, tid, cls, txt, is_child);
        }
        break;
    case HCBT_DESTROYWND:
        printf("HCBT_DESTROYWND(hwnd:%p)\n", hwnd);
        break;
    case HCBT_MINMAX:
        {
            INT nCmdShow = LOWORD(lParam);
            printf("HCBT_MINMAX(hwnd:%p,nCmdShow:%d)\n", hwnd, nCmdShow);
        }
        break;
    case HCBT_MOVESIZE:
        {
            LPRECT prc = (LPRECT)lParam;
            printf("HCBT_MOVESIZE(hwnd:%p,rc:{%ld, %ld, %ld, %ld})\n", hwnd,
                   prc->left, prc->top, prc->right, prc->bottom);
        }
        break;
    case HCBT_SETFOCUS:
        {
            HWND hwndLost = reinterpret_cast<HWND>(lParam);
            printf("HCBT_SETFOCUS(hwnd:%p, hwndLost:%p)\n", hwnd, hwndLost);
        }
        break;
    }

    return ::CallNextHookEx(s_hCBTHook, nCode, wParam, lParam);
}

int main(void)
{
    s_hCBTHook = ::SetWindowsHookEx(WH_CBT, CBTProc, NULL, GetCurrentThreadId());

    MessageBox(NULL, TEXT("OK"), NULL, 0);

    if (s_hCBTHook)
    {
        ::UnhookWindowsHookEx(s_hCBTHook);
        s_hCBTHook = NULL;
    }

    return 0;
}
