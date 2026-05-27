/*
 * 7seg.h
 *
 *  Created on: May 13, 2026
 *      Author: Aluno
 */

#ifndef INC_7SEG_H_
#define INC_7SEG_H_

void configurePc8ToPc11(void);
void test7Seg(void);
void display7Seg(int, int, int);
void prepare7SegDigits(int);
void refreshSevenSegDisplays(void);

#endif /* INC_7SEG_H_ */
