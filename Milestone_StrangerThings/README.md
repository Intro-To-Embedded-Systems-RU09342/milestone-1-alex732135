# Milestone 1: Communicating with Will Byers

This Milestone implements things utilized in the previous labs to combine into something more meaningful, albeit overengineered. Each MSP430 takes an input code of an undetermined number of bytes following a specific format. These bytes are recieved via UART, processed, and then sent to the next board via UART. The bytes determine which color the LED turns on, by controlling the amount of Red, Green and Blue for the LED. 
