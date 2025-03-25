#pragma once

#include <atomic>
#include <mutex>
#include <thread>
#include <vector>

#include "../color.hpp"


#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX
    #include <windows.h>

    inline HWND g_hwnd = NULL;
    inline HDC g_memDC = NULL;
    inline HBITMAP g_hBitmap = NULL;
    inline std::vector<RGBTRIPLE> g_buffer;
    inline int g_width = 0, g_height = 0;
    inline std::mutex g_buffer_mutex;
    inline std::atomic<bool> g_window_closed(false);

    void update_preview(const std::vector<color>& pixels, const std::vector<int>& sample_counts, int width, int height) {
        // Create a local bitmap buffer
        std::vector<RGBQUAD> bitmap_buffer(width * height, RGBQUAD{77, 77, 77, 0});
        
        // Fill with pixel data
        for (int i = 0; i < width * height; i++) {
            int count{ sample_counts[i] };

            if (count == 0) {
                continue; // Skip further calculations
            }
            
            float r = pixels[i].x() / count;
            float g = pixels[i].y() / count;
            float b = pixels[i].z() / count;

            r = sqrt(std::min(r, 1.0f));
            g = sqrt(std::min(g, 1.0f));
            b = sqrt(std::min(b, 1.0f));

            bitmap_buffer[i].rgbRed = static_cast<BYTE>(255.99f * r);
            bitmap_buffer[i].rgbGreen = static_cast<BYTE>(255.99f * g);
            bitmap_buffer[i].rgbBlue = static_cast<BYTE>(255.99f * b);
            bitmap_buffer[i].rgbReserved = 0;
        }
        
        std::lock_guard<std::mutex> lock(g_buffer_mutex);
        
        if (!g_hwnd) return;
        
        HDC hdc = GetDC(g_hwnd);
        if (!hdc) return;
        
        // Create a temporary bitmap and draw directly to window
        BITMAPINFO bmi = {0};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = width;
        bmi.bmiHeader.biHeight = -height; // Negative for top-down
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;    // Using 32-bit RGBQUAD
        bmi.bmiHeader.biCompression = BI_RGB;
        
        // Draw directly to window DC
        SetDIBitsToDevice(
            hdc,                // Destination DC
            0, 0,               // Destination coords
            width, height,      // Width, height
            0, 0,               // Source coords
            0,                  // First scan line
            height,             // Number of scan lines
            bitmap_buffer.data(), // Source bits
            &bmi,               // Bitmap info
            DIB_RGB_COLORS      // Usage
        );
        
        ReleaseDC(g_hwnd, hdc);
    }

    LRESULT CALLBACK wnd_proc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param) {
        switch (message) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            {
                std::lock_guard<std::mutex> lock(g_buffer_mutex);
                if (g_memDC && g_hBitmap) {
                    BitBlt(hdc, 0, 0, g_width, g_height, g_memDC, 0, 0, SRCCOPY);
                }
            }

            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_ERASEBKGND: {
            HDC hdc = (HDC)w_param;
            RECT rect;
            GetClientRect(hwnd, &rect);
            HBRUSH brush = CreateSolidBrush(RGB(128, 128, 128)); // Gray background
            FillRect(hdc, &rect, brush);
            DeleteObject(brush);
            return 1; // Prevent default erase
        }
        case WM_CLOSE:
            g_window_closed = true;
            DestroyWindow(hwnd);
            return 0;
        
        case WM_DESTROY:
            {
                std::lock_guard<std::mutex> lock(g_buffer_mutex);
                if (g_hBitmap) DeleteObject(g_hBitmap);
                if (g_memDC) DeleteDC(g_memDC);
                g_hBitmap = NULL;
                g_memDC = NULL;
            }
            PostQuitMessage(0);
            return 0;
        }

        return DefWindowProc(hwnd, message, w_param, l_param);
    }

    void window_thread_func(HINSTANCE h_instance, int width, int height
        , const std::vector<color>& pixels, const std::vector<int>& sample_counts
        , std::mutex& frame_buffer_mut, std::mutex& samples_mut) {
        WNDCLASS wc{0};
        wc.lpfnWndProc = wnd_proc;
        wc.hInstance = h_instance;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = CreateSolidBrush(RGB(128, 128, 128));
        wc.lpszClassName = "RenderPreviewClass";

        if(!RegisterClass(&wc)) {
            MessageBoxA(NULL, "Window Registration Failed!", "Error", MB_ICONERROR);
            return;
        }

        g_hwnd = CreateWindow(
            "RenderPreviewClass", "Render Preview",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT,
            width + 16, height + 39, // Add padding for borders
            NULL, NULL, h_instance, NULL
        );

        if (!g_hwnd) {
            MessageBoxA(NULL, "Window Creation Failed!", "Error", MB_ICONERROR);
            return;
        }

        ShowWindow(g_hwnd, SW_SHOW);
        UpdateWindow(g_hwnd);

        MSG msg;
        while (!g_window_closed) {
            auto start_time = std::chrono::high_resolution_clock::now();
            while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
                if (msg.message == WM_QUIT) {
                    g_window_closed = true;
                    break;
                }
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }

            {
                std::lock_guard<std::mutex> lock(frame_buffer_mut);
                std::lock_guard<std::mutex> lock_samples(samples_mut);
                update_preview(pixels, sample_counts, width, height);
            }
    
            if (g_hwnd) {
                InvalidateRect(g_hwnd, NULL, FALSE); // Force repaint
            }

            // Sleep for remainder of 33ms frame time
            auto elapsed = std::chrono::high_resolution_clock::now() - start_time;
            auto target_duration = std::chrono::milliseconds(33);
            if (elapsed < target_duration) {
                std::this_thread::sleep_for(target_duration - elapsed);
            }
        }
    }
#endif
