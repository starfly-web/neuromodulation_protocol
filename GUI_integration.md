# GUI Integration Options

## Overview

This document explains how a GUI can control your firmware's public API - meaning: "How do I let a user press buttons, move sliders, or pick values, and have those actions call functions like `set_audio_frequency()` or `start_session()`?"

There are three common patterns for ESP32 GUI input, and each one connects to your API in a slightly different way.

## 1. On-device GUI (LVGL + Touchscreen)

This is the classic embedded approach.

### How it works
- You run LVGL on the ESP32
- You draw buttons, sliders, dropdowns
- Each widget has a callback
- Inside the callback, you call your public API

### Example (LVGL button → firmware API)
```c
static void start_btn_cb(lv_event_t * e) {
    start_session(); // your public API
}

void build_gui(void) {
    lv_obj_t * btn = lv_btn_create(lv_scr_act());
    lv_obj_add_event_cb(btn, start_btn_cb, LV_EVENT_CLICKED, NULL);
}
```

### What this gives you
- A physical touchscreen device
- Direct control of your neuromodulation engine
- No networking required

## 2. Web GUI (ESP32 hosts a webpage)

This is the most popular modern approach because it's easy and looks great.

### How it works
- ESP32 runs a small web server
- The GUI is HTML/CSS/JS
- User interacts via phone or laptop
- JavaScript sends commands to ESP32 (HTTP or WebSocket)
- ESP32 handlers call your public API

### Example (HTTP endpoint → firmware API)
```c
esp_err_t api_set_freq_handler(httpd_req_t *req) {
    int freq = parse_int_from_request(req);
    set_audio_frequency(freq);
    httpd_resp_sendstr(req, "OK");
    return ESP_OK;
}
```

### What this gives you
- Beautiful UI
- No display hardware required
- Works on any device with Wi-Fi

## 3. Serial / USB GUI (PC app controlling ESP32)

This is great for development or research tools.

### How it works
- ESP32 listens on UART
- PC app (Python, C#, etc.) sends commands
- ESP32 parses commands and calls your API

### Example (UART command → firmware API)
```c
if (strcmp(cmd, "START") == 0) {
    start_session();
} else if (strncmp(cmd, "FREQ ", 5) == 0) {
    int f = atoi(cmd + 5);
    set_audio_frequency(f);
}
```

### What this gives you
- Fast prototyping
- Easy debugging
- No embedded GUI complexity

## How the GUI connects to your public API

```
[User GUI]
    ↓
[GUI Event Handler]
    ↓
[Public API]
    ↓
[Neuromod Engine (timers, state machine)]
    ↓
[Audio Driver]    [TENS Driver]
```

The GUI never touches hardware. It only calls your public API, which is exactly why that API exists.

## Which GUI style fits your project best?

| Goal | Best GUI |
|------|----------|
| Portable standalone device | LVGL touchscreen |
| Easy development + great UX | Web GUI |
| Research tool / lab use | PC GUI over UART |

## Implementation Strategy

### Phase 1: UART Command Interface
- Quick to implement
- Great for testing
- Easy to debug
- Use a simple protocol like:
  ```
  FREQ 7000
  DURATION 30
  DELAY 10
  START
  STOP
  ```

### Phase 2: Web Interface
- Add HTTP server
- Create simple HTML dashboard
- Use AJAX for real-time updates
- Add WebSocket for live status

### Phase 3: Touchscreen Interface
- Add LVGL library
- Design touch-friendly controls
- Optimize for embedded display

## Command Protocol Example

```
# Set parameters
FREQ <frequency_hz>
DURATION <minutes>
DELAY <milliseconds>

# Control session
START
STOP
STATUS

# System commands
RESET
FAULT_RESET
```

## Next Steps

1. Choose your GUI approach based on your use case
2. Implement the communication layer
3. Add validation and error handling
4. Create the user interface
5. Test the integration thoroughly

Would you like me to generate a complete implementation for any of these approaches?