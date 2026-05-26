# ECE3072 Mini Project
## Real-Time Embedded Vision Using Arm – Grove AI V2 and NIOS-based Multitasking System on FPGA

A multitasking embedded system implemented on the **Intel DE10-Lite FPGA** using a **dual-core NIOS II** soft processor architecture, integrated with the **Seeed Grove Vision AI V2** module for real-time embedded vision.

---

## Team Members

| No. | Full Name      | Student ID | Monash Email                                                  |
|-----|----------------|------------|---------------------------------------------------------------|
| 1   | You Qing Liew  | 33590400   | [ylie0025@student.monash.edu](mailto:ylie0025@student.monash.edu) |
| 2   | Melvin         | 33566380   | [mthi0007@student.monash.edu](mailto:mthi0007@student.monash.edu) |
| 3   | Zhi Hang       | 33524513   | [zteh0010@student.monash.edu](mailto:zteh0010@student.monash.edu) |
| 4   | Kenneth Lai    | 32946562   | [klai0028@student.monash.edu](mailto:klai0028@student.monash.edu) |

---

## Hardware Requirements

- Intel **DE10-Lite** FPGA Development Board (MAX 10)
- **Seeed Grove Vision AI V2** module
- USB Blaster cable (typically built into the DE10-Lite)
- Connecting wires for Grove module interface

## Software Requirements

- **Intel Quartus Prime** (Lite Edition recommended) — version with NIOS II support
- **NIOS II Software Build Tools (SBT) for Eclipse**
- USB Blaster drivers installed

> The exact Quartus version compatibility depends on the project files; if you encounter version mismatch warnings on opening, you may need to use the same version the project was developed in or perform a project upgrade when prompted.

---

## Setup and Run Instructions

### Step 1 — Download the Code Base
Clone or download this repository to your local machine.

```bash
git clone <repo-url>
```

---

### Step 2 — Open the Quartus Project
Open the `.qpf` (Quartus Project File) in Intel Quartus Prime.
---

### Step 3 — Compile and Program the DE10-Lite Board
1. In Quartus, run **Processing → Start Compilation** (or press `Ctrl + L`).
2. Once compilation completes successfully, connect the DE10-Lite board via USB.
3. Open **Tools → Programmer**, ensure the USB-Blaster is detected, load the generated `.sof` file, and click **Start** to program the FPGA.

**MAKE SURE USING THE TIME LIMITED SOF** <sofname>_time_limited.sof

<img width="802" height="639" alt="image" src="https://github.com/user-attachments/assets/886f3de9-5846-4a3e-9abc-483fad2c67da" />



---

### Step 4 — Open the NIOS II Software Build Tools for Eclipse
From the top menu in Quartus, select:

**Tools → NIOS II Software Build Tools for Eclipse**

When prompted, select the workspace directory (the `software/` folder of this project is recommended).




---

### Step 5 — Build the Code
In Eclipse, right-click each project in the **Project Explorer** and select **Build Project**. Make sure both the application project and its associated BSP project build without errors.

After building, open **Run → Run Configurations…** and under **NIOS II Hardware**, create **two new run configurations** — one for each NIOS II core.



---

### Step 6 — Configure Each Core
Because this design uses **two NIOS II cores**, two separate run configurations are required. For each configuration:

- Under the **Project** tab, select the corresponding project (one for **Core 0**, one for **Core 1**).
- Under the **Target Connection** tab, ensure each configuration is mapped to the correct CPU on the JTAG chain (use **Refresh Connections** if needed).


---

### Step 7 — Create a Launch Group
To run both cores simultaneously:

1. In **Run Configurations**, right-click **Launch Group → New Configuration**.
2. Add **both** NIOS II Hardware configurations created in Step 6 to this launch group.


---

### Step 8 — Apply
Click **Apply** to save all configurations.

<img width="942" height="694" alt="image" src="https://github.com/user-attachments/assets/eae58930-3805-4856-a1ef-e4faba4a62a8" />


---

### Step 9 — Run
Click **Run** on the Launch Group. Both NIOS II cores should now begin executing their respective programs on the DE10-Lite board.


---

## Troubleshooting

If you encounter any issues during setup or execution, please report them via the repository's **Issues** tab so we can help debug and improve the documentation.

Common things to double-check first:
- The `.sof` was successfully programmed before launching Eclipse.
- Both run configurations point to the **correct CPU** on the JTAG chain.
- The BSP has been regenerated if you modified the hardware (`.sopcinfo`) since the last build.
- The USB-Blaster is not being held by another tool (e.g., Quartus Programmer) when Eclipse tries to download.

---

## License

This project is developed for academic purposes as part of **ECE3072** at Monash University.
