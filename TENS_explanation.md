# What is TENS?

## Technical Definition

TENS is short for **Transcutaneous Electrical Nerve Stimulation**. It's a general engineering term for delivering small, controlled electrical pulses through the skin to stimulate peripheral nerves.

## What TENS means in engineering terms

- **Transcutaneous** → through the skin
- **Electrical** → using controlled current pulses
- **Nerve Stimulation** → targeting peripheral nerves (like branches of the trigeminal system)

## What a TENS system does (technical perspective)

- Generates biphasic, charge-balanced pulses
- Uses a constant-current driver so the amplitude stays stable
- Delivers pulses through surface electrodes
- Lets firmware control timing, burst structure, and enable/disable
- Keeps safety limits enforced in hardware (current caps, compliance voltage)

## Why it appears in your neuromodulation protocol

In your design, TENS is the somatosensory stimulus that must occur:

- **After** the audio event
- **With a fixed +10 ms delay**
- **With low jitter**
- **In burst form** rather than continuous stimulation

This aligns with the timing rules you outlined for STDP-based pairing.

## What TENS is NOT

- It's not a shock
- It's not high-voltage
- It's not invasive
- It's not a medical diagnosis or treatment description here — just a hardware/firmware component your system controls

## Electrical Block Diagram (Conceptual)

```
[Microcontroller] → [Digital-to-Analog/PWM] → [Constant Current Driver] → [Electrodes]
       ↓                    ↓                      ↓                    ↓
   Timing Logic        Pulse Generation      Current Control      Skin Interface
```

## Integration with Neuromodulation Engine

The TENS driver integrates with your firmware through:

- `tens_hw_init()` - Initialize hardware
- `tens_hw_start_burst()` - Trigger TENS burst
- `tens_hw_stop_burst()` - End TENS burst
- Hardware safety interlocks for current limiting