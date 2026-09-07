# Risk Analysis — Educational Simulation

This document demonstrates the style of reasoning used in safety-oriented engineering. It is **not** a medical-device risk analysis.

| Hazard | Possible cause | Control in this project |
|---|---|---|
| Event loss | Queue full | Count and report dropped events |
| Event corruption | Concurrency bug | SPSC ownership model + tests |
| Silent performance regression | Code change | Performance benchmark |
| Invalid processing result | Bad calibration | Unit tests + test fixtures |

A real medical-device project would require a formal risk-management process and applicable regulatory documentation.
