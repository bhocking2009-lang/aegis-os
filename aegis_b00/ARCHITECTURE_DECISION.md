# Aegis Architectural Decision — Heterogeneous AI Compute
Date: 2026-10-12
Decision: Aegis is intended as a heterogeneous compute environment for AI, not a CPU-centered operating system with GPU acceleration added afterward.
- CPU: control, interrupts, security, device coordination, latency-sensitive serial tasks.
- GPU: first-class compute scheduling, inference, parallel execution; protect resident model allocations and avoid unnecessary eviction.
- VRAM: explicitly managed scarce resource; system RAM and NVMe support staging, state, checkpoints.
- RX 6800 XT (16 GB): development target for hardware discovery and future GPU runtime; do not assume a working driver or compute stack.
- B00 scope remains read-only UEFI boot identity/firmware discovery. GPU initialization, memory management and workload scheduling are later gated milestones.
- Never modify target Windows partitions, ESP or boot entries without explicit approval.
Status: Architectural direction accepted; not an implemented runtime.
