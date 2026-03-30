# HAOS

Hydroponic Agriculture Automation System.

HAOS is a Raspberry Pi based automation and monitoring platform for hydroponic cultivation. The repository combines low level C programs for sensor reading and actuator control, a MySQL data model for historical logs and alarms, and a small PHP web interface for reviewing collected measurements.

The codebase appears to have been developed as an academic integration project focused on greenhouse and hydroponic control. Even in its current form, it shows a complete end to end prototype: read water chemistry and environmental sensors, compare those values against crop targets, dose nutrients or pH correction when needed, store events in a database, and expose results in a browser.

## Project goals

HAOS is designed to automate repetitive hydroponic maintenance tasks:

- Read pH and EC values from Atlas Scientific style serial sensors connected through multiplexers.
- Read ambient temperature and humidity from an AM2302 / DHT22 sensor.
- Read several float switches for tank and cistern level detection.
- Store measurements in a local log file and in a MySQL database.
- Trigger alarms when readings or tank states are abnormal.
- Fill the nutrient tank automatically.
- Dose nutrient solution when EC is below the configured range.
- Dose pH up or pH down when pH drifts outside the configured range.
- Provide a simple browser interface for login and historical data review.

## Repository structure

- `src/`: main control software, calibration tools, SQL scripts, test programs, and historical versions.
- `src/final/`: the most coherent version of the main automation code.
- `web/`: PHP and HTML interface for login and measurement display.
- `files/`: helper executables and scripts used by the controller, including DHT reading utilities and log samples.
- `libraries/`: bundled third party code and headers such as wiringPi and Adafruit DHT libraries.
- `log/`: local measurement logs written by the controller.
- `rpiImage/`: Raspberry Pi image artifacts used during development.

## Architecture overview

The project is organized around three layers.

### 1. Field control layer

The C program in `src/final/control_inv.c` is the main runtime. It:

- opens the serial device at `/dev/ttyAMA0`
- initializes WiringPi in sys mode
- switches multiplexers to read different sensors
- reads pH and EC over UART
- reads temperature and humidity through an external DHT utility
- reads digital tank level signals from float switches
- writes measurements to `../log/reading.log`
- inserts measurements and alarms into MySQL
- decides whether to add water, nutrients, pH minus, or pH plus

### 2. Data layer

The SQL scripts in `src/db/` define tables for:

- crops and their optimal ranges
- users
- sensor catalog
- cultivation records
- sensor to cultivation relationships
- actuator to cultivation relationships
- historical sensor logs
- alarms

The active code expects a database named `control_inv` and credentials hardcoded in the C and PHP sources.

### 3. Web layer

The `web/` folder contains a minimal PHP frontend:

- `index.php`: login page
- `checklogin.php`: username/password validation
- `printdata.php`: renders logged pH, EC, temperature, humidity, and alarms

This interface is simple, but it demonstrates the intended operational workflow: HAOS runs continuously on the Pi, stores readings in MySQL, and a local browser displays the system state.

## Hardware requirements

The code makes several hardware assumptions. Based on the pin definitions and control routines, a practical HAOS build requires the following.

### Core hardware

- Raspberry Pi with GPIO and UART available
- microSD card with Linux installed
- stable 5V power supply for the Raspberry Pi
- network connectivity if you want to access the web dashboard remotely

### Sensors

- 1 x pH sensor with UART interface, likely Atlas Scientific style
- 1 x EC sensor with UART interface, likely Atlas Scientific style
- 1 x AM2302 / DHT22 temperature and humidity sensor
- 4 x float switches for liquid level monitoring:
  - high tank level
  - low tank level
  - minimum cistern level
  - low cistern level

### Actuators

- 8 channel relay board
- 1 x water fill pump
- 1 x nutrient dosing pump
- 1 x pH minus dosing pump
- 1 x pH plus dosing pump
- optional spare relay channels for future actuators

### Signal routing and interfacing

- 1 x 4 channel multiplexer for serial sensor selection
- 1 x 16 channel multiplexer for digital input selection
- proper level shifting or electrically compatible peripherals
- tubing, dosing lines, and hydroponic reservoirs

### Recommended supporting equipment

- pH calibration solution
- EC calibration solution
- protected enclosure for the Raspberry Pi, relay board, and wiring
- fused power distribution for pumps and relays
- manual kill switch or emergency disconnect

## GPIO map

The pin mapping is defined in `src/final/control_inv.h`.

### Multiplexer control

- GPIO 4: `_mux1_S0`
- GPIO 3: `_mux1_S1`
- GPIO 17: `_mux2_S0`
- GPIO 27: `_mux2_S1`
- GPIO 22: `_mux2_S2`
- GPIO 23: `_mux2_S3`
- GPIO 24: `_mux2_SIG` input

### Relay outputs

- GPIO 5: relay IN1, pH plus
- GPIO 6: relay IN2, pH minus
- GPIO 13: relay IN3, nutrient pump
- GPIO 19: relay IN4
- GPIO 26: relay IN5
- GPIO 16: relay IN6
- GPIO 20: relay IN7
- GPIO 21: relay IN8, tank filling pump

Important behavior:

- the code assumes active low relays
- `safe_state()` drives all relay outputs HIGH to keep actuators off

## Software requirements

This project reflects an older Raspberry Pi / LAMP stack and will likely run most easily in a legacy environment or after some modernization.

### Required packages

- GCC and `make`
- WiringPi development libraries
- MySQL or MariaDB server
- MySQL client development headers for compilation
- PHP with MySQL support
- a Linux environment on Raspberry Pi with GPIO sysfs access and UART enabled

### Runtime assumptions in the code

- serial device exists at `/dev/ttyAMA0`
- the DHT utility exists at `../files/DHT`
- the database is reachable at `localhost`
- MySQL user is `ctrl_inv`
- MySQL password is `password`
- database name is `control_inv`

## Installation

The project does not currently provide a one command installer, so setup is manual. The sequence below is based on the repository contents and code expectations.

### 1. Clone the repository

```bash
git clone <repo-url> HAOS
cd HAOS
```

### 2. Install system dependencies

On a Raspberry Pi Debian based system, install the build and web stack packages you need.

```bash
sudo apt update
sudo apt install build-essential mariadb-server mariadb-client libmariadb-dev php php-mysql
```

If your target OS still packages WiringPi:

```bash
sudo apt install wiringpi
```

If not, you will need to install a compatible GPIO library manually or adapt the code to `libgpiod`.

### 3. Enable serial and GPIO access

Make sure the hardware UART is enabled and available as `/dev/ttyAMA0`. On many Raspberry Pi systems this also requires disabling the serial login console.

You also need permission to control GPIOs and serial devices. Several routines use `wiringPiSetupSys()` and sysfs GPIO access, so the simplest path during development is to run the controller with `sudo`.

### 4. Prepare the database

Create the database and import the schema.

```bash
mysql -u root -p
```

Then inside MySQL:

```sql
CREATE DATABASE control_inv;
CREATE USER 'ctrl_inv'@'localhost' IDENTIFIED BY 'password';
GRANT ALL PRIVILEGES ON control_inv.* TO 'ctrl_inv'@'localhost';
FLUSH PRIVILEGES;
```

Import the schema:

```bash
mysql -u root -p control_inv < src/db/script_control_Invernaderos.sql
```

Optionally review `src/db/scriptInvernadero2.sql` as an earlier or broader schema variant with actuator tables.

### 5. Seed crop configuration

The controller queries the `cHortaliza` table for crop operating ranges. Insert at least one crop row before running the controller. For example:

```sql
INSERT INTO cHortaliza (nombre, phMax, phMin, ecMax, ecMin, tempMax, tempMin, personalizado)
VALUES ('Jitomate', 6.5, 5.5, 2.5, 1.5, 30.0, 18.0, 0);
```

The current code defaults to crop ID `1`, so make sure the first usable row matches the crop you want to run.

### 6. Build the native binaries

The most relevant build target is in `src/final/`.

```bash
cd src/final
make
```

Expected binaries:

- `control_inv`
- `calibrar_atlas`
- `continous_reading`

If compilation fails, verify:

- `mysql_config` is present
- WiringPi headers and libraries are installed
- the include and linker paths match your system

### 7. Initialize GPIO lines

The repository includes `_init_GPIO.sh` to export GPIOs through sysfs and set relay outputs to a safe state.

```bash
cd src/final
sudo bash _init_GPIO.sh
```

This script:

- exports GPIO pins
- configures relay and multiplexer control lines as outputs
- configures GPIO 24 as input
- forces all relay outputs HIGH, which the project treats as OFF

### 8. Calibrate pH and EC probes

Run the calibration helper before starting full automation.

```bash
cd src/final
sudo ./calibrar_atlas
```

The calibration routine prompts you to place probes in calibration solutions and sends commands such as:

- `Cal,dry`
- `Cal,one,12.880`
- `Cal, mid, 7.00`

Use appropriate solutions and verify the sensor documentation for your exact probe model.

### 9. Start the controller

```bash
cd src/final
sudo ./control_inv
```

The controller will begin:

- reading sensors
- printing values to the terminal
- appending to `log/reading.log`
- inserting rows into `tLogLector`
- inserting alarms into `tAlarmas`
- actuating pumps when readings fall outside configured thresholds

### 10. Serve the web interface

Place the `web/` directory under your PHP capable web root, or configure your web server to point to it.

For a quick local test with PHP's built in server:

```bash
cd web
php -S 0.0.0.0:8000
```

Then open:

```text
http://<raspberry-pi-ip>:8000/index.php
```

The sample login form shows placeholder credentials in the page itself:

- username: `user`
- password: `password`

You will still need a matching row in the `sUser` table for login to succeed.

## Typical usage

Once installed and calibrated, a normal HAOS workflow looks like this:

1. Power the Raspberry Pi, sensors, relay board, and pumps.
2. Run `_init_GPIO.sh` to establish a safe output state.
3. Start `control_inv`.
4. Let the controller poll pH, EC, DHT, and level sensors.
5. Review live logs in the terminal or in `log/reading.log`.
6. Open the PHP dashboard to inspect historical values and alarms.
7. Recalibrate the probes regularly with `calibrar_atlas`.

## How control decisions are made

The current implementation uses direct threshold logic.

- If EC is above the configured maximum, the system attempts to add water.
- If EC is below the configured minimum, the system calculates a nutrient pump run time and doses nutrient solution.
- If pH is above the configured maximum, the system doses pH minus.
- If pH is below the configured minimum, the system doses pH plus.
- If float switch combinations are inconsistent, the system raises alarms.
- If the tank is low and the cistern has water, the system can fill the tank automatically.

This makes HAOS suitable as:

- a hydroponic nutrient tank supervisor
- a teaching platform for automation and embedded systems
- a prototype for greenhouse telemetry and actuation
- a base for a more modern IoT or Home Assistant integration

## Logs and data output

HAOS records data in two places.

### File logs

The controller writes a tab separated log to:

- `log/reading.log`

The format includes:

- date
- time
- pH
- EC
- temperature
- humidity

### Database logs

The controller inserts data into:

- `tLogLector` for measurements
- `tAlarmas` for abnormal events and warnings

The web interface reads directly from these tables.

## Current limitations

This repository is valuable as a prototype, but it should be treated carefully in production.

- Credentials are hardcoded in source files.
- Legacy `mysql_*` PHP functions are used in `web/checklogin.php`.
- The web login stores and compares passwords in plain text.
- Actuator timings are fixed and require empirical validation.
- The main loop runs continuously without service supervision.
- Error handling and shutdown logic are minimal.
- The code assumes specific sensor addresses, serial wiring, GPIO assignments, and crop IDs.
- WiringPi is deprecated and may not be available on modern Raspberry Pi OS releases.
- The system uses old sysfs GPIO export logic, which is deprecated on recent Linux kernels.
- There is no installation script, provisioning script, or automated test coverage.

## Possible project uses

Depending on your goals, HAOS can be used in several ways.

### Educational prototype

Use it to study:

- Raspberry Pi hardware interfacing
- serial sensor communication
- relay based automation
- database backed telemetry systems
- basic control logic for hydroponics

### Research and experimentation

Use it as a starting point for:

- nutrient dosing strategies
- pH stabilization experiments
- greenhouse monitoring research
- alarm and event modeling

### Practical grow controller

With hardware validation and software hardening, HAOS could support:

- small hydroponic racks
- greenhouse proof of concept systems
- demonstration setups in labs or classrooms

### Modernization base

The repository is also a useful migration target for:

- Python or Rust based controller rewrites
- MQTT or Home Assistant integration
- modern web dashboards
- Dockerized local services
- sensor abstraction and hardware simulation layers

## Suggested next improvements

The most impactful improvements would be:

- replace hardcoded credentials with environment based configuration
- migrate deprecated PHP and GPIO dependencies
- add a systemd service for unattended startup
- add seed SQL data and a reproducible installer
- document the exact wiring diagram
- add relay and pump safety interlocks
- support more crops and runtime configuration
- add graphs instead of raw HTML tables
- store calibration history
- add tests for dosing calculations and alarm logic

## Future vision

Beyond stabilizing the current prototype, HAOS could evolve into a broader intelligent cultivation and traceability platform.

### AI assisted crop health monitoring

One natural extension is AI based pest and disease detection. With camera modules or external IP cameras, HAOS could capture plant images at regular intervals and run computer vision models to detect:

- early pest presence
- leaf discoloration or spotting
- growth anomalies
- nutrient stress indicators
- visible mold or fungal development

Those detections could be stored alongside pH, EC, temperature, and humidity readings so the system provides both environmental telemetry and visual crop health signals in the same dashboard.

### Fair trade and traceability network

Another strategic direction is blockchain backed traceability. Instead of using HAOS only as a local controller, cultivation events could also become signed production records for a fair trade produce network.

In that model, HAOS could record and export:

- crop configuration and growth cycles
- calibration checkpoints
- irrigation and dosing events
- alarm history
- harvest timestamps
- operator or certification actions

Anchoring those records to a blockchain or other tamper evident ledger could help growers, buyers, certifiers, and cooperatives verify provenance, production practices, and quality milestones more transparently.

### Combined platform opportunity

Taken together, AI pest detection and traceable cultivation records would move HAOS from a local hydroponic controller toward a larger agricultural platform that supports:

- smarter crop monitoring
- earlier intervention on plant health issues
- stronger trust between growers and buyers
- fair trade verification workflows
- data driven marketplace participation for small and mid sized producers

## Contributing

Contributions are welcome. This repository would benefit from both hardware aware contributors and software maintainers.

Useful contribution areas:

- documenting sensor wiring and relay connections
- modernizing the build and dependency setup
- replacing deprecated libraries
- improving calibration workflows
- hardening the web interface and authentication
- adding deployment scripts
- writing unit and integration tests
- improving database migrations and seed data
- translating comments and docs into clearer English or Spanish

When contributing, it helps to include:

- the Raspberry Pi model used
- the OS version
- the exact sensor and relay hardware
- photos or diagrams of the wiring
- sample logs or SQL rows that reproduce the issue

## Call for participation

HAOS is exactly the kind of project that becomes more valuable when people with different backgrounds collaborate on it. If you work with embedded Linux, hydroponics, greenhouse automation, electronics, sensors, databases, or web dashboards, your input can directly improve the system.

There is room here for:

- growers who can validate dosing logic against real cultivation practice
- electronics builders who can refine wiring and protections
- embedded developers who can modernize GPIO and serial handling
- backend developers who can clean up schema management and logging
- frontend developers who can turn the raw PHP pages into a usable monitoring console
- technical writers who can add setup guides, wiring diagrams, and troubleshooting notes

If you want to help, open an issue, document your setup, propose a refactor, or submit a patch. Even small contributions such as better install notes, corrected pin maps, improved SQL seeds, or sensor calibration documentation will make HAOS easier to run and easier to trust.

## License

No top level project license is currently declared in this repository. If you plan to redistribute or substantially reuse the code, clarify licensing first with the project owner or add an explicit license in a future update.
