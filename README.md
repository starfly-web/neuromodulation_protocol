# Neuromodulation Protocol – Current Working Specs (v0.1)

## Purpose

Define the timing, signal structure, and control logic for a bimodal neuromodulation system based on publicly available research on spike-timing-dependent plasticity (STDP) in the dorsal cochlear nucleus.

**This document describes what the firmware must do, not how it is clinically used.**

## Core Principle (Non-Negotiable)

- **Sound stimulus FIRST**
- **Trigeminal (TENS) stimulus SECOND**
- **Fixed delay: +10 ms (±1 ms tolerance)**

This ordering is critical to bias synaptic plasticity toward long-term depression (LTD) rather than LTP.

## Audio Stimulus (Sound Path)

### Signal Type
- Narrowband or frequency-targeted audio centered near the user's tinnitus percept
- Can be tonal, multi-tone, or shaped noise
- No notching required in this implementation

### Frequency
- User-defined center frequency (initial working example: ~7 kHz, adjustable)

### Level
- Low intensity
- Clearly audible but not loud
- No masking
- No discomfort

### Duration
- Continuous or pulsed, depending on session mode
- Typical sessions: 30–60 minutes

### Audio Output Constraints
- Low output impedance preferred (≈ 10 Ω target)
- Designed for IEM use
- No EQ, compression, or enhancement

## Somatosensory / TENS Stimulus

### Target
- Trigeminal-innervated facial / oral region (hardware-dependent)

### Waveform
- Biphasic, charge-balanced pulses
- Burst-based delivery

### Current
- Constant-current controlled
- Amplitude limited in hardware
- Software sets timing, burst structure, and enable/disable only

### Burst Parameters (initial targets)
- Pulse width: hardware-defined
- Pulse frequency within burst: hardware-defined
- Burst duration: firmware-controlled
- Burst repetition rate: firmware-controlled

### Critical Timing
- TENS burst onset occurs ~10 ms after corresponding audio event

## Timing Model (Firmware-Critical)

Firmware must support:
- Precise event scheduling
- Deterministic delay between audio event and TENS trigger
- Jitter ≤ 1 ms
- No cumulative drift over long sessions

### Recommended implementation
- Timer-driven state machine
- Hardware timers (not software delays)
- Pre-computed event schedule preferred

## Session Structure

### Session Length
- Target: 30 minutes per session
- Max: 60 minutes

### Session Frequency
- Once daily (research-aligned)

### State Flow
1. Initialize hardware
2. Calibrate output limits
3. Start audio stream
4. Schedule TENS bursts with fixed offset
5. Monitor for faults
6. Graceful shutdown

## Safety & Guardrails (Engineering Level)

- Hard current limit in analog stage
- Watchdog timeout for firmware lockup
- Immediate shutdown on fault
- No dynamic gain changes mid-session
- No adaptive learning logic in v0.1

## Firmware Responsibilities (What Coders Can Build Now)

### Firmware should expose:
- `set_audio_frequency(f_hz)`
- `set_session_duration(minutes)`
- `set_tens_delay(ms)` (default = 10 ms)
- `start_session()`
- `stop_session()`
- `fault_handler()`

### Not required in v0.1
- UI polish
- Personalization algorithms
- Closed-loop feedback
- Adaptive timing

## Known Unknowns (Open Research Space)

- Optimal burst structure
- Ideal frequency bandwidth
- Best session cadence for individuals
- Long-term carryover effects

These are intentionally not solved yet.

## Versioning
- v0.1 – Fixed-delay, open-loop implementation
- Future versions will be posted and pinned here