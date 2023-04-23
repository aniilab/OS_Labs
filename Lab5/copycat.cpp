#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <X11/XKBlib.h>
#include <X11/Xlib.h>

using namespace std;

string keyCodeToSymbol(Display *display, int keyCode) {
    XkbStateRec xkbState;
    XkbGetState(display, XkbUseCoreKbd, &xkbState);

    XkbDescPtr xkbDesc = XkbGetKeyboard(display, XkbAllComponentsMask, XkbUseCoreKbd);
    KeySym keySym = XkbKeycodeToKeysym(display, keyCode + 8, xkbState.base_group, 0);

    if (keySym == NoSymbol) {
        return "Unknown";
    }

    char symbol[32];
    XkbTranslateKeySym(display, &keySym, 0, symbol, sizeof(symbol), NULL);

    XkbFreeKeyboard(xkbDesc, XkbAllComponentsMask, True);

    return string(symbol);
}

int main() {
    const char *dev = "/dev/input/by-path/platform-i8042-serio-0-event-kbd";
    int fd = open(dev, O_RDONLY);

    if (fd == -1) {
        cerr << "Cannot open " << dev << endl;
        return 1;
    }

    cout << "I am copycat and I am going to copy everything you do ( •̀ᴗ•́ )و ̑̑" << endl;

    Display *display = XOpenDisplay(NULL);
    if (display == NULL) {
        cerr << "Cannot open X display" << endl;
        return 1;
    }

    struct input_event ev;
    while (true) {
        read(fd, &ev, sizeof(ev));

        if (ev.type == EV_KEY && ev.value == 1) {
            string symbol = keyCodeToSymbol(display, ev.code);
            cout << "Have you pressed " << symbol << endl;
        }
    }

    XCloseDisplay(display);
    close(fd);
    return 0;
}

