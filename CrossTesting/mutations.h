#ifndef MUTATIONS_H
#define MUTATIONS_H

void ChangeRandomBits(struct genome * genome);
void RandomFragmentDeletion(struct genome * genome);
void RandomFragmentInsetrion(struct genome * genome);
void RandomFragmentDuplicate(struct genome * genome);
void RandomFragmentMove(struct genome * genome);
void RandomFragmentCopy(struct genome * genome);

#endif