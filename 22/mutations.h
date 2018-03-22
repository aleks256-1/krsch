#ifndef MUTATIONS_H
#define MUTATIONS_H

void ChangeRandomBits(struct uint16_genome * genome);
void RandomFragmentDeletion(struct uint16_genome * genome);
void RandomFragmentInsetrion(struct uint16_genome * genome);
void RandomFragmentDuplicate(struct uint16_genome * genome);
void RandomFragmentMove(struct uint16_genome * genome);
void RandomFragmentCopy(struct uint16_genome * genome);
void ExchangeDnaFragments(struct uint16_genome * genome1, struct uint16_genome * genome2);
void RandomFragmentReverse(struct uint16_genome * genome);
#endif