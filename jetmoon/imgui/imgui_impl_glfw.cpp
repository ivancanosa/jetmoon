// dear imgui: Platform Backend for GLFW
// This needs to be used along with a Renderer (e.g. OpenGL3, Vulkan, WebGPU..)
// (Info: GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan graphics context creation, etc.)
// (Requires: GLFW 3.1+)

// Implemented features:
//  [X] Platform: Clipboard support.
//  [X] Platform: Gamepad support. Enable with 'io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad'.
//  [X] Platform: Mouse cursor shape and visibility. Disable with 'io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange' (note: the resizing cursors requires GLFW 3.4+).
//  [X] Platform: Keyboard arrays indexed using GLFW_KEY_* codes, e.g. ImGui::IsKeyPressed(GLFW_KEY_SPACE).

// You can copy and use unmodified imgui_impl_* files in your project. See examples/ folder for examples of using this.
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

// CHANGELOG
// (minor and older changes stripped away, please see git history for details)
//  2020-01-17: Inputs: Disable error callback while assigning mouse cursors because some X11 setup don't have them and it generates errors.
//  2019-12-05: Inputs: Added support for new mouse cursors added in GLFW 3.4+ (resizing cursors, not allowed cursor).
//  2019-10-18: Misc: Previously installed user callbacks are now restored on shutdown.
//  2019-07-21: Inputs: Added mapping for ImGuiKey_KeyPadEnter.
//  2019-05-11: Inputs: Don't filter value from character callback before calling AddInputCharacter().
//  2019-03-12: Misc: Preserve DisplayFramebufferScale when main window is minimized.
//  2018-11-30: Misc: Setting up io.BackendPlatformName so it can be displayed in the About Window.
//  2018-11-07: Inputs: When installing our GLFW callbacks, we save user's previously installed ones - if any - and chain call them.
//  2018-08-01: Inputs: Workaround for Emscripten which doesn't seem to handle focus related calls.
//  2018-06-29: Inputs: Added support for the ImGuiMouseCursor_Hand cursor.
//  2018-06-08: Misc: Extracted imgui_impl_glfw.cpp/.h away from the old combined GLFW+OpenGL/Vulkan examples.
//  2018-03-20: Misc: Setup io.BackendFlags ImGuiBackendFlags_HasMouseCursors flag + honor ImGuiConfigFlags_NoMouseCursorChange flag.
//  2018-02-20: Inputs: Added support for mouse cursors (ImGui::GetMouseCursor() value, passed to glfwSetCursor()).
//  2018-02-06: Misc: Removed call to ImGui::Shutdown() which is not available from 1.60 WIP, user needs to call CreateContext/DestroyContext themselves.
//  2018-02-06: Inputs: Added mapping for ImGuiKey_Space.
//  2018-01-25: Inputs: Added gamepad support if ImGuiConfigFlags_NavEnableGamepad is set.
//  2018-01-25: Inputs: Honoring the io.WantSetMousePos by repositioning the mouse (when using navigation and ImGuiConfigFlags_NavMoveMouse is set).
//  2018-01-20: Inputs: Added Horizontal Mouse Wheel support.
//  2018-01-18: Inputs: Added mapping for ImGuiKey_Insert.
//  2017-08-25: Inputs: MousePos set to -FLT_MAX,-FLT_MAX when mouse is unavailable/missing (instead of -1,-1).
//  2016-10-15: Misc: Added a void* user_data parameter to Clipboard function handlers.

#include <iostream>
#include "imgui.h"
#include "imgui_impl_glfw.h"

float imgui_viewport[4] = {0,0,1.,1.};
int imgui_changedViewport = 0;
// GLFW
#include <GLFW/glfw3.h>
#ifdef _WIN32
#undef APIENTRY
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>   // for glfwGetWin32Window
#endif
#define GLFW_HAS_WINDOW_TOPMOST       (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3200) // 3.2+ GLFW_FLOATING
#define GLFW_HAS_WINDOW_HOVERED       (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3300) // 3.3+ GLFW_HOVERED
#define GLFW_HAS_WINDOW_ALPHA         (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3300) // 3.3+ glfwSetWindowOpacity
#define GLFW_HAS_PER_MONITOR_DPI      (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3300) // 3.3+ glfwGetMonitorContentScale
#define GLFW_HAS_VULKAN               (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3200) // 3.2+ glfwCreateWindowSurface
#ifdef GLFW_RESIZE_NESW_CURSOR  // let's be nice to people who pulled GLFW between 2019-04-16 (3.4 define) and 2019-11-29 (cursors defines) // FIXME: Remove when GLFW 3.4 is released?
#define GLFW_HAS_NEW_CURSORS          (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3400) // 3.4+ GLFW_RESIZE_ALL_CURSOR, GLFW_RESIZE_NESW_CURSOR, GLFW_RESIZE_NWSE_CURSOR, GLFW_NOT_ALLOWED_CURSOR
#else
#define GLFW_HAS_NEW_CURSORS          (0)
#endif

unsigned int glfwToImguiKeys[GLFW_KEY_LAST+1]{};

// Data
enum GlfwClientApi
{
    GlfwClientApi_Unknown,
    GlfwClientApi_OpenGL,
    GlfwClientApi_Vulkan
};
static GLFWwindow*          g_Window = NULL;    // Main window
static GlfwClientApi        g_ClientApi = GlfwClientApi_Unknown;
static double               g_Time = 0.0;
static bool                 g_MouseJustPressed[ImGuiMouseButton_COUNT] = {};
static GLFWcursor*          g_MouseCursors[ImGuiMouseCursor_COUNT] = {};
static bool                 g_InstalledCallbacks = false;

// Chain GLFW callbacks: our callbacks will call the user's previously installed callbacks, if any.
static GLFWmousebuttonfun   g_PrevUserCallbackMousebutton = NULL;
static GLFWscrollfun        g_PrevUserCallbackScroll = NULL;
static GLFWkeyfun           g_PrevUserCallbackKey = NULL;
static GLFWcharfun          g_PrevUserCallbackChar = NULL;

static const char* ImGui_ImplGlfw_GetClipboardText(void* user_data)
{
    return glfwGetClipboardString((GLFWwindow*)user_data);
}

static void ImGui_ImplGlfw_SetClipboardText(void* user_data, const char* text)
{
    glfwSetClipboardString((GLFWwindow*)user_data, text);
}

void ImGui_ImplGlfw_MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (g_PrevUserCallbackMousebutton != NULL)
        g_PrevUserCallbackMousebutton(window, button, action, mods);

    if (action == GLFW_PRESS && button >= 0 && button < IM_ARRAYSIZE(g_MouseJustPressed))
        g_MouseJustPressed[button] = true;
}

void ImGui_ImplGlfw_ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (g_PrevUserCallbackScroll != NULL)
        g_PrevUserCallbackScroll(window, xoffset, yoffset);

    ImGuiIO& io = ImGui::GetIO();
    io.MouseWheelH += (float)xoffset;
    io.MouseWheel += (float)yoffset;
}

void ImGui_ImplGlfw_KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (g_PrevUserCallbackKey != NULL)
        g_PrevUserCallbackKey(window, key, scancode, action, mods);

    if (action != GLFW_PRESS && action != GLFW_RELEASE)
        return;

    ImGuiIO& io = ImGui::GetIO();

	io.AddKeyEvent(glfwToImguiKeys[key], action);

/*	io.AddKeyEvent(ImGuiKey_ModCtrl, mods & (1 << GLFW_MOD_SHIFT)); 
	io.AddKeyEvent(ImGuiKey_ModAlt, mods & (1 << GLFW_MOD_CONTROL)); 
	io.AddKeyEvent(ImGuiKey_ModShift, mods & (1 << GLFW_MOD_ALT )); */
}

void ImGui_ImplGlfw_CharCallback(GLFWwindow* window, unsigned int c)
{
    if (g_PrevUserCallbackChar != NULL)
        g_PrevUserCallbackChar(window, c);

    ImGuiIO& io = ImGui::GetIO();
    io.AddInputCharacter(c);
}

static bool ImGui_ImplGlfw_Init(GLFWwindow* window, bool install_callbacks, GlfwClientApi client_api)
{
    g_Window = window;
    g_Time = 0.0;

    // Setup backend capabilities flags
    ImGuiIO& io = ImGui::GetIO();
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;         // We can honor GetMouseCursor() values (optional)
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;          // We can honor io.WantSetMousePos requests (optional, rarely used)
    io.BackendPlatformName = "imgui_impl_glfw";


glfwToImguiKeys[GLFW_KEY_SPACE        ]=       ImGuiKey_Space;
glfwToImguiKeys[GLFW_KEY_APOSTROPHE   ]=       ImGuiKey_Apostrophe;
glfwToImguiKeys[GLFW_KEY_COMMA        ]=       ImGuiKey_Comma;
glfwToImguiKeys[GLFW_KEY_MINUS        ]=       ImGuiKey_Minus;
glfwToImguiKeys[GLFW_KEY_PERIOD       ]=       ImGuiKey_Period;
glfwToImguiKeys[GLFW_KEY_SLASH        ]=       ImGuiKey_Slash;
glfwToImguiKeys[GLFW_KEY_0            ]=       ImGuiKey_0;
glfwToImguiKeys[GLFW_KEY_1            ]=       ImGuiKey_1;
glfwToImguiKeys[GLFW_KEY_2            ]=       ImGuiKey_2;
glfwToImguiKeys[GLFW_KEY_3            ]=       ImGuiKey_3;
glfwToImguiKeys[GLFW_KEY_4            ]=       ImGuiKey_4;
glfwToImguiKeys[GLFW_KEY_5            ]=       ImGuiKey_5;
glfwToImguiKeys[GLFW_KEY_6            ]=       ImGuiKey_6;
glfwToImguiKeys[GLFW_KEY_7            ]=       ImGuiKey_7;
glfwToImguiKeys[GLFW_KEY_8            ]=       ImGuiKey_8;
glfwToImguiKeys[GLFW_KEY_9            ]=       ImGuiKey_9;
glfwToImguiKeys[GLFW_KEY_SEMICOLON    ]=       ImGuiKey_Semicolon;
glfwToImguiKeys[GLFW_KEY_EQUAL        ]=       ImGuiKey_Equal;
glfwToImguiKeys[GLFW_KEY_A            ]=       ImGuiKey_A;
glfwToImguiKeys[GLFW_KEY_B            ]=       ImGuiKey_B;
glfwToImguiKeys[GLFW_KEY_C            ]=       ImGuiKey_C;
glfwToImguiKeys[GLFW_KEY_D            ]=       ImGuiKey_D;
glfwToImguiKeys[GLFW_KEY_E            ]=       ImGuiKey_E;
glfwToImguiKeys[GLFW_KEY_F            ]=       ImGuiKey_F;
glfwToImguiKeys[GLFW_KEY_G            ]=       ImGuiKey_G;
glfwToImguiKeys[GLFW_KEY_H            ]=       ImGuiKey_H;
glfwToImguiKeys[GLFW_KEY_I            ]=       ImGuiKey_I;
glfwToImguiKeys[GLFW_KEY_J            ]=       ImGuiKey_J;
glfwToImguiKeys[GLFW_KEY_K            ]=       ImGuiKey_K;
glfwToImguiKeys[GLFW_KEY_L            ]=       ImGuiKey_L;
glfwToImguiKeys[GLFW_KEY_M            ]=       ImGuiKey_M;
glfwToImguiKeys[GLFW_KEY_N            ]=       ImGuiKey_N;
glfwToImguiKeys[GLFW_KEY_O            ]=       ImGuiKey_O;
glfwToImguiKeys[GLFW_KEY_P            ]=       ImGuiKey_P;
glfwToImguiKeys[GLFW_KEY_Q            ]=       ImGuiKey_Q;
glfwToImguiKeys[GLFW_KEY_R            ]=       ImGuiKey_R;
glfwToImguiKeys[GLFW_KEY_S            ]=       ImGuiKey_S;
glfwToImguiKeys[GLFW_KEY_T            ]=       ImGuiKey_T;
glfwToImguiKeys[GLFW_KEY_U            ]=       ImGuiKey_U;
glfwToImguiKeys[GLFW_KEY_V            ]=       ImGuiKey_V;
glfwToImguiKeys[GLFW_KEY_W            ]=       ImGuiKey_W;
glfwToImguiKeys[GLFW_KEY_X            ]=       ImGuiKey_X;
glfwToImguiKeys[GLFW_KEY_Y            ]=       ImGuiKey_Y;
glfwToImguiKeys[GLFW_KEY_Z            ]=       ImGuiKey_Z;
glfwToImguiKeys[GLFW_KEY_LEFT_BRACKET ]=       ImGuiKey_LeftBracket;
glfwToImguiKeys[GLFW_KEY_BACKSLASH    ]=       ImGuiKey_Backslash;
glfwToImguiKeys[GLFW_KEY_RIGHT_BRACKET]=       ImGuiKey_RightBracket;
glfwToImguiKeys[GLFW_KEY_GRAVE_ACCENT ]=       ImGuiKey_GraveAccent;
glfwToImguiKeys[GLFW_KEY_WORLD_1      ]=       ImGuiKey_None;
glfwToImguiKeys[GLFW_KEY_WORLD_2      ]=       ImGuiKey_None;

glfwToImguiKeys[GLFW_KEY_ESCAPE       ]=       ImGuiKey_Escape;
glfwToImguiKeys[GLFW_KEY_ENTER        ]=       ImGuiKey_Enter;
glfwToImguiKeys[GLFW_KEY_TAB          ]=       ImGuiKey_Tab;
glfwToImguiKeys[GLFW_KEY_BACKSPACE    ]=       ImGuiKey_Backspace;
glfwToImguiKeys[GLFW_KEY_INSERT       ]=       ImGuiKey_Insert;
glfwToImguiKeys[GLFW_KEY_DELETE       ]=       ImGuiKey_Delete;
glfwToImguiKeys[GLFW_KEY_RIGHT        ]=       ImGuiKey_RightArrow;
glfwToImguiKeys[GLFW_KEY_LEFT         ]=       ImGuiKey_LeftArrow;
glfwToImguiKeys[GLFW_KEY_DOWN         ]=       ImGuiKey_DownArrow;
glfwToImguiKeys[GLFW_KEY_UP           ]=       ImGuiKey_UpArrow;
glfwToImguiKeys[GLFW_KEY_PAGE_UP      ]=       ImGuiKey_PageUp;
glfwToImguiKeys[GLFW_KEY_PAGE_DOWN    ]=       ImGuiKey_PageDown;
glfwToImguiKeys[GLFW_KEY_HOME         ]=       ImGuiKey_Home;
glfwToImguiKeys[GLFW_KEY_END          ]=       ImGuiKey_End;
glfwToImguiKeys[GLFW_KEY_CAPS_LOCK    ]=       ImGuiKey_CapsLock;
glfwToImguiKeys[GLFW_KEY_SCROLL_LOCK  ]=       ImGuiKey_ScrollLock;
glfwToImguiKeys[GLFW_KEY_NUM_LOCK     ]=       ImGuiKey_NumLock;
glfwToImguiKeys[GLFW_KEY_PRINT_SCREEN ]=       ImGuiKey_PrintScreen;
glfwToImguiKeys[GLFW_KEY_PAUSE        ]=       ImGuiKey_Pause;
glfwToImguiKeys[GLFW_KEY_F1           ]=       ImGuiKey_F1;
glfwToImguiKeys[GLFW_KEY_F2           ]=       ImGuiKey_F2;
glfwToImguiKeys[GLFW_KEY_F3           ]=       ImGuiKey_F3;
glfwToImguiKeys[GLFW_KEY_F4           ]=       ImGuiKey_F4;
glfwToImguiKeys[GLFW_KEY_F5           ]=       ImGuiKey_F5;
glfwToImguiKeys[GLFW_KEY_F6           ]=       ImGuiKey_F6;
glfwToImguiKeys[GLFW_KEY_F7           ]=       ImGuiKey_F7;
glfwToImguiKeys[GLFW_KEY_F8           ]=       ImGuiKey_F8;
glfwToImguiKeys[GLFW_KEY_F9           ]=       ImGuiKey_F9;
glfwToImguiKeys[GLFW_KEY_F10          ]=       ImGuiKey_F10;
glfwToImguiKeys[GLFW_KEY_F11          ]=       ImGuiKey_F11;
glfwToImguiKeys[GLFW_KEY_F12          ]=       ImGuiKey_F12;
glfwToImguiKeys[GLFW_KEY_F13          ]=       ImGuiKey_None;
glfwToImguiKeys[GLFW_KEY_F14          ]=       ImGuiKey_None;
glfwToImguiKeys[GLFW_KEY_F15          ]=       ImGuiKey_None;
glfwToImguiKeys[GLFW_KEY_F16          ]=       ImGuiKey_None;
glfwToImguiKeys[GLFW_KEY_F17          ]=       ImGuiKey_None;
glfwToImguiKeys[GLFW_KEY_F18          ]=       ImGuiKey_None;
glfwToImguiKeys[GLFW_KEY_F19          ]=       ImGuiKey_None;
glfwToImguiKeys[GLFW_KEY_F20          ]=       ImGuiKey_None;
glfwToImguiKeys[GLFW_KEY_F21          ]=       ImGuiKey_None;
glfwToImguiKeys[GLFW_KEY_F22          ]=       ImGuiKey_None;
glfwToImguiKeys[GLFW_KEY_F23          ]=       ImGuiKey_None;
glfwToImguiKeys[GLFW_KEY_F24          ]=       ImGuiKey_None;
glfwToImguiKeys[GLFW_KEY_F25          ]=       ImGuiKey_None;
glfwToImguiKeys[GLFW_KEY_KP_0         ]=       ImGuiKey_Keypad0;
glfwToImguiKeys[GLFW_KEY_KP_1         ]=       ImGuiKey_Keypad1;
glfwToImguiKeys[GLFW_KEY_KP_2         ]=       ImGuiKey_Keypad2;
glfwToImguiKeys[GLFW_KEY_KP_3         ]=       ImGuiKey_Keypad3;
glfwToImguiKeys[GLFW_KEY_KP_4         ]=       ImGuiKey_Keypad4;
glfwToImguiKeys[GLFW_KEY_KP_5         ]=       ImGuiKey_Keypad5;
glfwToImguiKeys[GLFW_KEY_KP_6         ]=       ImGuiKey_Keypad6;
glfwToImguiKeys[GLFW_KEY_KP_7         ]=       ImGuiKey_Keypad7;
glfwToImguiKeys[GLFW_KEY_KP_8         ]=       ImGuiKey_Keypad8;
glfwToImguiKeys[GLFW_KEY_KP_9         ]=       ImGuiKey_Keypad9;
glfwToImguiKeys[GLFW_KEY_KP_DECIMAL   ]=       ImGuiKey_KeypadDecimal;
glfwToImguiKeys[GLFW_KEY_KP_DIVIDE    ]=       ImGuiKey_KeypadDivide;
glfwToImguiKeys[GLFW_KEY_KP_MULTIPLY  ]=       ImGuiKey_KeypadMultiply;
glfwToImguiKeys[GLFW_KEY_KP_SUBTRACT  ]=       ImGuiKey_KeypadSubtract;
glfwToImguiKeys[GLFW_KEY_KP_ADD       ]=       ImGuiKey_KeypadAdd;
glfwToImguiKeys[GLFW_KEY_KP_ENTER     ]=       ImGuiKey_KeypadEnter;
glfwToImguiKeys[GLFW_KEY_KP_EQUAL     ]=       ImGuiKey_KeypadEqual;
glfwToImguiKeys[GLFW_KEY_LEFT_SHIFT   ]=       ImGuiKey_LeftShift;
glfwToImguiKeys[GLFW_KEY_LEFT_CONTROL ]=       ImGuiKey_LeftCtrl;
glfwToImguiKeys[GLFW_KEY_LEFT_ALT     ]=       ImGuiKey_LeftAlt;
glfwToImguiKeys[GLFW_KEY_LEFT_SUPER   ]=       ImGuiKey_LeftSuper;
glfwToImguiKeys[GLFW_KEY_RIGHT_SHIFT  ]=       ImGuiKey_RightShift;
glfwToImguiKeys[GLFW_KEY_RIGHT_CONTROL]=       ImGuiKey_RightCtrl;
glfwToImguiKeys[GLFW_KEY_RIGHT_ALT    ]=       ImGuiKey_RightAlt;
glfwToImguiKeys[GLFW_KEY_RIGHT_SUPER  ]=       ImGuiKey_RightSuper;
glfwToImguiKeys[GLFW_KEY_MENU         ]=       ImGuiKey_Menu;
    // Keyboard mapping. Dear ImGui will use those indices to peek into the io.KeysDown[] array.
	/*
    io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
    io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
    io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
    io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
    io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
    io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
    io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
    io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
    io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
    io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
    io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
    io.KeyMap[ImGuiKey_KeyPadEnter] = GLFW_KEY_KP_ENTER;
    io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
    io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
    io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
    io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
    io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
    io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;*/

    io.SetClipboardTextFn = ImGui_ImplGlfw_SetClipboardText;
    io.GetClipboardTextFn = ImGui_ImplGlfw_GetClipboardText;
    io.ClipboardUserData = g_Window;
#if defined(_WIN32)
    io.ImeWindowHandle = (void*)glfwGetWin32Window(g_Window);
#endif

    // Create mouse cursors
    // (By design, on X11 cursors are user configurable and some cursors may be missing. When a cursor doesn't exist,
    // GLFW will emit an error which will often be printed by the app, so we temporarily disable error reporting.
    // Missing cursors will return NULL and our _UpdateMouseCursor() function will use the Arrow cursor instead.)
    GLFWerrorfun prev_error_callback = glfwSetErrorCallback(NULL);
    g_MouseCursors[ImGuiMouseCursor_Arrow] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    g_MouseCursors[ImGuiMouseCursor_TextInput] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
    g_MouseCursors[ImGuiMouseCursor_ResizeNS] = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
    g_MouseCursors[ImGuiMouseCursor_ResizeEW] = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
    g_MouseCursors[ImGuiMouseCursor_Hand] = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
#if GLFW_HAS_NEW_CURSORS
    g_MouseCursors[ImGuiMouseCursor_ResizeAll] = glfwCreateStandardCursor(GLFW_RESIZE_ALL_CURSOR);
    g_MouseCursors[ImGuiMouseCursor_ResizeNESW] = glfwCreateStandardCursor(GLFW_RESIZE_NESW_CURSOR);
    g_MouseCursors[ImGuiMouseCursor_ResizeNWSE] = glfwCreateStandardCursor(GLFW_RESIZE_NWSE_CURSOR);
    g_MouseCursors[ImGuiMouseCursor_NotAllowed] = glfwCreateStandardCursor(GLFW_NOT_ALLOWED_CURSOR);
#else
    g_MouseCursors[ImGuiMouseCursor_ResizeAll] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    g_MouseCursors[ImGuiMouseCursor_ResizeNESW] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    g_MouseCursors[ImGuiMouseCursor_ResizeNWSE] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    g_MouseCursors[ImGuiMouseCursor_NotAllowed] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
#endif
    glfwSetErrorCallback(prev_error_callback);

    // Chain GLFW callbacks: our callbacks will call the user's previously installed callbacks, if any.
    g_PrevUserCallbackMousebutton = NULL;
    g_PrevUserCallbackScroll = NULL;
    g_PrevUserCallbackKey = NULL;
    g_PrevUserCallbackChar = NULL;
    if (install_callbacks)
    {
        g_InstalledCallbacks = true;
        g_PrevUserCallbackMousebutton = glfwSetMouseButtonCallback(window, ImGui_ImplGlfw_MouseButtonCallback);
        g_PrevUserCallbackScroll = glfwSetScrollCallback(window, ImGui_ImplGlfw_ScrollCallback);
        g_PrevUserCallbackKey = glfwSetKeyCallback(window, ImGui_ImplGlfw_KeyCallback);
        g_PrevUserCallbackChar = glfwSetCharCallback(window, ImGui_ImplGlfw_CharCallback);
    }

    g_ClientApi = client_api;
    return true;
}

bool ImGui_ImplGlfw_InitForOpenGL(GLFWwindow* window, bool install_callbacks)
{
    return ImGui_ImplGlfw_Init(window, install_callbacks, GlfwClientApi_OpenGL);
}

bool ImGui_ImplGlfw_InitForVulkan(GLFWwindow* window, bool install_callbacks)
{
    return ImGui_ImplGlfw_Init(window, install_callbacks, GlfwClientApi_Vulkan);
}

bool ImGui_ImplGlfw_InitForOther(GLFWwindow* window, bool install_callbacks)
{
    return ImGui_ImplGlfw_Init(window, install_callbacks, GlfwClientApi_Unknown);
}

void ImGui_ImplGlfw_Shutdown()
{
    if (g_InstalledCallbacks)
    {
        glfwSetMouseButtonCallback(g_Window, g_PrevUserCallbackMousebutton);
        glfwSetScrollCallback(g_Window, g_PrevUserCallbackScroll);
        glfwSetKeyCallback(g_Window, g_PrevUserCallbackKey);
        glfwSetCharCallback(g_Window, g_PrevUserCallbackChar);
        g_InstalledCallbacks = false;
    }

    for (ImGuiMouseCursor cursor_n = 0; cursor_n < ImGuiMouseCursor_COUNT; cursor_n++)
    {
        glfwDestroyCursor(g_MouseCursors[cursor_n]);
        g_MouseCursors[cursor_n] = NULL;
    }
    g_ClientApi = GlfwClientApi_Unknown;
}

static void ImGui_ImplGlfw_UpdateMousePosAndButtons()
{
    // Update buttons
    ImGuiIO& io = ImGui::GetIO();
    for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
    {
        // If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
        io.MouseDown[i] = g_MouseJustPressed[i] || glfwGetMouseButton(g_Window, i) != 0;
        g_MouseJustPressed[i] = false;
    }

    // Update mouse position
    const ImVec2 mouse_pos_backup = io.MousePos;
    io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
#ifdef __EMSCRIPTEN__
    const bool focused = true; // Emscripten
#else
    const bool focused = glfwGetWindowAttrib(g_Window, GLFW_FOCUSED) != 0;
#endif
    if (focused)
    {
        if (io.WantSetMousePos)
        {
            glfwSetCursorPos(g_Window, (double)mouse_pos_backup.x, (double)mouse_pos_backup.y);
        }
        else
        {
            double mouse_x, mouse_y;
            glfwGetCursorPos(g_Window, &mouse_x, &mouse_y);
			if(imgui_changedViewport){
				io.MousePos = ImVec2(((float)mouse_x-imgui_viewport[0])*imgui_viewport[2], ((float)mouse_y-imgui_viewport[1])*imgui_viewport[3]);
			}else{
				io.MousePos = ImVec2((float)mouse_x, (float)mouse_y);
			}
        }
    }
	imgui_viewport[0] = 0;
	imgui_viewport[1] = 0;
	imgui_viewport[2] = 1;
	imgui_viewport[3] = 1;
}

static void ImGui_ImplGlfw_UpdateMouseCursor()
{
    ImGuiIO& io = ImGui::GetIO();
    if ((io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) || glfwGetInputMode(g_Window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
        return;

    ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
    if (imgui_cursor == ImGuiMouseCursor_None || io.MouseDrawCursor)
    {
        // Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
        glfwSetInputMode(g_Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }
    else
    {
        // Show OS mouse cursor
        // FIXME-PLATFORM: Unfocused windows seems to fail changing the mouse cursor with GLFW 3.2, but 3.3 works here.
        glfwSetCursor(g_Window, g_MouseCursors[imgui_cursor] ? g_MouseCursors[imgui_cursor] : g_MouseCursors[ImGuiMouseCursor_Arrow]);
        glfwSetInputMode(g_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

void insertAnalog(int imguiKey0, int imguiKey1, int glfwAxis, ImGuiIO& io, const float* axes){
	float value = axes[glfwAxis];
	if(value < -0.1){
		io.AddKeyAnalogEvent(imguiKey0, true, std::abs(value)); 
		io.AddKeyAnalogEvent(imguiKey1, false, 0.);
	}else if(value >= 0.1){
		io.AddKeyAnalogEvent(imguiKey0, false, 0.); 
		io.AddKeyAnalogEvent(imguiKey1, true, value);
	}else{
		io.AddKeyAnalogEvent(imguiKey0, false, 0.); 
		io.AddKeyAnalogEvent(imguiKey1, false, 0.);
	}
}

static void ImGui_ImplGlfw_UpdateGamepads()
{
    ImGuiIO& io = ImGui::GetIO();
//    memset(io.NavInputs, 0, sizeof(io.NavInputs));
    if ((io.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad) == 0)
        return;
    GLFWgamepadstate gamepad;
    if (!glfwGetGamepadState(GLFW_JOYSTICK_1, &gamepad))
        return;
    // Update gamepad inputs
    #define MAP_BUTTON(IMGUI_NO, BUTTON_NO)       { io.AddKeyEvent(IMGUI_NO, buttons[BUTTON_NO]); }
    #define MAP_ANALOG(NAV_NO, AXIS_NO, V0, V1) { float v = (axes_count > AXIS_NO) ? axes[AXIS_NO] : V0; v = (v - V0) / (V1 - V0); if (v > 1.0f) v = 1.0f; if (io.NavInputs[NAV_NO] < v) io.NavInputs[NAV_NO] = v; }
    int axes_count = 0, buttons_count = 0;
    const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axes_count);
    const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttons_count);
    MAP_BUTTON(ImGuiKey_GamepadStart,  			GLFW_GAMEPAD_BUTTON_START);     
    MAP_BUTTON(ImGuiKey_GamepadBack,  			GLFW_GAMEPAD_BUTTON_BACK);     
    MAP_BUTTON(ImGuiKey_GamepadFaceDown,  		GLFW_GAMEPAD_BUTTON_A);     
    MAP_BUTTON(ImGuiKey_GamepadFaceRight,  		GLFW_GAMEPAD_BUTTON_B);    
    MAP_BUTTON(ImGuiKey_GamepadFaceLeft,        GLFW_GAMEPAD_BUTTON_X);   
    MAP_BUTTON(ImGuiKey_GamepadFaceUp,      	GLFW_GAMEPAD_BUTTON_Y);  
    MAP_BUTTON(ImGuiKey_GamepadDpadLeft,   		GLFW_GAMEPAD_BUTTON_DPAD_LEFT);    
    MAP_BUTTON(ImGuiKey_GamepadDpadRight,  		GLFW_GAMEPAD_BUTTON_DPAD_RIGHT);  
    MAP_BUTTON(ImGuiKey_GamepadDpadUp,     		GLFW_GAMEPAD_BUTTON_DPAD_UP);    
    MAP_BUTTON(ImGuiKey_GamepadDpadDown,   		GLFW_GAMEPAD_BUTTON_DPAD_DOWN);  
    MAP_BUTTON(ImGuiKey_GamepadL1,  			GLFW_GAMEPAD_BUTTON_LEFT_BUMPER);
    MAP_BUTTON(ImGuiKey_GamepadR1,  			GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER);
    MAP_BUTTON(ImGuiKey_GamepadL3, 				GLFW_GAMEPAD_BUTTON_LEFT_THUMB );
    MAP_BUTTON(ImGuiKey_GamepadR3, 				GLFW_GAMEPAD_BUTTON_RIGHT_THUMB);
	insertAnalog(ImGuiKey_GamepadLStickLeft, ImGuiKey_GamepadLStickRight, 	GLFW_GAMEPAD_AXIS_LEFT_X, io, axes);
	insertAnalog(ImGuiKey_GamepadLStickUp,ImGuiKey_GamepadLStickDown,  		GLFW_GAMEPAD_AXIS_LEFT_Y, io, axes);
	insertAnalog(ImGuiKey_GamepadRStickLeft, ImGuiKey_GamepadRStickRight, 	GLFW_GAMEPAD_AXIS_RIGHT_X, io, axes);
	insertAnalog(ImGuiKey_GamepadRStickUp,ImGuiKey_GamepadRStickDown, 	 	GLFW_GAMEPAD_AXIS_RIGHT_Y, io, axes);

    #undef MAP_BUTTON
    #undef MAP_ANALOG
    if (axes_count > 0 && buttons_count > 0)
        io.BackendFlags |= ImGuiBackendFlags_HasGamepad;
    else
        io.BackendFlags &= ~ImGuiBackendFlags_HasGamepad;
}

void ImGui_ImplGlfw_NewFrame()
{
    ImGuiIO& io = ImGui::GetIO();
    IM_ASSERT(io.Fonts->IsBuilt() && "Font atlas not built! It is generally built by the renderer backend. Missing call to renderer _NewFrame() function? e.g. ImGui_ImplOpenGL3_NewFrame().");

    // Setup display size (every frame to accommodate for window resizing)
    int w, h;
    int display_w, display_h;
    glfwGetWindowSize(g_Window, &w, &h);
    glfwGetFramebufferSize(g_Window, &display_w, &display_h);
//	float imgui_viewport[4] = {0,0,1.,1.};
//	int imgui_changedViewport = 0;

	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	display_w = viewport[2];
	w = viewport[2];
	display_h = viewport[3];
	h = viewport[3];

    io.DisplaySize = ImVec2((float)w, (float)h);
    if (w > 0 && h > 0)
        io.DisplayFramebufferScale = ImVec2((float)display_w / w, (float)display_h / h);

    // Setup time step
    double current_time = glfwGetTime();
    io.DeltaTime = g_Time > 0.0 ? (float)(current_time - g_Time) : (float)(1.0f / 60.0f);
    g_Time = current_time;

    ImGui_ImplGlfw_UpdateMousePosAndButtons();
    ImGui_ImplGlfw_UpdateMouseCursor();

	//Update keys

/*	io.AddKeyEvent(ImGuiKey_LeftCtrl, glfwGetKey(g_Window, GLFW_KEY_LEFT_CONTROL));
	io.AddKeyEvent(ImGuiKey_RightCtrl, glfwGetKey(g_Window, GLFW_KEY_RIGHT_CONTROL));
	io.AddKeyEvent(ImGuiKey_LeftShift, glfwGetKey(g_Window, GLFW_KEY_LEFT_SHIFT));
	io.AddKeyEvent(ImGuiKey_RightShift, glfwGetKey(g_Window, GLFW_KEY_RIGHT_SHIFT));
	io.AddKeyEvent(ImGuiKey_LeftAlt, glfwGetKey(g_Window, GLFW_KEY_LEFT_ALT));
	io.AddKeyEvent(ImGuiKey_RightAlt, glfwGetKey(g_Window, GLFW_KEY_RIGHT_ALT));
	io.AddKeyEvent(ImGuiKey_LeftSuper, glfwGetKey(g_Window, GLFW_KEY_LEFT_SUPER));
	io.AddKeyEvent(ImGuiKey_RightSuper, glfwGetKey(g_Window, GLFW_KEY_RIGHT_SUPER));
	io.AddKeyEvent(ImGuiKey_Space, glfwGetKey(g_Window, GLFW_KEY_SPACE));
	io.AddKeyEvent(ImGuiKey_Enter, glfwGetKey(g_Window, GLFW_KEY_ENTER));*/

	io.KeyCtrl = glfwGetKey(g_Window, GLFW_KEY_LEFT_CONTROL) | glfwGetKey(g_Window, GLFW_KEY_RIGHT_CONTROL);
	io.KeyShift = glfwGetKey(g_Window, GLFW_KEY_LEFT_SHIFT) | glfwGetKey(g_Window, GLFW_KEY_RIGHT_SHIFT);

    // Update game controllers (if enabled and available)
    ImGui_ImplGlfw_UpdateGamepads();
	imgui_changedViewport = 0;
}
