/***************************************************************************//**
  @file     pdb.h
  @brief    Programmable Delay Block (PDB) driver for K64F
  @author   Group 4: - Oms, Mariano
                     - Solari Raigoso, Agustín
                     - Wickham, Tomás
                     - Vieira, Valentin Ulises
 ******************************************************************************/

#ifndef _PDB_H_
#define _PDB_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef enum {
    PDB0_ID,
    PDB1_ID,

    PDB_CANT_IDS
} pdb_id_t;

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Initialize the PDB module
 * @param id PDB module ID
 * @return true if initialization was successful, false otherwise
 */
bool PDB_Init(pdb_id_t id);

/**
 * @brief Start the PDB module
 * @param id PDB module ID
 */
void PDB_Start(pdb_id_t id);

/**
 * @brief Stop the PDB module
 * @param id PDB module ID
 */
void PDB_Stop(pdb_id_t id);

/**
 * @brief Set the delay value for the PDB module
 * @param id PDB module ID
 * @param delay Delay value in microseconds
 */
void PDB_SetDelay(pdb_id_t id, uint32_t delay);

/*******************************************************************************
 ******************************************************************************/

#endif // _PDB_H_
