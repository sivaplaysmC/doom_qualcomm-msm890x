#include "input.h"

#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include <linux/input-event-codes.h>
#include <linux/input.h>

static doom_key_t conver_linux_to_doom(struct input_event *ev);
static inline void input_process_for_fd(int fd);

#ifndef KEYPAD_INPUTDEV
#define KEYPAD_INPUTDEV "/dev/input/event0"
#endif /* ifndef KEYPAD_INPUTDEV */

#ifndef CALLREJECT_INPUTDEV
#define CALLREJECT_INPUTDEV "/dev/input/event1"
#endif /* ifndef CALLREJECT_INPUTDEV */

int fd_input0 = -1;
int fd_input1 = -1;

int input_init() {
  fd_input0 = open(KEYPAD_INPUTDEV, O_RDONLY | O_NONBLOCK);
  assert(fd_input0 >= 0 && "Failed to open KEYPAD_INPUTDEV");

  fd_input1 = open(CALLREJECT_INPUTDEV, O_RDONLY | O_NONBLOCK);
  assert(fd_input1 >= 0 && "Failed to open CALLREJECT_INPUTDEV");

  return 0;
}

void input_process() {
  input_process_for_fd(fd_input0);
  input_process_for_fd(fd_input1);
}

static inline void input_process_for_fd(int fd) {
  struct input_event ev;
  for (;;) {
    int res = read(fd, &ev, sizeof(ev));

    if (res != sizeof(ev)) // maybe we hit EAGAIN
      break;

    if (ev.type != EV_KEY)
      continue;

    doom_key_t key = conver_linux_to_doom(&ev);
    if (ev.value == 1) {
      doom_key_down(key);
    } else if (ev.value == 0) {
      doom_key_up(key);
    }
  }
}

// WARN: Slop gen'd.
// https://chatgpt.com/share/69c80e8e-98c0-8322-a615-30407feb39e9
static doom_key_t conver_linux_to_doom(struct input_event *ev) {
  switch (ev->code) {

  // D-Pad movement
  case KEY_DOWN:
    return DOOM_KEY_DOWN_ARROW;
  case KEY_UP:
    return DOOM_KEY_UP_ARROW;
  case KEY_LEFT:
    return DOOM_KEY_LEFT_ARROW;
  case KEY_RIGHT:
    return DOOM_KEY_RIGHT_ARROW;

  // Weapon select (1–7)
  case KEY_1:
    return DOOM_KEY_1;
  case KEY_2:
    return DOOM_KEY_2;
  case KEY_3:
    return DOOM_KEY_3;
  case KEY_4:
    return DOOM_KEY_4;
  case KEY_5:
    return DOOM_KEY_5;
  case KEY_6:
    return DOOM_KEY_6;
  case KEY_7:
    return DOOM_KEY_7;

  // Use / open door
  case KEY_0:
    return DOOM_KEY_SPACE;

  // Fire
  case KEY_SEND: // CALL ACCEPT
    return DOOM_KEY_CTRL;

  // Strafe
  case KEY_BACK:         // BACK
    return DOOM_KEY_ALT; // commonly used as strafe modifier

  case KEY_MENU: // MENU
    return DOOM_KEY_ALT;

  // Run
  case KEY_NUMERIC_POUND: // #
    return DOOM_KEY_SHIFT;

  // Map
  case KEY_NUMERIC_STAR: // *
    return DOOM_KEY_TAB;

  // Confirm / Enter
  case KEY_OK:
  case KEY_ENTER:
    return DOOM_KEY_ENTER;

  default:
    break;
  }

  printf("Unknown input event received. Fix your code. code=%d value=%d, "
         "type=%d\n",
         ev->code, ev->value, ev->type);
  return DOOM_KEY_UNKNOWN;
}

int input_deinit() {
  close(fd_input0);
  close(fd_input1);

  fd_input0 = -1;
  fd_input1 = -1;
  return 0;
}

// vim:sw=2
