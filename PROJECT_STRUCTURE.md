# Firmware File Structure Diagram

## Directory Layout

```
📁 neuromodulation_protocol_v0.1/
│
├── README.md                    # Main specification document
├── PROJECT_STRUCTURE.md         # This file
├── TENS_explanation.md          # TENS technical explanation
├── API_explanation.md           # Public API documentation
├── GUI_integration.md           # GUI integration options
│
├── main.c                       # Application entry point
├── neuromod.h                   # Public API declarations
├── neuromod.c                   # Session engine implementation
│
├── audio_driver.h               # Audio hardware interface
├── audio_driver.c               # Audio hardware implementation
│
├── tens_driver.h                # TENS hardware interface
└── tens_driver.c                # TENS hardware implementation
```

## Code Flow Architecture

```
[User GUI] ←→ [Communication Layer]
                    ↓
              [Public API]
                    ↓
           [Session State Machine]
                    ↓
        ┌───────────┴───────────┐
        ↓                       ↓
[Audio Driver]           [TENS Driver]
        ↓                       ↓
   [DAC/I2S]           [Constant Current]
        ↓                       ↓
   [Headphones]           [Electrodes]
```

## Module Responsibilities

### Public API Layer (`neuromod.h`/`neuromod.c`)
- **Purpose**: Interface between application and timing engine
- **Functions**: `set_audio_frequency()`, `start_session()`, etc.
- **Constraints**: Thread-safe, parameter validation
- **Dependencies**: Hardware drivers, FreeRTOS

### Session Engine (`neuromod.c`)
- **Purpose**: Core timing and state management
- **Features**: Timer-driven state machine, event scheduling
- **Constraints**: Jitter ≤ 1 ms, deterministic timing
- **Dependencies**: ESP-IDF timers, hardware drivers

### Hardware Abstraction (`audio_driver.c`, `tens_driver.c`)
- **Purpose**: Low-level hardware control
- **Features**: Initialization, configuration, safety checks
- **Constraints**: Hardware-specific, safety-critical
- **Dependencies**: ESP-IDF peripherals

### Application Layer (`main.c`)
- **Purpose**: System initialization and user interface
- **Features**: Configuration, monitoring, error handling
- **Constraints**: Non-blocking, responsive
- **Dependencies**: Public API, communication interfaces

## Build Configuration

### ESP-IDF Requirements
- **Target**: ESP32-WROOM-32E
- **Framework**: ESP-IDF v5.x
- **RTOS**: FreeRTOS
- **Components**: Timer, logging, hardware drivers

### Memory Layout
- **Flash**: 4MB minimum
- **PSRAM**: Recommended for audio buffering
- **Stack**: 8KB per task minimum

### Safety Considerations
- Watchdog timer for firmware lockup protection
- Hardware current limiting for TENS output
- Fault detection and safe shutdown procedures
- No dynamic memory allocation in timing-critical paths

## Testing Strategy

### Unit Tests
- API function validation
- Timer accuracy verification
- State machine transition testing

### Integration Tests
- Audio-TENS timing verification
- Session duration accuracy
- Fault recovery testing

### Hardware Tests
- Output impedance verification
- Current limiting validation
- EMI/EMC compliance testing