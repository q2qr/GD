#include "main.h"
#include "AppDelegate.h"
#include "CCEGLView.h"

#include <stdio.h>

static void writeCrashAddress(FILE* file, DWORD address) {
    HMODULE module = NULL;
    char name[MAX_PATH] = {};
    if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        (LPCSTR)address, &module)) {
        GetModuleFileNameA(module, name, MAX_PATH);
        fprintf(file, "%s + 0x%08lx\n", name, address - (DWORD)module);
    } else fprintf(file, "0x%08lx\n", address);
}

static LONG WINAPI logGameCrash(EXCEPTION_POINTERS* exception) {
    FILE* file = fopen("crash_trace.txt", "w");
    if (file) {
        fprintf(file, "Exception 0x%08lx\n", exception->ExceptionRecord->ExceptionCode);
        writeCrashAddress(file, exception->ContextRecord->Eip);
        DWORD frame = exception->ContextRecord->Ebp;
        for (int i = 0; i < 30 && frame; ++i) {
            DWORD words[2]; SIZE_T bytes = 0;
            if (!ReadProcessMemory(GetCurrentProcess(), (LPCVOID)frame, words, sizeof(words), &bytes) || bytes != sizeof(words)) break;
            writeCrashAddress(file, words[1]);
            if (words[0] <= frame || words[0] - frame > 1024 * 1024) break;
            frame = words[0];
        }
        fclose(file);
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

USING_NS_CC;

int APIENTRY _tWinMain(HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       LPTSTR    lpCmdLine,
                       int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    SetUnhandledExceptionFilter(logGameCrash);
    // create the application instance
    AppDelegate app;
    CCEGLView* eglView = CCEGLView::sharedOpenGLView();
    eglView->setViewName("Geometry Dash");
    eglView->setFrameSize(960, 640);
    return CCApplication::sharedApplication()->run();
}
