#ifndef _FSKDEM_H_
#define _FSKDEM_H_

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Initializes the FSK demodulator by setting up the necessary hardware components and configurations.
 * This includes initializing the FTM and DMA modules, configuring the comparator, setting up the input capture mode,
 * and enabling DMA requests and interrupts.
 */
void initFSKDemodulator();

/**
 * @brief Checks if the processing of the current word has finished by verifying if the output queue is not empty.
 * @return true if data is ready, false otherwise.
 */
bool isDataReadyHART();

/**
 * @brief Retrieves the next value from the output queue.
 * @return The next character value from the output queue.
 */
char getNextValueHART(void);


#endif /* void _FSKDEM_H_ */
