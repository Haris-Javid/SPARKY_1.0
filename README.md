# SPRAKY1.0
STM32 BASED DRONE FLIGHT CONTROLLER 

<img src="https://github.com/user-attachments/assets/b2dcd65b-607d-4165-a58b-efb39a23741e" width="300">
<img src="https://github.com/user-attachments/assets/b36cc964-b575-4fdf-ae64-d7db82cce889" width="285">

<img src="https://github.com/user-attachments/assets/9065804d-552e-499d-bf0a-607d8e42f1c3" width="300">
<img src="https://github.com/user-attachments/assets/c216b574-9ff9-4cd1-9e73-eec270dfd407" width="300">


## PCB POWER SUPPLY 

### Table estimating power consumption of major components
| Component   | Operating current | 1.25 safety factor  |
| ------------- | ------------- | ------------- |
| Brushed motors 8520 | 3A*4 = 12 A  | 12A  |
| STM32F411CEU6TR | 30 mA  |  37.5 mA  |
| NRF24L01P-R | 13.5 mA  | 16.875 mA  |
| QMC5883P | 2.2 mA  | 2.75 mA  |
| BMP581 | 0.26 mA  | 0.325 mA  |
| BMI088 | 5.15 mA  | 6.4375 mA  |
| IP5306 | 3 mA  | 3.75 mA  |
| MIC5365-3.3YD5-TR | 29 uA | 36.25 uA |


### High Level Power Supply Schematic
![Image](https://github.com/user-attachments/assets/e67e70c4-7239-43a3-a4e8-4d791ccce274)


## TYPICAL BRUSHED MOTOR CONTROLLER

The STM32 GPIO port outputs a PWM signal to the GATE pin of the N-channel MOSFET, allowing it to switch on and off according to the PWM duty cycle. When the MOSFET is on, current flows from the M4_POS pin through the motor, into the M4_NEG pin, and passes through the MOSFET’s drain-to-source path to ground. A flyback diode is placed across the motor to suppress voltage spikes and block reverse current, ensuring safe energy dissipation when the motor spins without power. A 100 Ω resistor is connected in series with the GATE to limit the inrush current and reduce power usage. In contrast, a 5 kΩ pull-down resistor is connected to the PWM line to prevent unintended switching due to floating signals.

![Image](https://github.com/user-attachments/assets/b17fa8ce-dfe2-4bfa-8981-9aa05600d982)

## nRF24L01

### SPI Settings: 
Frame Foramt: Motorola

Data Size: 8 Bits

First Bit: MSB First

Clock Polarity: 0

Clock Phase: 1

Max speed 2Mbps at 2.4 Ghz
