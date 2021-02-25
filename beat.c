#undef UNICODE
#undef _UNICODE
#include <windows.h>
#include "beat.h"
#include <stdio.h>

LRESULT CALLBACK wndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void getViewRect (HWND hwnd, LPRECT viewRect);
const char wcName[] = "Beat Window";
int afBuff = 0; // Tracks number of audio frames currently buffered
int buffcnt = 0;

int APIENTRY WinMain (HINSTANCE hinst, HINSTANCE phinst, LPSTR cmdLine, int nCmdShow) {
    HWND hwnd;
    HWAVEOUT hwo;
    // Create window
    {
    WNDCLASS wc;
    ZeroMemory(&wc,sizeof(wc));
    wc.hInstance = hinst;
    wc.lpfnWndProc = &wndProc;
    wc.lpszClassName = wcName;
    wc.hCursor = LoadCursor(NULL,IDC_ARROW);
    wc.hIcon = LoadIcon(NULL,IDI_APPLICATION);
    wc.hbrBackground = CreateSolidBrush(RGB(bgColor.color.red,bgColor.color.green,bgColor.color.blue));
    RegisterClass(&wc);
    hwnd = CreateWindow(
        wcName, winTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,CW_USEDEFAULT, 800,600,
        NULL,NULL,hinst,NULL
    );
    if (hwnd == NULL) {
        MessageBox(0,"The window could not be created","Error",MB_ICONERROR);
        return EXIT_FAILURE;
    }
    }
    // Get audio device
    {
    WAVEFORMATEX wf;
    ZeroMemory(&wf,sizeof(wf));
    wf.wFormatTag = WAVE_FORMAT_PCM;
    wf.nChannels = 1;
    wf.nSamplesPerSec = 44100;
    wf.wBitsPerSample = 16;
    wf.nBlockAlign = 2;
    wf.nAvgBytesPerSec = 44100;
    MMRESULT mr = waveOutOpen(&hwo,WAVE_MAPPER,&wf,(DWORD_PTR)hwnd,(DWORD_PTR)0,CALLBACK_WINDOW);
    if (mr) { // Error 
        MessageBox(0,"Could not access the audio device","Error",MB_ICONERROR);
        return EXIT_FAILURE;
    }
    }
    // Show the window
    ShowWindow(hwnd,SW_SHOW);
    MSG msg;
    ZeroMemory(&msg,sizeof(msg));
    bool quit = false;
    // Tell the program to initialize
    init();
    DWORD initTime = timeGetTime();
    DWORD lastUpdate = initTime;
    DWORD lastDraw = initTime;
    DWORD lastPlay = initTime;
    // Listen for messages and call update functions
    while (true) {
        while (PeekMessage(&msg,NULL,0,0,PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                quit = true;
                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        if (quit) break;
        DWORD currTime = timeGetTime();
        update((double)(currTime-initTime)/1000, (double)(currTime-lastUpdate)/1000);
        lastUpdate = currTime;
        if (currTime-lastDraw > (1000/60)) {
            lastDraw = currTime;
            InvalidateRect(hwnd,NULL,false);
        }
        if ((currTime-initTime)/10 > buffcnt) {
            lastPlay = currTime;
            audioframe buffer = malloc(sizeof(*buffer));
            play(buffer,(currTime-initTime)/10-buffcnt);
            LPWAVEHDR wh = malloc(sizeof(WAVEHDR));
            ZeroMemory(wh,sizeof(WAVEHDR));
            wh->lpData = (char*)buffer;
            wh->dwBufferLength = sizeof(*buffer);
            waveOutPrepareHeader(hwo,wh,sizeof(WAVEHDR));
            waveOutWrite(hwo,wh,sizeof(WAVEHDR));
            afBuff++;
            buffcnt = (currTime-initTime)/10;
        }
        for (int l=0; l<256; l++) {
            pkeys[l] = keys[l];
        }
        for (int l=0; l<256; l++) {
            kchars[l] = false;
        }
    }
    cleanup();
    waveOutReset(hwo);
    waveOutClose(hwo);
}
void getViewRect (HWND hwnd, LPRECT viewRect) {
    int winWidth, winHeight;
    {
        RECT winRect;
        GetClientRect(hwnd,&winRect);
        winWidth = winRect.right-winRect.left;
        winHeight = winRect.bottom-winRect.top;
    }
    int mindim = min(winWidth,winHeight);
    int drawdim = mindim;
    if (mindim >= viewSize) {
        drawdim = mindim-mindim%viewSize;
    } else if (mindim > 0) {
        drawdim = viewSize;
        while (drawdim > mindim) {
            drawdim /= 2;
        }
    }
    viewRect->left = (winWidth-drawdim)/2;
    viewRect->top = (winHeight-drawdim)/2;
    viewRect->right = viewRect->left+drawdim;
    viewRect->bottom = viewRect->top+drawdim;
}

LRESULT CALLBACK wndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case (MM_WOM_OPEN): {
            break;
        }
        case (MM_WOM_DONE): {
            afBuff--;
            HWAVEOUT hwo = (HWAVEOUT)wParam;
            LPWAVEHDR wh = (LPWAVEHDR)lParam;
            waveOutUnprepareHeader(hwo,wh,sizeof(WAVEHDR));
            free(wh->lpData);
            free(wh);
            break;
        }
        case (MM_WOM_CLOSE): {
            break;
        }
        case (WM_PAINT): {
            PAINTSTRUCT ps;
            ZeroMemory(&ps,sizeof(ps));
            HDC dc = BeginPaint(hwnd,&ps);
            pixel *pixels = malloc(sizeof(pixel)*viewSize*viewSize);
            draw((screen){viewSize,viewSize,pixels});
            HBITMAP hbm = CreateBitmap(viewSize,viewSize,1,32,pixels);
            HDC bmdc = CreateCompatibleDC(dc);
            SelectObject(bmdc,hbm);
            RECT vr;
            getViewRect(hwnd,&vr);
            StretchBlt(dc,vr.left,vr.top,vr.right-vr.left,vr.bottom-vr.top,bmdc,0,0,viewSize,viewSize,SRCCOPY);
            DeleteObject(hbm);
            DeleteDC(bmdc);
            free(pixels);
            EndPaint(hwnd,&ps);
            return 0;
        }
        case (WM_SIZE): {
            InvalidateRect(hwnd,NULL,TRUE);
            return 0;
        }
        case (WM_CLOSE): {
            DestroyWindow(hwnd);
            return 0;
        }
        case (WM_LBUTTONDOWN): {
            mouseLButton = true;
            return 0;
        }
        case (WM_LBUTTONUP): {
            mouseLButton = false;
            return 0;
        }
        case (WM_RBUTTONDOWN): {
            mouseRButton = true;
            return 0;
        }
        case (WM_RBUTTONUP): {
            mouseRButton = false;
            return 0;
        }
        case (WM_MOUSEMOVE): {
            RECT vr;
            getViewRect(hwnd,&vr);
            float scale = ((float)viewSize)/(vr.bottom-vr.top);
            mouseX = ((short)LOWORD(lParam)-vr.left)*scale;
            mouseY = ((short)HIWORD(lParam)-vr.top)*scale;
            bool pmouseInScreen = mouseInScreen;
            mouseInScreen = 0 <= mouseX && mouseX < viewSize && 0 <= mouseY && mouseY < viewSize;
            if (mouseInScreen != pmouseInScreen) ShowCursor(!mouseInScreen);
            return 0;
        }
        case (WM_CHAR): {
            if (0 <= wParam && wParam < 256) kchars[wParam] = true;
            return 0;
        }
        case (WM_KEYDOWN): {
            if (0 <= wParam && wParam < 256) keys[wParam] = true;
            return 0;
        }
        case (WM_KEYUP): {
            if (0 <= wParam && wParam < 256) keys[wParam] = false;
            return 0;
        }
        case (WM_DESTROY): {
            PostQuitMessage(EXIT_SUCCESS);
            return 0;
        }
    }
    DefWindowProc(hwnd,msg,wParam,lParam);
}
