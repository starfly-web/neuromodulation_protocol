# Neuromodulation Protocol v0.1 - Complete Implementation

## Overview

This folder contains a complete implementation of a bimodal neuromodulation system based on spike-timing-dependent plasticity (STDP) research for tinnitus treatment. The system delivers precisely timed audio and TENS stimuli with a fixed 10ms delay to bias synaptic plasticity toward long-term depression (LTD).

## Quick Start

### Hardware Requirements
- ESP32-WROOM-32E development board
- Audio output (DAC/I2S) for narrowband sound
- TENS driver circuit for trigeminal stimulation
- Surface electrodes for TENS delivery

### Build Instructions
1. Install ESP-IDF v5.x
2. Configure project with `idf.py menuconfig`
3. Build with `idf.py build`
4. Flash with `idf.py flash`

### Basic Usage
```c
// Configure system
set_audio_frequency(7000);      // 7 kHz tone
set_session_duration(30);       // 30 minute session
set_tens_delay(10);             // 10 ms delay

// Start neuromodulation
start_session();

// Monitor and stop when needed
stop_session();
```

## File Structure

### Core Implementation
- **`README.md`** - Main specification and requirements
- **`neuromod.h`** - Public API declarations
- **`neuromod.c`** - Session engine and timing logic
- **`main.c`** - Application entry point and example usage

### Hardware Abstraction
- **`audio_driver.h`** - Audio hardware interface
- **`audio_driver.c`** - Audio hardware implementation
- **`tens_driver.h`** - TENS hardware interface  
- **`tens_driver.c`** - TENS hardware implementation

### Documentation
- **`TENS_explanation.md`** - Technical explanation of TENS
- **`API_explanation.md`** - Public API documentation and design
- **`GUI_integration.md`** - GUI integration options and strategies
- **`PROJECT_STRUCTURE.md`** - Complete project architecture
- **`TIMING_DIAGRAM.md`** - Timing requirements and state machine

## Key Features

### Precise Timing Engine
- **10ms fixed delay** between audio and TENS stimuli
- **≤1ms jitter** for reliable STDP induction
- **Microsecond resolution** timing
- **No cumulative drift** over long sessions

### Safety Features
- **Hardware current limiting** for TENS output
- **Watchdog timer** for firmware lockup protection
- **Fault detection** and safe shutdown
- **Manual reset required** after faults

### Configurable Parameters
- **Audio frequency**: 1000-10000 Hz range
- **Session duration**: 1-60 minutes
- **TENS delay**: Configurable (default 10ms)
- **Event period**: Configurable timing between pairs

## Technical Specifications

### Timing Requirements
- **Audio → TENS delay**: 10ms ± 1ms
- **Event jitter**: < 1ms RMS
- **Session accuracy**: ±1 second over 60 minutes
- **Timer resolution**: 1 microsecond

### Hardware Interface
- **Audio**: DAC/I2S output, low impedance (~10Ω)
- **TENS**: Constant current, biphasic pulses
- **Control**: GPIO for enable/disable
- **Safety**: Hardware interlocks and current limiting

### Software Architecture
- **RTOS**: FreeRTOS with task-based timers
- **Memory**: < 10KB dynamic allocation
- **CPU**: < 5% utilization during active session
- **Power**: Optimized for battery operation

## Development Roadmap

### v0.1 (Current)
- ✅ Fixed-delay, open-loop implementation
- ✅ Precise timing engine
- ✅ Safety features
- ✅ Hardware abstraction

### v0.2 (Planned)
- 🔄 Adaptive timing based on user response
- 🔄 GUI integration (web or touchscreen)
- 🔄 Parameter optimization algorithms
- 🔄 Enhanced safety monitoring

### v0.3 (Future)
- 🔄 Closed-loop feedback system
- 🔄 Personalization algorithms
- 🔄 Long-term carryover effect tracking
- 🔄 Multi-channel audio support

## Testing and Validation

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

## Contributing

This is a research-oriented implementation. Contributions are welcome for:

- Hardware driver improvements
- GUI interface development
- Testing framework enhancement
- Documentation updates

## License

This implementation is provided for research and educational purposes. Consult with medical professionals before any clinical use.

## Support

For questions about the implementation:
1. Check the documentation files
2. Review the code comments
3. Examine the timing diagrams
4. Test with the provided examples

For medical or clinical questions, consult appropriate healthcare professionals.