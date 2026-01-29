# What is a Public API?

## Definition

A public API is simply the set of functions that your firmware intentionally exposes so that other parts of the system (or other developers) can use them without needing to know anything about the internal implementation.

Think of it as the official doorway into your firmware logic.

## What "public API" means in your project

In your neuromodulation firmware, the public API is the list of functions you decided should be callable from outside the timing engine:

- `set_audio_frequency(f_hz)`
- `set_session_duration(minutes)`
- `set_tens_delay(ms)`
- `start_session()`
- `stop_session()`
- `fault_handler()`

These functions are:

- Declared in `neuromod.h`
- Implemented in `neuromod.c`
- Called from `main.c` or any UI/communication layer you add later

Everything else (timers, state machine, hardware drivers) stays private and hidden.

## Why a public API matters

A good public API:

- Defines the boundaries of your module
- Protects internal logic from accidental misuse
- Keeps the code modular
- Lets you change internals later without breaking the rest of the system
- Makes the firmware predictable and testable

In other words, the public API is the contract between your neuromodulation engine and the rest of the firmware.

## Public API vs Internal Logic

| Layer | What it contains | Visible to outside? |
|-------|------------------|-------------------|
| **Public API** (`neuromod.h`) | The functions you want others to call | Yes |
| **Session engine** (`neuromod.c`) | Timers, state machine, scheduling | No |
| **Hardware drivers** (`audio_driver.c`, `tens_driver.c`) | DAC, I2S, GPIO, current driver | No |
| **Application layer** (`main.c`) | Calls the public API | Yes |

Only the top layer is public.

## Simple analogy

If your firmware were a restaurant:

- **Public API** = the menu
- **Internal logic** = the kitchen
- **Hardware drivers** = the stove, fridge, tools
- **main.c** = the waiter placing orders

Customers (other code modules) only see the menu, not the kitchen.

## API Versioning Strategy

- **v0.1** – Fixed-delay, open-loop implementation
- **v0.2** – Add adaptive timing, UI integration
- **v0.3** – Add closed-loop feedback, personalization

Each version maintains backward compatibility where possible.

## Documentation Standards

Each public function should include:

1. **Purpose** - What does this function do?
2. **Parameters** - What inputs does it accept?
3. **Returns** - What does it return?
4. **Side effects** - What state does it change?
5. **Error conditions** - When might it fail?

Example:
```c
/**
 * @brief Set the audio frequency for neuromodulation
 * @param f_hz Frequency in Hertz (1000-10000 typical range)
 * @return void
 * @side_effects Updates global audio frequency variable
 */
void set_audio_frequency(uint32_t f_hz);