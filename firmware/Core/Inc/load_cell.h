/*
 * load_cell.h
 *
 *  Created on: Jun 10, 2026
 *      Author: lucas
 */

#ifndef INC_LOAD_CELL_H_
#define INC_LOAD_CELL_H_

void LoadCellInit(void);
void LoadCellRead(void);
float LoadCellGetLastReading(void);
float LoadCellGetReadingRate(void);

#endif /* INC_LOAD_CELL_H_ */
