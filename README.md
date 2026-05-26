<img width="1600" height="900" alt="de10_lite_wiring" src="https://github.com/user-attachments/assets/e2bdf691-9278-400a-bf7c-994f79f50a07" /># ECE3073 Mini Project
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

  ## Wiring Diagram
![Uploading<?xml version="1.0" encoding="UTF-8"?>
<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 1600 900" width="1600" height="900" font-family="'Helvetica Neue', Helvetica, Arial, sans-serif">

<style>
  .title    { font-size: 18px; font-weight: 600; fill: #1a1a1a; }
  .subtitle { font-size: 12px; font-weight: 400; fill: #555; }
  .header   { font-size: 14px; font-weight: 600; fill: #1a1a1a; }
  .label    { font-size: 13px; font-weight: 500; fill: #1a1a1a; }
  .pin      { font-size: 12px; font-weight: 500; fill: #1a1a1a; }
  .signal   { font-size: 12px; font-weight: 500; }
  .note     { font-size: 11px; fill: #666; }
  .col      { font-size: 12px; font-weight: 600; fill: #0c447c; }
  .secthdr  { font-size: 11px; font-weight: 600; fill: #888; letter-spacing: 1.5px; }
</style>

<!-- TITLE -->
<text class="title" x="40" y="40">DE10-Lite peripheral wiring · ECE3073</text>
<text class="subtitle" x="40" y="62">Grove Vision AI V2 → XIAO ESP32-C3 (I²C bridge) → DE10-Lite (SPI) → LEDs · Buzzer · VGA monitor</text>

<!-- LEGEND -->
<g transform="translate(900, 36)">
  <line x1="0" y1="0" x2="32" y2="0" stroke="#185FA5" stroke-width="2.5"/>
  <text class="signal" x="40" y="4" fill="#185FA5">I²C</text>
  <line x1="90" y1="0" x2="122" y2="0" stroke="#534AB7" stroke-width="2.5"/>
  <text class="signal" x="130" y="4" fill="#534AB7">SPI</text>
  <line x1="180" y1="0" x2="212" y2="0" stroke="#993C1D" stroke-width="2.5"/>
  <text class="signal" x="220" y="4" fill="#993C1D">5V</text>
  <line x1="260" y1="0" x2="292" y2="0" stroke="#5F5E5A" stroke-width="2.5" stroke-dasharray="5 4"/>
  <text class="signal" x="300" y="4" fill="#5F5E5A">GND</text>
</g>

<line x1="40" y1="80" x2="1560" y2="80" stroke="#d0d0d0" stroke-width="0.5"/>

<!-- ===== ROW 1: CAMERA -> XIAO (I2C link, top of page) ===== -->
<text class="secthdr" x="60" y="110">CAMERA MODULE</text>
<rect x="60" y="125" width="260" height="200" rx="14" fill="#EEEDFE" stroke="#534AB7" stroke-width="1"/>
<text class="header" x="80" y="155" fill="#26215C">Grove Vision AI V2</text>
<text class="subtitle" x="80" y="174" fill="#3C3489">Himax WE2 + Raspberry Pi cam</text>

<!-- Camera right-edge pins -->
<rect x="292" y="200" width="32" height="20" rx="3" fill="#fff" stroke="#185FA5" stroke-width="0.8"/>
<text class="pin" x="308" y="214" text-anchor="middle">SDA</text>
<rect x="292" y="226" width="32" height="20" rx="3" fill="#fff" stroke="#185FA5" stroke-width="0.8"/>
<text class="pin" x="308" y="240" text-anchor="middle">SCL</text>
<rect x="292" y="252" width="32" height="20" rx="3" fill="#fff" stroke="#993C1D" stroke-width="0.8"/>
<text class="pin" x="308" y="266" text-anchor="middle">5V</text>
<rect x="292" y="278" width="32" height="20" rx="3" fill="#fff" stroke="#5F5E5A" stroke-width="0.8"/>
<text class="pin" x="308" y="292" text-anchor="middle">GND</text>

<!-- XIAO block -->
<text class="secthdr" x="500" y="110">BRIDGE MCU</text>
<rect x="500" y="125" width="260" height="200" rx="14" fill="#E1F5EE" stroke="#0F6E56" stroke-width="1"/>
<text class="header" x="520" y="155" fill="#04342C">XIAO ESP32-C3</text>
<text class="subtitle" x="520" y="174" fill="#085041">SPI master · I²C master</text>

<!-- XIAO left-edge pins (I2C from camera) -->
<rect x="496" y="200" width="36" height="20" rx="3" fill="#fff" stroke="#185FA5" stroke-width="0.8"/>
<text class="pin" x="514" y="214" text-anchor="middle">D4</text>
<rect x="496" y="226" width="36" height="20" rx="3" fill="#fff" stroke="#185FA5" stroke-width="0.8"/>
<text class="pin" x="514" y="240" text-anchor="middle">D5</text>
<rect x="496" y="252" width="36" height="20" rx="3" fill="#fff" stroke="#993C1D" stroke-width="0.8"/>
<text class="pin" x="514" y="266" text-anchor="middle">5V</text>
<rect x="496" y="278" width="36" height="20" rx="3" fill="#fff" stroke="#5F5E5A" stroke-width="0.8"/>
<text class="pin" x="514" y="292" text-anchor="middle">GND</text>

<!-- I2C wires: camera -> XIAO (straight horizontal) -->
<line x1="324" y1="210" x2="496" y2="210" stroke="#185FA5" stroke-width="2"/>
<text class="signal" x="410" y="202" text-anchor="middle" fill="#0C447C">SDA · GPIO6</text>

<line x1="324" y1="236" x2="496" y2="236" stroke="#185FA5" stroke-width="2"/>
<text class="signal" x="410" y="222" text-anchor="middle" fill="#0C447C">SCL · GPIO7</text>

<line x1="324" y1="262" x2="496" y2="262" stroke="#993C1D" stroke-width="2"/>

<line x1="324" y1="288" x2="496" y2="288" stroke="#5F5E5A" stroke-width="2" stroke-dasharray="5 4"/>

<!-- XIAO bottom-edge pins (SPI down to DE10) -->
<rect x="540" y="325" width="22" height="32" rx="3" fill="#fff" stroke="#534AB7" stroke-width="0.8"/>
<text class="pin" x="551" y="345" text-anchor="middle">D10</text>

<rect x="572" y="325" width="22" height="32" rx="3" fill="#fff" stroke="#534AB7" stroke-width="0.8"/>
<text class="pin" x="583" y="345" text-anchor="middle">D9</text>

<rect x="604" y="325" width="22" height="32" rx="3" fill="#fff" stroke="#534AB7" stroke-width="0.8"/>
<text class="pin" x="615" y="345" text-anchor="middle">D8</text>

<rect x="636" y="325" width="22" height="32" rx="3" fill="#fff" stroke="#534AB7" stroke-width="0.8"/>
<text class="pin" x="647" y="345" text-anchor="middle">D7</text>

<rect x="668" y="325" width="32" height="32" rx="3" fill="#fff" stroke="#5F5E5A" stroke-width="0.8"/>
<text class="pin" x="684" y="345" text-anchor="middle">GND</text>

<!-- ===== ROW 2: DE10-LITE BLOCK ===== -->
<text class="secthdr" x="60" y="395">FPGA · DE10-LITE</text>
<rect x="60" y="410" width="1240" height="440" rx="14" fill="#E6F1FB" stroke="#185FA5" stroke-width="1"/>
<text class="header" x="80" y="440" fill="#042C53">MAX 10 FPGA pin assignments</text>
<text class="subtitle" x="80" y="459" fill="#0C447C">Intel Quartus · GPIO header JP1</text>

<!-- TOP edge of DE10: SPI input pins (receiving from XIAO above) -->
<rect x="540" y="403" width="22" height="22" rx="3" fill="#fff" stroke="#534AB7" stroke-width="0.8"/>
<text class="pin" x="551" y="394" text-anchor="middle">W9</text>

<rect x="572" y="403" width="22" height="22" rx="3" fill="#fff" stroke="#534AB7" stroke-width="0.8"/>
<text class="pin" x="583" y="394" text-anchor="middle">W8</text>

<rect x="604" y="403" width="22" height="22" rx="3" fill="#fff" stroke="#534AB7" stroke-width="0.8"/>
<text class="pin" x="615" y="394" text-anchor="middle">W7</text>

<rect x="636" y="403" width="22" height="22" rx="3" fill="#fff" stroke="#534AB7" stroke-width="0.8"/>
<text class="pin" x="647" y="394" text-anchor="middle">V5</text>

<rect x="668" y="403" width="32" height="22" rx="3" fill="#fff" stroke="#5F5E5A" stroke-width="0.8"/>
<text class="pin" x="684" y="394" text-anchor="middle">GND</text>

<!-- SPI vertical wires: XIAO bottom -> DE10 top (straight verticals, no crossing) -->
<line x1="551" y1="357" x2="551" y2="403" stroke="#534AB7" stroke-width="2"/>
<line x1="583" y1="357" x2="583" y2="403" stroke="#534AB7" stroke-width="2"/>
<line x1="615" y1="357" x2="615" y2="403" stroke="#534AB7" stroke-width="2"/>
<line x1="647" y1="357" x2="647" y2="403" stroke="#534AB7" stroke-width="2"/>
<line x1="684" y1="357" x2="684" y2="403" stroke="#5F5E5A" stroke-width="2" stroke-dasharray="5 4"/>

<!-- SPI signal labels next to vertical wires -->
<text class="signal" x="551" y="382" text-anchor="middle" fill="#534AB7" font-size="10">MOSI</text>
<text class="signal" x="583" y="382" text-anchor="middle" fill="#534AB7" font-size="10">MISO</text>
<text class="signal" x="615" y="382" text-anchor="middle" fill="#534AB7" font-size="10">SCK</text>
<text class="signal" x="647" y="382" text-anchor="middle" fill="#534AB7" font-size="10">SS</text>

<!-- ====== TWO-COLUMN TABLE INSIDE DE10 BLOCK ====== -->
<!-- LEFT half: SPI inputs (no peripheral wires) -->
<!-- RIGHT half: peripheral outputs (wires exit right) -->

<!-- LEFT TABLE: SPI -->
<text class="secthdr" x="80" y="495" fill="#0C447C">SPI INPUT BUS</text>
<text class="col" x="95" y="525">Signal</text>
<text class="col" x="220" y="525" text-anchor="middle">FPGA pin</text>
<text class="col" x="340" y="525" text-anchor="middle">GPIO_[]</text>
<text class="col" x="440" y="525" text-anchor="middle">JP1</text>
<line x1="80" y1="535" x2="500" y2="535" stroke="#185FA5" stroke-width="0.5" opacity="0.5"/>

<rect x="80" y="545" width="420" height="28" rx="4" fill="#fff" stroke="#534AB7" stroke-width="0.6"/>
<text class="label" x="95" y="564" fill="#534AB7">SPI_MOSI</text>
<text class="label" x="220" y="564" text-anchor="middle">PIN_W9</text>
<text class="label" x="340" y="564" text-anchor="middle">GPIO_[3]</text>
<text class="label" x="440" y="564" text-anchor="middle">pin 4</text>

<rect x="80" y="578" width="420" height="28" rx="4" fill="#fff" stroke="#534AB7" stroke-width="0.6"/>
<text class="label" x="95" y="597" fill="#534AB7">SPI_MISO</text>
<text class="label" x="220" y="597" text-anchor="middle">PIN_W8</text>
<text class="label" x="340" y="597" text-anchor="middle">GPIO_[5]</text>
<text class="label" x="440" y="597" text-anchor="middle">pin 6</text>

<rect x="80" y="611" width="420" height="28" rx="4" fill="#fff" stroke="#534AB7" stroke-width="0.6"/>
<text class="label" x="95" y="630" fill="#534AB7">SPI_SCK</text>
<text class="label" x="220" y="630" text-anchor="middle">PIN_W7</text>
<text class="label" x="340" y="630" text-anchor="middle">GPIO_[7]</text>
<text class="label" x="440" y="630" text-anchor="middle">pin 8</text>

<rect x="80" y="644" width="420" height="28" rx="4" fill="#fff" stroke="#534AB7" stroke-width="0.6"/>
<text class="label" x="95" y="663" fill="#534AB7">SPI_SS</text>
<text class="label" x="220" y="663" text-anchor="middle">PIN_V5</text>
<text class="label" x="340" y="663" text-anchor="middle">GPIO_[9]</text>
<text class="label" x="440" y="663" text-anchor="middle">pin 10</text>

<rect x="80" y="677" width="420" height="28" rx="4" fill="#fff" stroke="#5F5E5A" stroke-width="0.6"/>
<text class="label" x="95" y="696" fill="#5F5E5A">GND (shared)</text>
<text class="label" x="220" y="696" text-anchor="middle">—</text>
<text class="label" x="340" y="696" text-anchor="middle">GND</text>
<text class="label" x="440" y="696" text-anchor="middle">pin 12</text>

<!-- RIGHT TABLE: Peripheral outputs -->
<text class="secthdr" x="700" y="495" fill="#0C447C">PERIPHERAL OUTPUTS</text>
<text class="col" x="715" y="525">Signal</text>
<text class="col" x="860" y="525" text-anchor="middle">FPGA pin</text>
<text class="col" x="990" y="525" text-anchor="middle">GPIO_[]</text>
<text class="col" x="1110" y="525" text-anchor="middle">JP1</text>
<line x1="700" y1="535" x2="1180" y2="535" stroke="#185FA5" stroke-width="0.5" opacity="0.5"/>

<!-- Buzzer Y5 -->
<rect x="700" y="545" width="480" height="28" rx="4" fill="#fff" stroke="#185FA5" stroke-width="0.6"/>
<text class="label" x="715" y="564" fill="#185FA5">Buzzer</text>
<text class="label" x="860" y="564" text-anchor="middle">PIN_Y5</text>
<text class="label" x="990" y="564" text-anchor="middle">GPIO_[29]</text>
<text class="label" x="1110" y="564" text-anchor="middle">pin 34</text>
<circle cx="1180" cy="559" r="4" fill="#185FA5"/>

<!-- Green LED Y4 -->
<rect x="700" y="578" width="480" height="28" rx="4" fill="#fff" stroke="#3B6D11" stroke-width="0.6"/>
<text class="label" x="715" y="597" fill="#3B6D11">Green LED</text>
<text class="label" x="860" y="597" text-anchor="middle">PIN_Y4</text>
<text class="label" x="990" y="597" text-anchor="middle">GPIO_[31]</text>
<text class="label" x="1110" y="597" text-anchor="middle">pin 36</text>
<circle cx="1180" cy="592" r="4" fill="#3B6D11"/>

<!-- Red LED Y3 -->
<rect x="700" y="611" width="480" height="28" rx="4" fill="#fff" stroke="#A32D2D" stroke-width="0.6"/>
<text class="label" x="715" y="630" fill="#A32D2D">Red LED</text>
<text class="label" x="860" y="630" text-anchor="middle">PIN_Y3</text>
<text class="label" x="990" y="630" text-anchor="middle">GPIO_[33]</text>
<text class="label" x="1110" y="630" text-anchor="middle">pin 38</text>
<circle cx="1180" cy="625" r="4" fill="#A32D2D"/>

<!-- Yellow LED AA2 -->
<rect x="700" y="644" width="480" height="28" rx="4" fill="#fff" stroke="#854F0B" stroke-width="0.6"/>
<text class="label" x="715" y="663" fill="#854F0B">Yellow LED</text>
<text class="label" x="860" y="663" text-anchor="middle">PIN_AA2</text>
<text class="label" x="990" y="663" text-anchor="middle">—</text>
<text class="label" x="1110" y="663" text-anchor="middle">—</text>
<circle cx="1180" cy="658" r="4" fill="#854F0B"/>

<!-- VGA -->
<rect x="700" y="677" width="480" height="28" rx="4" fill="#fff" stroke="#5F5E5A" stroke-width="0.6"/>
<text class="label" x="715" y="696" fill="#5F5E5A">VGA monitor</text>
<text class="label" x="860" y="696" text-anchor="middle">D-sub on-board</text>
<text class="label" x="990" y="696" text-anchor="middle">—</text>
<text class="label" x="1110" y="696" text-anchor="middle">—</text>
<circle cx="1180" cy="691" r="4" fill="#5F5E5A"/>

<!-- ===== PERIPHERAL BLOCKS (right side, aligned to their row) ===== -->
<!-- Each peripheral sits directly to the right of its row -->

<!-- Buzzer (aligned to y=559) -->
<line x1="1180" y1="559" x2="1340" y2="559" stroke="#185FA5" stroke-width="2"/>
<rect x="1340" y="540" width="200" height="40" rx="8" fill="#E6F1FB" stroke="#185FA5" stroke-width="1"/>
<text class="header" x="1440" y="565" text-anchor="middle" fill="#042C53">Buzzer</text>

<!-- Green LED (aligned to y=592) -->
<line x1="1180" y1="592" x2="1340" y2="592" stroke="#3B6D11" stroke-width="2"/>
<rect x="1340" y="582" width="200" height="22" rx="6" fill="#EAF3DE" stroke="#3B6D11" stroke-width="1"/>
<text class="header" x="1440" y="599" text-anchor="middle" fill="#173404">Green LED</text>

<!-- Red LED (aligned to y=625) -->
<line x1="1180" y1="625" x2="1340" y2="625" stroke="#A32D2D" stroke-width="2"/>
<rect x="1340" y="614" width="200" height="22" rx="6" fill="#FCEBEB" stroke="#A32D2D" stroke-width="1"/>
<text class="header" x="1440" y="631" text-anchor="middle" fill="#501313">Red LED</text>

<!-- Yellow LED (aligned to y=658) -->
<line x1="1180" y1="658" x2="1340" y2="658" stroke="#854F0B" stroke-width="2"/>
<rect x="1340" y="647" width="200" height="22" rx="6" fill="#FAEEDA" stroke="#854F0B" stroke-width="1"/>
<text class="header" x="1440" y="664" text-anchor="middle" fill="#412402">Yellow LED</text>

<!-- VGA Monitor (aligned to y=691) -->
<line x1="1180" y1="691" x2="1340" y2="691" stroke="#5F5E5A" stroke-width="2"/>
<rect x="1340" y="671" width="200" height="40" rx="8" fill="#F1EFE8" stroke="#5F5E5A" stroke-width="1"/>
<text class="header" x="1440" y="696" text-anchor="middle" fill="#2C2C2A">Monitor (VGA)</text>

<!-- NOTES -->
<text class="note" x="40" y="880">XIAO is SPI master, DE10-Lite is SPI slave. I²C bus runs only between XIAO and Grove Vision AI V2.</text>
<text class="note" x="40" y="895">PIN_AA2 (yellow LED) is not on JP1 in the original pinout — verify .qsf for actual header / LEDR mapping.</text>

</svg>
 de10_lite_wiring.svg…]()


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

This project is developed for academic purposes as part of **ECE3073** at Monash University.
