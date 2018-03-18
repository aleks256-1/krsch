#ifndef EMBRIOGENESIS_H
#define EMBRIOGENESIS_H

float calc_sigma(float x);
void calculateDvForCells_v2(struct creature * creature, struct genome * genome);
void init_blur_matrix(struct matrix ** matrix);
void diff_v2(struct creature * creature, struct matrix * matrix);

#endif