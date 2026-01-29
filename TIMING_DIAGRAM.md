# Timing Diagram and State Machine

## Critical Timing Requirements

### Core Principle
- **Sound stimulus FIRST**
- **Trigeminal (TENS) stimulus SECOND**
- **Fixed delay: +10 ms (±1 ms tolerance)**

This ordering is critical to bias synaptic plasticity toward long-term depression (LTD) rather than LTP.

## Timing Diagram

```
Time (ms)    0         10        500       510       1000      1010
             |         |         |         |         |         |
Audio Event:  ──[START]─────────────────[START]─────────────────[START]──
             │         │         │         │         │         │
TENS Burst:   ────────[START]─────────────────[START]─────────────────[START]──
             │         │         │         │         │         │
             ▲         ▲         ▲         ▲         ▲         ▲
             │         │         │         │         │         │
             │         │         │         │         │         └─ TENS Event N
             │         │         │         │         └─ Audio Event N
             │         │         │         └─ TENS Event N-1
             │         │         └─ Audio Event N-1
             │         └─ TENS Event N-2 (10ms after Audio N-2)
             └─ Audio Event N-2
```

## Event Sequence

### Single Audio-TENS Pair
1. **t=0ms**: Audio event starts
2. **t=10ms**: TENS burst starts (exactly 10ms after audio)
3. **t=10ms+**: TENS burst duration (hardware-defined)
4. **t=500ms**: Next audio event starts (example period)

### Timing Constraints
- **Jitter**: ≤ 1 ms between audio start and TENS start
- **Drift**: No cumulative timing drift over long sessions
- **Precision**: Microsecond resolution timing
- **Determinism**: Fixed delay regardless of system load

## State Machine

```
[INITIALIZING]
       ↓
[CONFIGURED] ←→ [RUNNING] ←→ [FAULT]
       ↓              ↓
    [IDLE] ←───────[STOPPING]
```

### State Transitions

#### Normal Operation
1. **INITIALIZING** → **CONFIGURED**: Hardware initialization complete
2. **CONFIGURED** → **RUNNING**: `start_session()` called
3. **RUNNING** → **STOPPING**: `stop_session()` called or session duration reached
4. **STOPPING** → **IDLE**: All timers stopped, hardware disabled
5. **IDLE** → **CONFIGURED**: Ready for next session

#### Fault Conditions
1. **RUNNING** → **FAULT**: Hardware fault detected
2. **FAULT** → **STOPPING**: Safety shutdown initiated
3. **FAULT** → **CONFIGURED**: Manual fault reset (requires user intervention)

### State Actions

#### CONFIGURED State
- Hardware initialized and calibrated
- Parameters set (frequency, duration, delay)
- Ready to start session
- No active timing events

#### RUNNING State
- Audio events scheduled and executing
- TENS bursts triggered with 10ms delay
- Session timer counting down
- Fault monitoring active
- All timing events active

#### FAULT State
- All outputs disabled immediately
- All timers stopped
- Fault condition logged
- Manual reset required
- Safety interlocks engaged

## Timer Architecture

### Hardware Timers Used
- **Audio Event Timer**: Schedules audio stimulus events
- **TENS Event Timer**: Schedules TENS burst triggers
- **Session End Timer**: Monitors session duration
- **Watchdog Timer**: Monitors firmware health

### Timer Configuration
- **Resolution**: 1 microsecond
- **Accuracy**: ±1 ms over session duration
- **Priority**: High priority to minimize jitter
- **Dispatch**: Task-based (not ISR-based) for determinism

## Implementation Details

### Event Scheduling
```c
// Schedule next audio-TENS pair
static void schedule_next_pair(void) {
    if (g_state != SESSION_STATE_RUNNING) return;
    
    uint64_t period_us = (uint64_t)PAIR_PERIOD_MS * 1000ULL;
    esp_timer_start_once(audio_event_timer, period_us);
}

// Audio event callback
static void audio_timer_cb(void *arg) {
    if (g_state != SESSION_STATE_RUNNING) return;
    
    audio_event_start();
    
    // Schedule TENS burst after fixed delay
    uint64_t delay_us = (uint64_t)g_tens_delay_ms * 1000ULL;
    esp_timer_start_once(tens_event_timer, delay_us);
    
    // Schedule next pair
    schedule_next_pair();
}
```

### Safety Monitoring
- **Watchdog**: 5-second timeout for firmware lockup
- **Current Monitoring**: Hardware current limiting
- **Temperature Monitoring**: Thermal protection
- **Fault Logging**: Persistent fault storage

## Performance Requirements

### Timing Accuracy
- **Audio-TENS Delay**: 10ms ± 1ms
- **Event Period**: Configurable, typical 500ms
- **Session Duration**: Configurable, typical 30 minutes
- **Jitter**: < 1ms RMS

### Resource Usage
- **CPU**: < 5% during active session
- **Memory**: < 10KB dynamic allocation
- **Power**: Optimized for battery operation
- **Thermal**: No active cooling required

## Error Handling

### Timing Errors
- **Missed Events**: Log and continue (don't restart)
- **Timer Failures**: Enter fault state
- **Jitter Exceeded**: Log warning, continue session

### Hardware Errors
- **Current Limit**: Immediate shutdown
- **Temperature**: Gradual shutdown with warning
- **Communication**: Continue with last known good settings