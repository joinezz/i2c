1.  Initialisierung des I2C-Moduls: Bevor die Kommunikation beginnt, wird das I2C1-Modul des STM32 initialisiert. 
    Dies erfolgt über die Funktion i2c_master_setup(I2C1, I2C_STANDARD).
    Das Standard-I2C-Protokoll wird verwendet.

2.  Transmit-Funktion (i2c_master_transmit): Die Funktion i2c_master_transmit(I2C1, add_slaveX, data, 1); wird verwendet, um Daten an die I2C-Slave-Geräte zu senden. #
    add_slaveX ist die Adresse des Slave-Geräts, an das Daten gesendet werden sollen (entweder add_slave1 oder add_slave2).
    data ist ein Array, das die zu sendenden Daten enthält. Der Parameter 1 gibt an, dass ein Byte Daten gesendet wird.

3.  Receive-Funktion (i2c_master_receive): Die Funktion i2c_master_receive(I2C1, add_slaveX, data_received, 1); wird verwendet, um Daten von den I2C-Slave-Geräten zu empfangen.
    add_slaveX ist die Adresse des Slave-Geräts, von dem Daten empfangen werden sollen.
    data_received ist ein Array, in das die empfangenen Daten geschrieben werden. Der Parameter 1 gibt an, dass ein Byte Daten empfangen wird.

4.  Ablauf der Befehlsausführung: Im Code wird je nach empfangenem Befehl (SERIAL_COMMAND_t) entschieden, welcher Slave und welche Aktion ausgeführt werden sollen.
    Zum Beispiel wird bei CMD_LED11_ON oder CMD_LED12_ON das entsprechende LED-Befehlsbyte in data[0] gespeichert und dann über I2C an den entsprechenden Slave gesendet.
    Nach dem Senden von Befehlen wie LED-Ein-/Ausschalten (CMD_LED11_ON, CMD_LED11_OFF, usw.) werden Rückmeldungen von den Slaves über die State-Befehle (CMD_slave1_state, CMD_slave2_state) empfangen,
    um den aktuellen Zustand der LEDs abzufragen.

5.  Behandlung von Datenrückmeldungen: Die empfangenen Daten werden analysiert, um den aktuellen Zustand der LEDs auf den Slave-Geräten zu bestimmen.
    Basierend auf den empfangenen Daten werden entsprechende Debug-Nachrichten über UART ausgegeben, um den aktuellen Zustand der LEDs anzuzeigen.
